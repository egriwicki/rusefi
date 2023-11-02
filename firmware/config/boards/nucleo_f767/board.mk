# List of all the board related files.

# F429 and F767 Nucleo are indeed the same board with a different chip fitted - so recycle the F429 config
BOARDCPPSRC = $(BOARDS_DIR)/nucleo_f429/board_configuration.cpp

# reducing flash consumption for EFI_ETHERNET to fit
DDEFS += -DEFI_FILE_LOGGING=FALSE -DEFI_ALTERNATOR_CONTROL=FALSE -DEFI_LOGIC_ANALYZER=FALSE -DEFI_ENABLE_ASSERTS=FALSE

DDEFS += -DLED_CRITICAL_ERROR_BRAIN_PIN=Gpio::B14

# Enable ethernet
LWIP = yes
ALLOW_SHADOW = yes
DDEFS += -DEFI_ETHERNET=TRUE

DEBUG_LEVEL_OPT = -Os -ggdb -g

DDEFS += -DFIRMWARE_ID=\"nucleo_f767\"
DDEFS += -DDEFAULT_ENGINE_TYPE=engine_type_e::MINIMAL_PINS
DDEFS += -DSHORT_BOARD_NAME=nucleo_f767 -DSTATIC_BOARD_ID=STATIC_BOARD_ID_NUCLEO_F767
