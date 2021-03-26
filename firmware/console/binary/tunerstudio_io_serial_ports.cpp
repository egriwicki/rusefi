/**
 * @file	This file initializes the hardware serial ports that run the TS protocol.
 *
 * @date Mar 26, 2021
 */

#include "engine.h"

#if EFI_PROD_CODE
#include "tunerstudio.h"
#include "tunerstudio_io.h"
#include "connector_uart_dma.h"

EXTERN_ENGINE;

#if defined(TS_PRIMARY_UART) || defined(TS_PRIMARY_SERIAL)
	#ifdef TS_PRIMARY_UART
		#if EFI_USE_UART_DMA
			UartDmaTsChannel primaryChannel(TS_PRIMARY_UART);
		#else
			UartTsChannel primaryChannel(TS_PRIMARY_UART);
		#endif
	#elif defined(TS_PRIMARY_SERIAL)
	SerialTsChannel primaryChannel(TS_PRIMARY_SERIAL);
	#endif

	struct PrimaryChannelThread : public TunerstudioThread {
		PrimaryChannelThread() : TunerstudioThread("Primary TS Channel") { }

		TsChannelBase* setupChannel() {
			efiSetPadMode("Primary Channel RX", EFI_CONSOLE_RX_BRAIN_PIN, PAL_MODE_ALTERNATE(EFI_CONSOLE_AF));
			efiSetPadMode("Primary Channel TX", EFI_CONSOLE_TX_BRAIN_PIN, PAL_MODE_ALTERNATE(EFI_CONSOLE_AF));

			primaryChannel.start(CONFIG(uartConsoleSerialSpeed));

			return &primaryChannel;
		}
	};

	static PrimaryChannelThread primaryChannelThread;
#endif // defined(TS_PRIMARY_UART) || defined(TS_PRIMARY_SERIAL)

#if defined(TS_SECONDARY_UART) || defined(TS_SECONDARY_SERIAL)
	#ifdef TS_SECONDARY_UART
		#if EFI_USE_UART_DMA
			UartDmaTsChannel secondaryChannel(TS_SECONDARY_UART);
		#else
			UartTsChannel secondaryChannel(TS_SECONDARY_UART);
		#endif
	#elif defined(TS_SECONDARY_SERIAL)
	SerialTsChannel secondaryChannel(TS_SECONDARY_SERIAL);
	#endif

	struct SecondaryChannelThread : public TunerstudioThread {
		SecondaryChannelThread() : TunerstudioThread("Secondary TS Channel") { }

		TsChannelBase* setupChannel() {
			efiSetPadMode("Secondary Channel RX", engineConfiguration->binarySerialRxPin, PAL_MODE_ALTERNATE(TS_SERIAL_AF));
			efiSetPadMode("Secondary Channel TX", engineConfiguration->binarySerialTxPin, PAL_MODE_ALTERNATE(TS_SERIAL_AF));

			secondaryChannel.start(CONFIG(uartConsoleSerialSpeed));

			return &secondaryChannel;
		}
	};

	static SecondaryChannelThread secondaryChannelThread;
#endif // defined(TS_SECONDARY_UART) || defined(TS_SECONDARY_SERIAL)

void startSerialChannels() {
#if defined(TS_PRIMARY_UART) || defined(TS_PRIMARY_SERIAL)
	primaryChannelThread.Start();
#endif

#if defined(TS_SECONDARY_UART) || defined(TS_SECONDARY_SERIAL)
	secondaryChannelThread.Start();
#endif
}

SerialTsChannelBase* getBluetoothChannel() {
#if defined(TS_SECONDARY_UART) || defined(TS_SECONDARY_SERIAL)
	return &secondaryChannel;
#elif defined(TS_PRIMARY_UART) || defined(TS_PRIMARY_SERIAL)
	// Use primary channel for BT if no secondary exists
	return &primaryChannel;
#endif

	// no HW serial channels on this board, fail
	return nullptr;
}

#endif // EFI_PROD_CODE
