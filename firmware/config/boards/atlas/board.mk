# List of all the board related files.
BOARDCPPSRC =  $(PROJECT_DIR)/config/boards/atlas/board_configuration.cpp

BOARDINC = $(PROJECT_DIR)/config/boards/atlas

#DDEFS += -DLED_CRITICAL_ERROR_BRAIN_PIN=Gpio::E3
DDEFS += -DFIRMWARE_ID=\"atlas\"

# This stuff doesn't work on H7 yet
# DDEFS += -DSTM32_ADC_USE_ADC3=TRUE
# DEFS += -DEFI_SOFTWARE_KNOCK=TRUE

# disable hardware serial ports on H7
# DDEFS += -DTS_NO_PRIMARY=1 -DTS_NO_SECONDARY=1

# We are running on Atlas hardware!
DDEFS += -DHW_ATLAS=1

# Atlas needs networking library
LWIP = yes
DDEFS += -DEFI_ETHERNET=TRUE

DDEFS += -DSHORT_BOARD_NAME=atlas
