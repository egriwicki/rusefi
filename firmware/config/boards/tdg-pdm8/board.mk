
BOARDCPPSRC = $(PROJECT_DIR)/config/boards/tdg-pdm8/board_configuration.cpp

PROJECT_CPU = ARCH_STM32F4

DDEFS += -DLED_CRITICAL_ERROR_BRAIN_PIN=Gpio::C11

# We're running on TDG PDM hardware!
DDEFS += -DHW_TDG_PDM8=1

DDEFS += -DEFI_CAN_SERIAL=TRUE

DDEFS += -DFIRMWARE_ID=\"tdg-pdm8\" -DSHORT_BOARD_NAME=tdg-pdm8
DDEFS += -DDEFAULT_ENGINE_TYPE=MINIMAL_PINS

# We recycle GPIOI_0..7 for protected outputs since this is a 64-pin STM32 which has no port I
DDEFS += -DBRAIN_PIN_ONCHIP_LAST=GPIOH_15
