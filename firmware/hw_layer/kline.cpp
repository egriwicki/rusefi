#include "pch.h"
#include "kline.h"
#include "hellen_meta.h"
#include "crc8hondak.h"

size_t readWhileGives(ByteSource source, uint8_t *buffer, int bufferSize) {
        size_t totalBytes = 0;
        while (totalBytes < bufferSize) {
           int readThisTime = source(&buffer[totalBytes], bufferSize - totalBytes);
           if (readThisTime == 0) {
                // looks like idle gap
                break;
           }
           totalBytes += readThisTime;
        }
    return totalBytes;
}

#ifdef EFI_KLINE
static SerialDriver* const klDriver = &KLINE_SERIAL_DEVICE;
static THD_WORKING_AREA(klThreadStack, UTILITY_THREAD_STACK_SIZE);

static int totalBytes = 0;
static bool kLineOutPending = false;
static int kLineOut;

void kLineThread(void*) {
    while (1) {
        /**
         * under the hood there is SERIAL_BUFFERS_SIZE which we hope to help us
         */

        uint8_t bufferIn[16];
        // a bit of a busy read open question if this would affect performance?
        // on 2003 Honda for instance the bus seems to be 70%-ish busy. 9600 baud is 1.04ms per byte, a bit below 1kHz
        ByteSource serialSource = [] (uint8_t * buffer, int maxSize) {
            return chnReadTimeout(klDriver,buffer, maxSize, KLINE_READ_TIMEOUT);
        };
        int len = readWhileGives(serialSource, bufferIn, sizeof(serialSource));

        // to begin with just write byte to console
        if (len > 0) {
            efiPrintf("kline: got count 0x%02x", len);
            for (size_t i =0;i<len;i++) {
                efiPrintf("kline: got 0x%02x", bufferIn[i]);
                totalBytes++;
            }
            if (len > 1) {
                int crc = crc_hondak_calc(bufferIn, len - 1);
                if (crc == bufferIn[len - 1]) {
                    efiPrintf("happy CRC 0x%02x", crc);
                }
            }
        }
        if (kLineOutPending) {
            kLineOutPending = false;
            efiPrintf("kline OUT: 0x%02x", kLineOut);
            chnWrite(klDriver, (const uint8_t *)kLineOut, 1);
        }
    }
}
#endif // EFI_KLINE

void startKLine() {
#ifdef EFI_KLINE
    if (!engineConfiguration->enableKline) {
        return;
    }
#if EFI_PROD_CODE
	efiSetPadMode("K-Line UART RX", KLINE_SERIAL_DEVICE_RX, PAL_MODE_ALTERNATE(TS_SERIAL_AF));
	efiSetPadMode("K-Line UART TX", KLINE_SERIAL_DEVICE_TX, PAL_MODE_ALTERNATE(TS_SERIAL_AF));
#endif /* EFI_PROD_CODE */

	static SerialConfig cfg = {
		#if EFI_PROD_CODE
			.speed = 0,
			.cr1 = 0,
			.cr2 = USART_CR2_STOP1_BITS | USART_CR2_LINEN,
			.cr3 = 0
		#endif // EFI_PROD_CODE
	};

    if (engineConfiguration->kLineBaudRate < 100)
        engineConfiguration->kLineBaudRate = KLINE_BAUD_RATE;
    cfg.speed = engineConfiguration->kLineBaudRate;

	sdStart(klDriver, &cfg);
#endif // EFI_KLINE
}

void stopKLine() {
#ifdef EFI_KLINE
#if EFI_PROD_CODE
    if (activeConfiguration.enableKline) {
	    efiSetPadUnused(KLINE_SERIAL_DEVICE_RX);
	    efiSetPadUnused(KLINE_SERIAL_DEVICE_TX);

	    sdStop(klDriver);
	}

#endif /* EFI_PROD_CODE */
#endif // EFI_KLINE
}

void initKLine() {
    if (!engineConfiguration->enableKline) {
        return;
    }
#ifdef EFI_KLINE
	startKLine();

    chThdCreateStatic(klThreadStack, sizeof(klThreadStack), NORMALPRIO + 1, kLineThread, nullptr);
    addConsoleAction("kline", [](){
        efiPrintf("kline totalBytes %d", totalBytes);
    });
    addConsoleActionI("klinesend", [](int value){
        kLineOutPending = true;
        kLineOut = value;
    });

#endif // EFI_KLINE
}
