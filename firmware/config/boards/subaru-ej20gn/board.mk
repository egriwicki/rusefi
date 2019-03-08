# List of all the board related files.
BOARDSRC = $(PROJECT_DIR)/config/boards/subaruej20g/board.c
BOARDSRC_CPP = $(PROJECT_DIR)/config/boards/subaruej20g/board_configuration.cpp

# Required include directories
BOARDINC = $(PROJECT_DIR)/config/boards/subaruej20g

# Override LD script
ifeq ($(USE_BOOTLOADER),yes)
  # include Prometheus bootloader code
  BOOTLOADERINC= $(PROJECT_DIR)/bootloader/subaruej20g
endif

LDSCRIPT= $(PROJECT_DIR)/config/boards/subaruej20g/STM32F76xxI.ld

# Override DEFAULT_ENGINE_TYPE
DDEFS += -DDEFAULT_ENGINE_TYPE=SUBARUEJ20G_DEFAULTS -DSTM32F765xx
