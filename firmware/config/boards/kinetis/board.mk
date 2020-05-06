# List of all the board related files.

BOARD_DIR = $(PROJECT_DIR)/config/boards/$(PROJECT_BOARD)

BOARDSRC = $(BOARD_DIR)/board.c
BOARDSRC_CPP = $(BOARD_DIR)/board_configuration.cpp

# Required include directories
BOARDINC = $(BOARD_DIR)
BOARDINC += $(BOARD_DIR)/config/controllers/algo

BOARDSRC_CPP += $(CONTROLLERS_ALGO_AUTO_GENERATED_ENUMS)

# Define linker script file here
LDSCRIPT= $(STARTUPLD)/MKE1xF512.ld

CONFDIR = $(BOARD_DIR)
