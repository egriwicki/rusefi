/**
 * @file	mmc_card.cpp
 *
 * @date Dec 28, 2013
 * @author Kot_dnz
 * @author Andrey Belomutskiy, (c) 2012-2020
 *
 * default pinouts in case of SPI2 connected to MMC: PB13 - SCK, PB14 - MISO, PB15 - MOSI, PD4 - CS, 3.3v
 * default pinouts in case of SPI3 connected to MMC: PB3  - SCK, PB4  - MISO, PB5  - MOSI, PD4 - CS, 3.3v
 *
 *
 * todo: extract some logic into a controller file
 */

#include "global.h"

#if EFI_FILE_LOGGING

#include <stdio.h>
#include <string.h>
#include "mmc_card.h"
#include "pin_repository.h"
#include "ff.h"
#include "hardware.h"
#include "engine_configuration.h"
#include "status_loop.h"
#include "buffered_writer.h"
#include "null_device.h"

#include "rtc_helper.h"

#define SD_STATE_INIT "init"
#define SD_STATE_MOUNTED "MOUNTED"
#define SD_STATE_MOUNT_FAILED "MOUNT_FAILED"
#define SD_STATE_OPEN_FAILED "OPEN_FAILED"
#define SD_STATE_SEEK_FAILED "SEEK_FAILED"
#define SD_STATE_NOT_INSERTED "NOT_INSERTED"
#define SD_STATE_CONNECTING "CONNECTING"
#define SD_STATE_NOT_CONNECTED "NOT_CONNECTED"

static const char *sdStatus = SD_STATE_INIT;
static bool fs_ready = false;

EXTERN_ENGINE;

#define F_SYNC_FREQUENCY 100

static int totalLoggedBytes = 0;
static int fileCreatedCounter = 0;
static int writeCounter = 0;
static int totalWritesCounter = 0;
static int totalSyncCounter = 0;

/**
 * on't re-read SD card spi device after boot - it could change mid transaction (TS thread could preempt),
 * which will cause disaster (usually multiple-unlock of the same mutex in UNLOCK_SD_SPI)
 */

spi_device_e mmcSpiDevice = SPI_NONE;

#define LOG_INDEX_FILENAME "index.txt"

#define RUSEFI_LOG_PREFIX "re_"
#define PREFIX_LEN 3
#define SHORT_TIME_LEN 13

#define LS_RESPONSE "ls_result"
#define FILE_LIST_MAX_COUNT 20

#if HAL_USE_USB_MSD
#include "hal_usb_msd.h"
#if STM32_USB_USE_OTG2
  USBDriver *usb_driver = &USBD2;
#else
  USBDriver *usb_driver = &USBD1;
#endif
extern const USBConfig msdusbcfg;
#endif /* HAL_USE_USB_MSD */

static THD_WORKING_AREA(mmcThreadStack,3 * UTILITY_THREAD_STACK_SIZE);		// MMC monitor thread

/**
 * MMC driver instance.
 */
MMCDriver MMCD1;

// SD cards are good up to 25MHz in "slow" mode, and 50MHz in "fast" mode
// 168mhz F4:
// Slow mode is 10.5 or 5.25 MHz, depending on which SPI device
// Fast mode is 42 or 21 MHz
// 216mhz F7:
// Slow mode is 13.5 or 6.75 MHz
// Fast mode is 54 or 27 MHz (technically out of spec, needs testing!)
static SPIConfig hs_spicfg = {
		.circular = false,
		.end_cb = NULL,
		.ssport = NULL,
		.sspad = 0,
		.cr1 = SPI_BaudRatePrescaler_2,
		.cr2 = 0};
static SPIConfig ls_spicfg = {
		.circular = false,
		.end_cb = NULL,
		.ssport = NULL,
		.sspad = 0,
		.cr1 = SPI_BaudRatePrescaler_8,
		.cr2 = 0};

/* MMC/SD over SPI driver configuration.*/
static MMCConfig mmccfg = { NULL, &ls_spicfg, &hs_spicfg };

