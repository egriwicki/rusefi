	/**
 * @file	console_io.cpp
 *
 * @date Dec 29, 2012
 * @author Andrey Belomutskiy, (c) 2012-2020
 *
 * This file is part of rusEfi - see http://rusefi.com
 *
 * rusEFI can communicate with external universe via native USB or some sort of TTL mode
 * We have an interesting situation with TTL communication channels, we have
 * 1) SERIAL - this one was implemented first simply because the code was readily available (works on stm32)
 *    this one is most suitable for streaming HAL API
 *    this one is not great since each byte requires an IRQ and with enough IRQ delay we have a risk of data loss
 * 2) UART DMA - the best one since FIFO buffer reduces data loss (works on stm32)
 *    We have two halves of DMA buffer - one is used for TTL while rusEFI prepares next batch of data in the other side.
 *    We need idle support in order to not wait for the complete buffer to get full in order to recieve a message.
 *    Back when we were implementing this STM32_DMA_CR_HTIE was not available in ChibiOS driver so we have added it.
 *    we have custom rusEFI changes to ChibiOS HAL driver v1
 *    F7 uses driver v2 which currently does not have rusEFI changes.
 *    open question if fresh ChibiOS is better in this regard.
 * 3) UART this one is useful on platforms with hardware FIFO buffer like Kinetis.
 *    stm32 does not have such buffer so for stm32 UART without DMA has no advantages
 *
 *
 * rusEfi is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * rusEfi is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "engine.h"
#include "console_io.h"
#include "os_util.h"
#include "tunerstudio.h"
#include "connector_uart_dma.h"
#include "thread_priority.h"

#if EFI_SIMULATOR
#include "rusEfiFunctionalTest.h"
#endif /*EFI_SIMULATOR */

EXTERN_ENGINE;

// 10 seconds
#define CONSOLE_WRITE_TIMEOUT 10000

static bool isSerialConsoleStarted = false;

#if (defined(EFI_CONSOLE_SERIAL_DEVICE) && ! EFI_SIMULATOR)
static event_listener_t consoleEventListener;
#endif

bool consoleByteArrived = false;

void onDataArrived(void) {
	consoleByteArrived = true;
}

CommandHandler console_line_callback;

#if (defined(EFI_CONSOLE_SERIAL_DEVICE) && ! EFI_SIMULATOR )
SerialConfig serialConfig = {
	.speed	= 0,
	.cr1	= 0,
	.cr2	= USART_CR2_STOP1_BITS | USART_CR2_LINEN,
	.cr3	= 0
};
#endif

#if (defined(EFI_CONSOLE_UART_DEVICE) && ! EFI_SIMULATOR )
/* Note: This structure is modified from the default ChibiOS layout! */
UARTConfig uartConfig = {
	.txend1_cb		= NULL,
	.txend2_cb		= NULL,
	.rxend_cb		= NULL,
	.rxchar_cb		= NULL,
	.rxerr_cb		= NULL,
	.timeout_cb		= NULL,
	.speed			= 0,
	.cr1			= 0,
	.cr2			= 0/*USART_CR2_STOP1_BITS*/ | USART_CR2_LINEN,
	.cr3			= 0,
	.rxhalf_cb		= NULL
};
#endif /* EFI_CONSOLE_UART_DEVICE */

ts_channel_s primaryChannel;

#if EFI_PROD_CODE || EFI_EGT

bool isCommandLineConsoleReady(void) {
	return isSerialConsoleStarted;
}
#endif /* EFI_PROD_CODE || EFI_EGT */

#if !defined(EFI_CONSOLE_NO_THREAD)

static THD_WORKING_AREA(consoleThreadStack, CONNECTIVITY_THREAD_STACK);
static THD_FUNCTION(consoleThreadEntryPoint, arg) {
	(void) arg;
	chRegSetThreadName("console thread");

#if !PRIMARY_UART_DMA_MODE
	primaryChannel.channel = (BaseChannel *) getConsoleChannel();
#endif

#if EFI_TUNER_STUDIO
	runBinaryProtocolLoop(&primaryChannel);
#endif /* EFI_TUNER_STUDIO */
}

#endif /* EFI_CONSOLE_NO_THREAD */

static Logging *logger;

void startConsole(Logging *sharedLogger, CommandHandler console_line_callback_p) {
	logger = sharedLogger;
	console_line_callback = console_line_callback_p;

#if (defined(EFI_CONSOLE_SERIAL_DEVICE) || defined(EFI_CONSOLE_UART_DEVICE)) && ! EFI_SIMULATOR
	efiSetPadMode("console RX", EFI_CONSOLE_RX_BRAIN_PIN, PAL_MODE_ALTERNATE(EFI_CONSOLE_AF));
	efiSetPadMode("console TX", EFI_CONSOLE_TX_BRAIN_PIN, PAL_MODE_ALTERNATE(EFI_CONSOLE_AF));
#endif

#if PRIMARY_UART_DMA_MODE && ! EFI_SIMULATOR
		primaryChannel.uartp = EFI_CONSOLE_UART_DEVICE;
		startUartDmaConnector(primaryChannel.uartp PASS_CONFIG_PARAMETER_SUFFIX);
		isSerialConsoleStarted = true;
#elif (defined(EFI_CONSOLE_SERIAL_DEVICE) && ! EFI_SIMULATOR)
		/*
		 * Activates the serial
		 * it is important to set 'NONE' as flow control! in terminal application on the PC
		 */
		serialConfig.speed = engineConfiguration->uartConsoleSerialSpeed;
		sdStart(EFI_CONSOLE_SERIAL_DEVICE, &serialConfig);

		chEvtRegisterMask((event_source_t *) chnGetEventSource(EFI_CONSOLE_SERIAL_DEVICE), &consoleEventListener, 1);
		isSerialConsoleStarted = true;
#elif (defined(EFI_CONSOLE_UART_DEVICE) && ! EFI_SIMULATOR)
		uartConfig.speed = engineConfiguration->uartConsoleSerialSpeed;
		uartStart(EFI_CONSOLE_UART_DEVICE, &uartConfig);
		isSerialConsoleStarted = true;
#endif /* EFI_CONSOLE_SERIAL_DEVICE || EFI_CONSOLE_UART_DEVICE */

#if !defined(EFI_CONSOLE_NO_THREAD)
	chThdCreateStatic(consoleThreadStack, sizeof(consoleThreadStack), PRIO_CONSOLE, (tfunc_t)consoleThreadEntryPoint, NULL);
#endif /* EFI_CONSOLE_NO_THREAD */
}
