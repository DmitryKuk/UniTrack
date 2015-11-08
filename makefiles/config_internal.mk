# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This is the root Makefile of project.


# ---===       Project internal properties        ===---

# Project directories (addresses with $(PROJECT_ROOT) as current directory)
# Sources
SRC_DIR					= src

# Building results
BUILD_DIR				= build


# Executables
BIN_DIR					= $(BUILD_DIR)/bin

# Dynamic libraries
LIB_DIR					= $(BUILD_DIR)/lib

# All .o files (including tests)
OBJ_DIR					= $(BUILD_DIR)/obj

# All test executables
TEST_DIR				= $(BUILD_DIR)/test


# Modules dirs
MODULES_ONLY_DIR		= modules
MODULES_SRC_DIR			= $(SRC_DIR)/$(MODULES_ONLY_DIR)
MODULES_OBJ_DIR			= $(OBJ_DIR)/$(MODULES_ONLY_DIR)
MODULES_TEST_DIR		= $(TEST_DIR)/$(MODULES_ONLY_DIR)


# Targets dirs
TARGETS_ONLY_DIR		= targets
TARGETS_SRC_DIR			= $(SRC_DIR)/$(TARGETS_ONLY_DIR)
TARGETS_OBJ_DIR			= $(OBJ_DIR)/$(TARGETS_ONLY_DIR)
TARGETS_TEST_DIR		= $(TEST_DIR)/$(TARGETS_ONLY_DIR)


# Config
CONFIG_DIR				= config

# WWW data
WWW_DIR					= www


# Third-party
THIRDPARTY_DIR			= third-party

# ---===    End of project internal properties    ===---
