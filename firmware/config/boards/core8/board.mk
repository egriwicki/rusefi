# List of all the board related files.
BOARDCPPSRC =  $(BOARD_DIR)/board_configuration.cpp

# Override DEFAULT_ENGINE_TYPE
DDEFS += -DSHORT_BOARD_NAME=core8 -DSTATIC_BOARD_ID=STATIC_BOARD_ID_CORE8
DDEFS += -DFIRMWARE_ID=\"core8\"
DDEFS += -DDEFAULT_ENGINE_TYPE=engine_type_e::MINIMAL_PINS
