include $(PROJECT_DIR)/hw_layer/ports/stm32/stm32_common.mk

HW_LAYER_PORT += $(PROJECT_DIR)/hw_layer/ports/stm32/stm32f4/stm32f4xx_hal_flash.c \
				$(PROJECT_DIR)/hw_layer/ports/stm32/stm32f4/stm32f4xx_hal_flash_ex.c
               
HW_LAYER_PORT_CPP += $(PROJECT_DIR)/hw_layer/ports/stm32/stm32f4/mpu_util.cpp \
					$(PROJECT_DIR)/hw_layer/ports/stm32/stm32_adc_v2.cpp

MCU = cortex-m4
LDSCRIPT = $(PROJECT_DIR)/hw_layer/ports/stm32/stm32f4/STM32F4.ld
# kludge: while we the very generic ChibiOS board.c we use our custom board.h from current folder!
ALLCSRC += $(CHIBIOS)/os/hal/boards/ST_STM32F4_DISCOVERY/board.c
CONFDIR = $(PROJECT_DIR)/hw_layer/ports/stm32/stm32f4/cfg

# STM32F42x has extra memory, so change some flags so we can use it.
ifeq ($(IS_STM32F429),yes)
	USE_OPT += -Wl,--defsym=STM32F4_HAS_SRAM3=1
	DDEFS += -DSTM32F429xx
	DDEFS += -DEFI_IS_F42x
else
	DDEFS += -DSTM32F407xx
endif

# TODO: remove, for efifeatures.h
ALLINC += $(PROJECT_DIR)/config/stm32f4ems
