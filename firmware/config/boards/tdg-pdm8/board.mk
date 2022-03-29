
BOARDCPPSRC = $(PROJECT_DIR)/config/boards/tdg-pdm8/board_configuration.cpp

PROJECT_CPU = ARCH_STM32F4

DDEFS += -DLED_CRITICAL_ERROR_BRAIN_PIN=GPIO

# We're running on TDG PDM hardware!
DDEFS += -DHW_TDG_PDM8=1


