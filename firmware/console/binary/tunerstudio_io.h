/**
 * @file	tunerstudio_io.h
 * @file TS protocol commands and methods are here
 *
 * @date Mar 8, 2015
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once
#include "global.h"

#if EFI_PROD_CODE
#include "usbconsole.h"
#include "pin_repository.h"
#endif

#define TS_RESPONSE_UNDERRUN 0x80
#define TS_RESPONSE_CRC_FAILURE 0x82

typedef enum {
	TS_PLAIN = 0,
	TS_CRC = 1
} ts_response_format_e;

struct ts_channel_s {
#if ! EFI_UNIT_TEST
	BaseChannel * channel = nullptr;
#endif
	uint8_t writeBuffer[7];	// size(2 bytes) + response(1 byte) + crc32 (4 bytes)
	/**
	 * See 'blockingFactor' in rusefi.ini
	 */
	char crcReadBuffer[BLOCKING_FACTOR + 30];

#if TS_UART_DMA_MODE || PRIMARY_UART_DMA_MODE || TS_UART_MODE
	UARTDriver *uartp = nullptr;
#endif // TS_UART_DMA_MODE
};

#define CRC_VALUE_SIZE 4
// todo: double-check this
#define CRC_WRAPPING_SIZE (CRC_VALUE_SIZE + 3)

#if HAL_USE_SERIAL_USB
#define CONSOLE_USB_DEVICE SDU1
#endif /* HAL_USE_SERIAL_USB */

void startTsPort(ts_channel_s *tsChannel);
bool stopTsPort(ts_channel_s *tsChannel);

// that's 1 second
#define BINARY_IO_TIMEOUT TIME_MS2I(1000)

// that's 1 second
#define SR5_READ_TIMEOUT TIME_MS2I(1000)

void sr5WriteData(ts_channel_s *tsChannel, const uint8_t * buffer, int size);
void sr5WriteCrcPacket(ts_channel_s *tsChannel, const uint8_t responseCode, const void *buf, const uint16_t size);
void sr5SendResponse(ts_channel_s *tsChannel, ts_response_format_e mode, const uint8_t * buffer, int size);
void sendOkResponse(ts_channel_s *tsChannel, ts_response_format_e mode);
int sr5ReadData(ts_channel_s *tsChannel, uint8_t * buffer, int size);
int sr5ReadDataTimeout(ts_channel_s *tsChannel, uint8_t * buffer, int size, int timeout);
bool sr5IsReady(ts_channel_s *tsChannel);

