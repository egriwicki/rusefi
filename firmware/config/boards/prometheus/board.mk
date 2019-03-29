# List of all the board related files.
BOARDSRC = $(PROJECT_DIR)/config/boards/Prometheus/board.c
BOARDSRC_CPP = $(PROJECT_DIR)/config/boards/Prometheus/board_configuration.cpp

# Required include directories
BOARDINC = $(PROJECT_DIR)/config/boards/Prometheus

# Override LD script
ifeq ($(USE_BOOTLOADER),yes)
  # include Prometheus bootloader code
  BOOTLOADERINC= $(PROJECT_DIR)/bootloader/Prometheus/$(PROMETHEUS_BOARD)
endif

ifeq ($(PROMETHEUS_BOARD),405)
LDSCRIPT= $(PROJECT_DIR)/config/boards/Prometheus/STM32F405xG.ld
DDEFS += -DDEFAULT_ENGINE_TYPE=PROMETHEUS_DEFAULTS -DSTM32F405xx
else
# Override DEFAULT_ENGINE_TYPE
LDSCRIPT= $(PROJECT_DIR)/config/boards/Prometheus/STM32F469xI.ld
DDEFS += -DDEFAULT_ENGINE_TYPE=PROMETHEUS_DEFAULTS -DSTM32F469xx
endif
