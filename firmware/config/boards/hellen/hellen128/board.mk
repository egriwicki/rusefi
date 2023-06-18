# Combine the related files for a specific platform and MCU.

# Target ECU board design
BOARDCPPSRC = $(BOARD_DIR)/board_configuration.cpp

# Set this if you want a default engine type other than normal hellen128
ifeq ($(VAR_DEF_ENGINE_TYPE),)
  VAR_DEF_ENGINE_TYPE = -DDEFAULT_ENGINE_TYPE=engine_type_e::HELLEN_128_MERCEDES_4_CYL
endif


DDEFS += -DEFI_MAIN_RELAY_CONTROL=TRUE

# Add them all together
DDEFS += -DFIRMWARE_ID=\"hellen128\" $(VAR_DEF_ENGINE_TYPE)
DDEFS += -DEFI_SOFTWARE_KNOCK=TRUE -DSTM32_ADC_USE_ADC3=TRUE

DDEFS += -DSHORT_BOARD_NAME=hellen128

include $(BOARDS_DIR)/hellen/hellen-common176.mk