/**
 * fatfs MMC/SPI
 */
static FATFS MMC_FS;

static LoggingWithStorage logger("mmcCard");

static int fatFsErrors = 0;

static void mmcUnMount(void);

static void setSdCardReady(bool value) {
	fs_ready = value;
}

// print FAT error function
static void printError(const char *str, FRESULT f_error) {
	if (fatFsErrors++ > 16) {
		// no reason to spam the console
		return;
	}

	scheduleMsg(&logger, "FATfs Error \"%s\" %d", str, f_error);
}

static FIL FDLogFile NO_CACHE;
static FIL FDCurrFile NO_CACHE;

// 10 because we want at least 4 character name
#define MIN_FILE_INDEX 10
static int logFileIndex = MIN_FILE_INDEX;
static char logName[_MAX_FILLER + 20];

static void printMmcPinout(void) {
	scheduleMsg(&logger, "MMC CS %s", hwPortname(CONFIG(sdCardCsPin)));
	// todo: we need to figure out the right SPI pinout, not just SPI2
//	scheduleMsg(&logger, "MMC SCK %s:%d", portname(EFI_SPI2_SCK_PORT), EFI_SPI2_SCK_PIN);
//	scheduleMsg(&logger, "MMC MISO %s:%d", portname(EFI_SPI2_MISO_PORT), EFI_SPI2_MISO_PIN);
//	scheduleMsg(&logger, "MMC MOSI %s:%d", portname(EFI_SPI2_MOSI_PORT), EFI_SPI2_MOSI_PIN);
}

static void sdStatistics(void) {
	printMmcPinout();
	scheduleMsg(&logger, "SD enabled=%s status=%s", boolToString(CONFIG(isSdCardEnabled)),
			sdStatus);
	printSpiConfig(&logger, "SD", mmcSpiDevice);
	if (isSdCardAlive()) {
		scheduleMsg(&logger, "filename=%s size=%d", logName, totalLoggedBytes);
	}
}

static void incLogFileName(void) {
	LOCK_SD_SPI;
	memset(&FDCurrFile, 0, sizeof(FIL));						// clear the memory
	FRESULT err = f_open(&FDCurrFile, LOG_INDEX_FILENAME, FA_READ);				// This file has the index for next log file name

	char data[_MAX_FILLER];
	UINT result = 0;
	if (err != FR_OK && err != FR_EXIST) {
			logFileIndex = MIN_FILE_INDEX;
			scheduleMsg(&logger, "%s: not found or error: %d", LOG_INDEX_FILENAME, err);
	} else {
		f_read(&FDCurrFile, (void*)data, sizeof(data), &result);

		scheduleMsg(&logger, "Got content [%s] size %d", data, result);
		f_close(&FDCurrFile);
		if (result < 5) {
            data[result] = 0;
			logFileIndex = maxI(MIN_FILE_INDEX, atoi(data));
			if (absI(logFileIndex) == ERROR_CODE) {
				logFileIndex = MIN_FILE_INDEX;
			} else {
				logFileIndex++; // next file would use next file name
			}
		} else {
			logFileIndex = MIN_FILE_INDEX;
		}
	}

	err = f_open(&FDCurrFile, LOG_INDEX_FILENAME, FA_OPEN_ALWAYS | FA_WRITE);
	itoa10(data, logFileIndex);
	f_write(&FDCurrFile, (void*)data, strlen(data), &result);
	f_close(&FDCurrFile);
	scheduleMsg(&logger, "Done %d", logFileIndex);
	UNLOCK_SD_SPI;
}

static void prepareLogFileName(void) {
	strcpy(logName, RUSEFI_LOG_PREFIX);
	char *ptr;

#if HAL_USE_USB_MSD
	bool result = dateToStringShort(&logName[PREFIX_LEN]);
#else 
	// TS SD protocol supports only short 8 symbol file names :(
	bool result = false;
#endif

	if (result) {
		ptr = &logName[PREFIX_LEN + SHORT_TIME_LEN];
	} else {
		ptr = itoa10(&logName[PREFIX_LEN], logFileIndex);
	}

	strcat(ptr, DOT_MLG);
}

