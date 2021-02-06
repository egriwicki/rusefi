BOARD_DIR = $(PROJECT_DIR)/config/boards/$(PROJECT_BOARD)

# List of all the board related files.
BOARDSRC = $(BOARD_DIR)/board.c
BOARDCPPSRC = $(BOARD_DIR)/board_configuration.cpp

# Required include directories
BOARDINC = $(BOARD_DIR)
BOARDINC += $(BOARD_DIR)/config/controllers/algo

# Override LD script
ifeq ($(USE_BOOTLOADER),yes)
  # include Prometheus bootloader code
  BOOTLOADERINC = $(PROJECT_DIR)/bootloader/subaru_eg33
endif

LDSCRIPT = $(BOARD_DIR)/STM32F76xxI.ld

#FIX THIS
CONFIGPATH = $(PROJECT_DIR)/config/stm32f7ems

CONFDIR = config/stm32f7ems

#LED
LED_CRITICAL_ERROR_BRAIN_PIN = -DLED_CRITICAL_ERROR_BRAIN_PIN=GPIOG_7

# We are running on Subaru EG33 hardware!
DDEFS += -DHW_SUBARU_EG33=1

# Override DEFAULT_ENGINE_TYPE
# Fix
DDEFS += -DDEFAULT_ENGINE_TYPE=SUBARUEG33_DEFAULTS -DSTM32F765xx $(LED_CRITICAL_ERROR_BRAIN_PIN)

# Shared variables
ALLCSRC   += $(BOARDSRC)
ALLCPPSRC += $(BOARDCPPSRC)
ALLINC    += $(BOARDINC)
