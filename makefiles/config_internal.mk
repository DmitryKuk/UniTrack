# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This is the root Makefile of project.


# ---===       Project internal properties        ===---

# Project directories (addresses with $(PROJECT_ROOT) as current directory)
# Sources only
SRC_DIR					= src

# Modules sources only (without `src/' prefix)
MODULES_ONLY_DIR		= modules

# Modules sources only
MODULES_SRC_DIR			= $(SRC_DIR)/$(MODULES_ONLY_DIR)

# Targets sources only
TARGETS_DIR				= $(SRC_DIR)/targets


# Building results only
BUILD_DIR				= build

# Executables
BIN_DIR					= $(BUILD_DIR)/bin

# Dynamic libraries
LIB_DIR					= $(BUILD_DIR)/lib

# All .o files (including tests)
OBJ_DIR					= $(BUILD_DIR)/obj
MODULES_OBJ_DIR			= $(OBJ_DIR)/$(MODULES_ONLY_DIR)

# All test executables
TEST_DIR				= $(BUILD_DIR)/test
MODULES_TEST_DIR		= $(TEST_DIR)/$(MODULES_ONLY_DIR)


# Config
CONFIG_DIR				= config

# WWW data
WWW_DIR					= www


# Third-party
THIRDPARTY_DIR			= third-party

# ---===    End of project internal properties    ===---



# Absolute paths
export SRC_DIR_ABS			= $(PROJECT_ROOT)/$(SRC_DIR)
export MODULES_DIR_ABS		= $(PROJECT_ROOT)/$(MODULES_DIR)
export TARGETS_DIR_ABS		= $(PROJECT_ROOT)/$(TARGETS_DIR)

export BUILD_DIR_ABS		= $(PROJECT_ROOT)/$(BUILD_DIR)
export BIN_DIR_ABS			= $(PROJECT_ROOT)/$(BIN_DIR)
export LIB_DIR_ABS			= $(PROJECT_ROOT)/$(LIB_DIR)
export OBJ_DIR_ABS			= $(PROJECT_ROOT)/$(OBJ_DIR)
export TEST_DIR_ABS			= $(PROJECT_ROOT)/$(TEST_DIR)

export CONFIG_DIR_ABS		= $(PROJECT_ROOT)/$(CONFIG_DIR)
export WWW_DIR_ABS			= $(PROJECT_ROOT)/$(WWW_DIR)

export THIRDPARTY_DIR_ABS	= $(PROJECT_ROOT)/$(THIRDPARTY_DIR)

export MODULES_SRC_DIR_ABS	= $(PROJECT_ROOT)/$(MODULES_SRC_DIR)
export MODULES_OBJ_DIR_ABS	= $(PROJECT_ROOT)/$(MODULES_OBJ_DIR)
export MODULES_TEST_DIR_ABS	= $(PROJECT_ROOT)/$(MODULES_TEST_DIR)
