
CONSOLESRC = 

CONSOLE_SRC_CPP = $(PROJECT_DIR)/console/status_loop.cpp \
	$(PROJECT_DIR)/console/console_io.cpp \
	$(PROJECT_DIR)/console/eficonsole.cpp \
	$(PROJECT_DIR)/console/connector_uart_dma.cpp \
	$(PROJECT_DIR)/console/binary/tooth_logger.cpp \
	$(PROJECT_DIR)/console/binary_log/log_field.cpp \
	$(PROJECT_DIR)/console/binary_log/binary_logging.cpp \


CONSOLE_INC=\
	$(PROJECT_DIR)/console/binary \
	$(PROJECT_DIR)/console \
	$(PROJECT_DIR)/console/binary_log