/**
 * @brief Create a new file with the specified name
 *
 * This function saves the name of the file in a global variable
 * so that we can later append to that file
 */
static void createLogFile(void) {
	LOCK_SD_SPI;
	memset(&FDLogFile, 0, sizeof(FIL));						// clear the memory
	prepareLogFileName();

	FRESULT err = f_open(&FDLogFile, logName, FA_OPEN_ALWAYS | FA_WRITE);				// Create new file
	if (err != FR_OK && err != FR_EXIST) {
		UNLOCK_SD_SPI;
		sdStatus = SD_STATE_OPEN_FAILED;
		warning(CUSTOM_ERR_SD_MOUNT_FAILED, "SD: mount failed");
		printError("FS mount failed", err);	// else - show error
		return;
	}

	err = f_lseek(&FDLogFile, f_size(&FDLogFile)); // Move to end of the file to append data
	if (err) {
		UNLOCK_SD_SPI;
		sdStatus = SD_STATE_SEEK_FAILED;
		warning(CUSTOM_ERR_SD_SEEK_FAILED, "SD: seek failed");
		printError("Seek error", err);
		return;
	}
	f_sync(&FDLogFile);
	setSdCardReady(true);						// everything Ok
	UNLOCK_SD_SPI;
}

static void removeFile(const char *pathx) {
	if (!isSdCardAlive()) {
		scheduleMsg(&logger, "Error: No File system is mounted");
		return;
	}
	LOCK_SD_SPI;
	f_unlink(pathx);

	UNLOCK_SD_SPI;
}

int
    mystrncasecmp(const char *s1, const char *s2, size_t n)
    {

           if (n != 0) {
                    const char *us1 = (const char *)s1;
                    const char *us2 = (const char *)s2;

                   do {
                            if (mytolower(*us1) != mytolower(*us2))
                                    return (mytolower(*us1) - mytolower(*us2));
                           if (*us1++ == '\0')
                                   break;
                            us2++;
                    } while (--n != 0);
            }
            return (0);
    }

static void listDirectory(const char *path) {

	if (!isSdCardAlive()) {
		scheduleMsg(&logger, "Error: No File system is mounted");
		return;
	}
	LOCK_SD_SPI;

	DIR dir;
	FRESULT res = f_opendir(&dir, path);

	if (res != FR_OK) {
		scheduleMsg(&logger, "Error opening directory %s", path);
		UNLOCK_SD_SPI;
		return;
	}

	scheduleMsg(&logger, LS_RESPONSE);

	for (int count = 0;count < FILE_LIST_MAX_COUNT;) {
		FILINFO fno;

		res = f_readdir(&dir, &fno);
		if (res != FR_OK || fno.fname[0] == 0) {
			break;
		}
		if (fno.fname[0] == '.') {
			continue;
		}
		if ((fno.fattrib & AM_DIR) || mystrncasecmp(RUSEFI_LOG_PREFIX, fno.fname, sizeof(RUSEFI_LOG_PREFIX) - 1)) {
			continue;
		}
		scheduleMsg(&logger, "logfile%lu:%s", fno.fsize, fno.fname);
		count++;

//			scheduleMsg(&logger, "%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %-12s", (fno.fattrib & AM_DIR) ? 'D' : '-',
//					(fno.fattrib & AM_RDO) ? 'R' : '-', (fno.fattrib & AM_HID) ? 'H' : '-',
//					(fno.fattrib & AM_SYS) ? 'S' : '-', (fno.fattrib & AM_ARC) ? 'A' : '-', (fno.fdate >> 9) + 1980,
//					(fno.fdate >> 5) & 15, fno.fdate & 31, (fno.ftime >> 11), (fno.ftime >> 5) & 63, fno.fsize,
//					fno.fname);
	}
	UNLOCK_SD_SPI;
}

