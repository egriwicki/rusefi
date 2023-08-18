# List of all the board related files.
BOARDCPPSRC =  $(BOARD_DIR)/board_configuration.cpp

# Override DEFAULT_ENGINE_TYPE
DDEFS += -DSTM32F407xx
DDEFS += -DSHORT_BOARD_NAME=BB_V3 -DSTATIC_BOARD_ID=STATIC_BOARD_ID_BB_V3 -DDEFAULT_ENGINE_TYPE=engine_type_e::MINIMAL_PINS
DDEFS += -DFIRMWARE_ID=\"BB_V3\"
DDEFS += -DLED_CRITICAL_ERROR_BRAIN_PIN=Gpio::Unassigned
DDEFS += -DHAL_TRIGGER_USE_PAL=TRUE
DDEFS += -DHAL_VSS_USE_PAL=TRUE

DDEFS += -DSTM32_ADC_USE_ADC3=TRUE
DDEFS += -DEFI_SOFTWARE_KNOCK=TRUE
