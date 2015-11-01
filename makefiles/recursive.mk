# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.


# Helper functions
include $(MK_UTILITY_ABS)


ifeq ($(TARGET_NAME),)
	PROMT_STR				= [Global]  Building module:
else
	PROMT_STR				= [$(TARGET_TYPE): $(TARGET_NAME)]  Building module dependency:
endif


LIB_DIR_CURR				= $(LIB_DIR_ABS)
BIN_DIR_CURR				= $(BIN_DIR_ABS)


# Targets
.SILENT:


# Module dependencies recursive building
$(LIB_DIR_CURR)/%.so:
	MODULE_NAME=$(call lib_to_module,$@);										\
	echo "    $(COLOR_RUN)$(PROMT_STR) $$MODULE_NAME...$(COLOR_RESET)";			\
	$(MAKE) -C "$(MODULES_SRC_DIR_ABS)/$$MODULE_NAME" TARGET_NAME=$$MODULE_NAME


# Targets recursive building
$(BIN_DIR_CURR)/%:
	TARGET_NAME=$(notdir $@);													\
	echo "    $(COLOR_RUN)$(PROMT_STR) $$TARGET_NAME...$(COLOR_RESET)";			\
	$(MAKE) -C "$(TARGETS_SRC_DIR_ABS)/$$TARGET_NAME" TARGET_NAME=$$TARGET_NAME