/*
 * MMC card un-mount.
 */
static void mmcUnMount(void) {
	if (!isSdCardAlive()) {
		scheduleMsg(&logger, "Error: No File system is mounted. \"mountsd\" first");
		return;
	}
	f_close(&FDLogFile);						// close file
	f_sync(&FDLogFile);							// sync ALL
	mmcDisconnect(&MMCD1);						// Brings the driver in a state safe for card removal.
	mmcStop(&MMCD1);							// Disables the MMC peripheral.
	f_mount(NULL, 0, 0);						// FATFS: Unregister work area prior to discard it
	memset(&FDLogFile, 0, sizeof(FIL));			// clear FDLogFile
	setSdCardReady(false);						// status = false
	scheduleMsg(&logger, "MMC/SD card removed");
}

#if HAL_USE_USB_MSD
static NO_CACHE uint8_t blkbuf[MMCSD_BLOCK_SIZE];

static const scsi_inquiry_response_t scsi_inquiry_response = {
    0x00,           /* direct access block device     */
    0x80,           /* removable                      */
    0x04,           /* SPC-2                          */
    0x02,           /* response data format           */
    0x20,           /* response has 0x20 + 4 bytes    */
    0x00,
    0x00,
    0x00,
    "rusEFI",
    "SD Card",
    {'v',CH_KERNEL_MAJOR+'0','.',CH_KERNEL_MINOR+'0'}
};

static binary_semaphore_t usbConnectedSemaphore;

void onUsbConnectedNotifyMmcI() {
	chBSemSignalI(&usbConnectedSemaphore);
}

#endif /* HAL_USE_USB_MSD */

/*
 * Attempts to initialize the MMC card.
 * Returns a BaseBlockDevice* corresponding to the SD card if successful, otherwise nullptr.
 */
static BaseBlockDevice* initializeMmcBlockDevice() {
	if (!CONFIG(isSdCardEnabled)) {
		return nullptr;
	}

	// Configures and activates the MMC peripheral.
	mmcSpiDevice = CONFIG(sdCardSpiDevice);

	efiAssert(OBD_PCM_Processor_Fault, mmcSpiDevice != SPI_NONE, "SD card enabled, but no SPI device configured!", nullptr);

	// todo: reuse initSpiCs method?
	hs_spicfg.ssport = ls_spicfg.ssport = getHwPort("mmc", CONFIG(sdCardCsPin));
	hs_spicfg.sspad = ls_spicfg.sspad = getHwPin("mmc", CONFIG(sdCardCsPin));
	mmccfg.spip = getSpiDevice(mmcSpiDevice);

	// We think we have everything for the card, let's try to mount it!
	mmcObjectInit(&MMCD1);
	mmcStart(&MMCD1, &mmccfg);

	// Performs the initialization procedure on the inserted card.
	LOCK_SD_SPI;
	sdStatus = SD_STATE_CONNECTING;
	if (mmcConnect(&MMCD1) != HAL_SUCCESS) {
		sdStatus = SD_STATE_NOT_CONNECTED;
		warning(CUSTOM_OBD_MMC_ERROR, "Can't connect or mount MMC/SD");
		UNLOCK_SD_SPI;
		return nullptr;
	}

	UNLOCK_SD_SPI;
	return (BaseBlockDevice*)&MMCD1;
}

