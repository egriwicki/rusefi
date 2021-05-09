/**
 * @file	mpu_util.cpp
 *
 * @date Jul 27, 2014
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#include "hal.h"
#include "flash_int.h"

static bool isDualBank() {
	// cleared bit indicates dual bank
	return (FLASH->OPTCR & FLASH_OPTCR_nDBANK) == 0;
}

static uint16_t flashSize() {
	return *reinterpret_cast<const volatile uint16_t*>(FLASHSIZE_BASE);
}

enum class DeviceType {
	DualBank1MB,
	DualBank2MB,
	SingleBank1MB,
	SingleBank2MB,
	Unknown
};

static DeviceType determineDevice() {
	bool db = isDualBank();
	uint16_t fs = flashSize();

	if (db) {
		if (fs == 1024) {
			return DeviceType::DualBank1MB;
		} else if (fs == 2048) {
			return DeviceType::DualBank2MB;
		}
	} else {
		if (fs == 1024) {
			// Unsupported scenario! Not enough space for program plus two config copies
			firmwareError(OBD_PCM_Processor_Fault, "1MB single bank MCU detected: please clear nDBANK option bit and reinstall FW.");
			return DeviceType::SingleBank1MB;
		} else if (fs == 2048) {
			return DeviceType::SingleBank2MB;
		}
	}

	firmwareError(OBD_PCM_Processor_Fault, "Unrecognized flash memory layout db=%d, size=%d", db, fs);
	return DeviceType::Unknown;
}

size_t flashSectorSize(flashsector_t sector) {
	// 1MB devices have 8 sectors per bank
	// 2MB devices have 12 sectors per bank
	// However, the second bank always starts at index 12 (1MB devices have a 4 sector discontinuity between banks)

	if (sector >= 12) {
		// The second bank has the same structure as the first
		return flashSectorSize(sector - 12);
	}

	// Pages are twice the size when in single bank mode
	size_t dbMul = isDualBank() ? 1 : 2;

	if (sector <= 3)
		return 16 * 1024 * dbMul;
	else if (sector == 4)
		return 64 * 1024 * dbMul;
	else if (sector >= 5)
		return 128 * 1024 * dbMul;
	return 0;
}

uintptr_t getFlashAddrFirstCopy() {
	switch (determineDevice()) {
		case DeviceType::DualBank1MB:
			// Sector 18, second to last 128K sector
			return 0x080C0000;
		case DeviceType::SingleBank1MB:
			// Sector 7, last 256K sector
			return 0x080C0000;
		case DeviceType::DualBank2MB: /* falls thru */
		case DeviceType::SingleBank2MB:
			// Start of the second megabyte
			// Sector 12 for dual bank
			// Sector 8 for single bank
			return 0x08100000;
		default:
			return 0;
	}
}

uintptr_t getFlashAddrSecondCopy() {
	switch (determineDevice()) {
		case DeviceType::DualBank1MB:
			// Sector 19, last 128K sector, 128K after the first copy
			return 0x080E0000;
		case DeviceType::DualBank2MB:
			// Sector 14, 32K after the first copy
			return 0x08108000;
		case DeviceType::SingleBank2MB:
			// Sector 9, 256K after the first copy
			return 0x08140000;
		case DeviceType::SingleBank1MB:
			// We can't fit a second copy in this config, fall thru to failure case
		default:
			return 0;
	}
}
