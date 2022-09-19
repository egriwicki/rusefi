
# 144 package MCU
ifeq ($(LED_CRITICAL_ERROR_BRAIN_PIN),)
  LED_CRITICAL_ERROR_BRAIN_PIN = -DLED_CRITICAL_ERROR_BRAIN_PIN=H144_LED1_RED
endif

DDEFS += $(LED_CRITICAL_ERROR_BRAIN_PIN)

PRIMARY_COMMUNICATION_PORT_USART2=-DEFI_CONSOLE_TX_BRAIN_PIN=Gpio::D6 -DEFI_CONSOLE_RX_BRAIN_PIN=Gpio::D5 -DTS_PRIMARY_PORT=UARTD2 -DSTM32_UART_USE_USART2=1

include $(BOARDS_DIR)/hellen/hellen-common.mk