// Initialize and mount the SD card.
// Returns true if the filesystem was successfully mounted for writing.
static bool mountMmc() {
	auto cardBlockDevice = initializeMmcBlockDevice();

#if HAL_USE_USB_MSD
	// Wait for the USB stack to wake up, or a 5 second timeout, whichever occurs first
	msg_t usbResult = chBSemWaitTimeout(&usbConnectedSemaphore, TIME_MS2I(5000));

	bool hasUsb = usbResult == MSG_OK;

	msdObjectInit(&USBMSD1);

	// If we have a device AND USB is connected, mount the card to USB, otherwise
	// mount the null device and try to mount the filesystem ourselves
	if (cardBlockDevice && hasUsb) {
		// Mount the real card to USB
		msdStart(&USBMSD1, usb_driver, cardBlockDevice, blkbuf, &scsi_inquiry_response, NULL);

		// At this point we're done: don't try to write files ourselves
		return false;
	} else {
		// Mount a  "no media" device to USB
		msdMountNullDevice(&USBMSD1, usb_driver, blkbuf, &scsi_inquiry_response);
	}
#endif

	// if no card, don't try to mount FS
	if (!cardBlockDevice) {
		return false;
	}

	// We were able to connect the SD card, mount the filesystem
	memset(&MMC_FS, 0, sizeof(FATFS));
	if (f_mount(&MMC_FS, "/", 1) == FR_OK) {
		sdStatus = SD_STATE_MOUNTED;
		incLogFileName();
		createLogFile();
		fileCreatedCounter++;
		scheduleMsg(&logger, "MMC/SD mounted!");
		return true;
	} else {
		sdStatus = SD_STATE_MOUNT_FAILED;
		return false;
	}
}

struct SdLogBufferWriter final : public BufferedWriter<512> {
	bool failed = false;

	size_t writeInternal(const char* buffer, size_t count) override {
		size_t bytesWritten;

		totalLoggedBytes += count;

		LOCK_SD_SPI;
		FRESULT err = f_write(&FDLogFile, buffer, count, &bytesWritten);

		if (bytesWritten != count) {
			printError("write error or disk full", err); // error or disk full

			// Close file and unmount volume
			mmcUnMount();
			UNLOCK_SD_SPI;
			failed = true;
			return 0;
		} else {
			writeCounter++;
			totalWritesCounter++;
			if (writeCounter >= F_SYNC_FREQUENCY) {
				/**
				 * Performance optimization: not f_sync after each line, f_sync is probably a heavy operation
				 * todo: one day someone should actually measure the relative cost of f_sync
				 */
				f_sync(&FDLogFile);
				totalSyncCounter++;
				writeCounter = 0;
			}
		}

		UNLOCK_SD_SPI;
		return bytesWritten;
	}
};

static NO_CACHE SdLogBufferWriter logBuffer;

static THD_FUNCTION(MMCmonThread, arg) {
	(void)arg;
	chRegSetThreadName("MMC Card Logger");

	if (!mountMmc()) {
		// no card present (or mounted via USB), don't do internal logging
		return;
	}

	while (true) {
		// if the SPI device got un-picked somehow, cancel SD card
		if (CONFIG(sdCardSpiDevice) == SPI_NONE) {
			return;
		}

		if (CONFIG(debugMode) == DBG_SD_CARD) {
			tsOutputChannels.debugIntField1 = totalLoggedBytes;
			tsOutputChannels.debugIntField2 = totalWritesCounter;
			tsOutputChannels.debugIntField3 = totalSyncCounter;
			tsOutputChannels.debugIntField4 = fileCreatedCounter;
		}

		writeLogLine(logBuffer);

		// Something went wrong (already handled), so cancel further writes
		if (logBuffer.failed) {
			return;
		}

		auto period = CONFIG(sdCardPeriodMs);
		if (period > 0) {
			chThdSleepMilliseconds(period);
		}
	}
}

bool isSdCardAlive(void) {
	return fs_ready;
}

void initMmcCard(void) {
	logName[0] = 0;

#if HAL_USE_USB_MSD
	chBSemObjectInit(&usbConnectedSemaphore, true);
#endif

	chThdCreateStatic(mmcThreadStack, sizeof(mmcThreadStack), LOWPRIO, (tfunc_t)(void*) MMCmonThread, NULL);

	addConsoleAction("sdinfo", sdStatistics);
	addConsoleActionS("ls", listDirectory);
	addConsoleActionS("del", removeFile);
	addConsoleAction("incfilename", incLogFileName);
}

#endif /* EFI_FILE_LOGGING */
