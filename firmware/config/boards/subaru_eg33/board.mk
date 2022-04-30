BOARD_DIR = $(PROJECT_DIR)/config/boards/$(PROJECT_BOARD)

# List of all the board related files.
BOARDCPPSRC = $(BOARD_DIR)/board_configuration.cpp

# Required include directories
BOARDINC = $(BOARD_DIR)
BOARDINC += $(BOARD_DIR)/config/controllers/algo

# Override LD script
ifeq ($(USE_BOOTLOADER),yes)
  # include Prometheus bootloader code
  BOOTLOADERINC = $(PROJECT_DIR)/bootloader/subaru_eg33
endif

#LED
DDEFS +=  -DLED_CRITICAL_ERROR_BRAIN_PIN=Gpio::G7

# We are running on Subaru EG33 hardware!
DDEFS += -DHW_SUBARU_EG33=1
DDEFS += -DFIRMWARE_ID=\"EG33\"

DDEFS += -DSHORT_BOARD_NAME=subaru_eg33_f7

# Override DEFAULT_ENGINE_TYPE
DDEFS += -DDEFAULT_ENGINE_TYPE=SUBARUEG33_DEFAULTS

# HW options
DDEFS += -DBOARD_L9779_COUNT=0

#Some options override
#ICU vs PAL/EXTI
DDEFS += -DHAL_TRIGGER_USE_PAL=TRUE
DDEFS += -DEFI_ICU_INPUTS=FALSE
DDEFS += -DEFI_LOGIC_ANALYZER=FALSE

DDEFS += -DHAL_USE_UART=FALSE
DDEFS += -DUART_USE_WAIT=FALSE

#Mass Storage
DDEFS += -DEFI_EMBED_INI_MSD=TRUE

#Linker options, flash size
USE_OPT += -Wl,--defsym=FLASH_SIZE=1m

# Shared variables
ALLINC    += $(BOARDINC)

#Serial flash support
include $(PROJECT_DIR)/hw_layer/drivers/flash/sst26f_jedec.mk
