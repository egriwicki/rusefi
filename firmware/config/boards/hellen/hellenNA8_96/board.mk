# Combine the related files for a specific platform and MCU.

# Target ECU board design
BOARDCPPSRC = $(BOARDS_DIR)/hellen/hellen-nb1/board_configuration.cpp
BOARDINC = $(BOARDS_DIR)/hellen/hellen-nb1

# Set this if you want a default engine type other than normal hellen-nb1
ifeq ($(DEFAULT_ENGINE_TYPE),)
  DEFAULT_ENGINE_TYPE = -DDEFAULT_ENGINE_TYPE=HELLEN_NA8_96
endif


DDEFS += -DEFI_MAIN_RELAY_CONTROL=TRUE



# Add them all together
DDEFS += -DFIRMWARE_ID=\"hellenNB1\" $(DEFAULT_ENGINE_TYPE)
DDEFS += -DEFI_SOFTWARE_KNOCK=TRUE -DSTM32_ADC_USE_ADC3=TRUE
DDEFS += -DHAL_TRIGGER_USE_PAL=TRUE
include $(BOARDS_DIR)/hellen/hellen-common144.mk

# Enable serial pins on expansion header
DDEFS += $(PRIMARY_COMMUNICATION_PORT_USART2)

