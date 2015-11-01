# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.


# Helper functions
include $(MAKEFILE_UTILITY_ABS)

# Target config
include config.mk


GPP_LIBS					+= $(call get_external_libs,$(EXTERNAL_LIBS))
GPP_LIBS					+= $(call get_libs,$(MODULE_DEPS))


# Paths
SRC_DIR_CURR				= $(TARGETS_SRC_DIR_ABS)/$(TARGET_NAME)
BIN_DIR_CURR				= $(BIN_DIR_ABS)
OBJ_DIR_CURR				= $(TARGETS_OBJ_DIR_ABS)/$(TARGET_NAME)
TEST_DIR_CURR				= $(TARGETS_TEST_DIR_ABS)/$(TARGET_NAME)
TARGETS_SRC_DIR_CURR		= $(TARGETS_SRC_DIR_ABS)/$(TARGET_NAME)


GPP_HEADER_PATHS			+= -I'$(TARGETS_SRC_DIR_CURR)'


# Sources and headers
HEADER_FILES =												\
	$(shell find '$(SRC_DIR_CURR)'							\
		\( -type f -or -type l \)							\
		-not -name 'test*'									\
		\( -name '*.h' -or -name '*.hpp' \))


# .cpp files in BFS order
SRC_CPP_FILES =												\
	$(shell													\
		DIRS=$(SRC_DIR_CURR);								\
		while [ "X$$DIRS" != "X" ]; do						\
			find $$DIRS -d 1 \( -type f -or -type l \) 		\
				 -not \( -name 'test*' -or -name 'main*' \)	\
				 -name '*.cpp';								\
			DIRS=$$(find $$DIRS -d 1 -type d);				\
		done												\
	)


# main*.cpp files in BFS order
MAIN_CPP_FILES =											\
	$(shell													\
		DIRS=$(SRC_DIR_CURR);								\
		while [ "X$$DIRS" != "X" ]; do						\
			find $$DIRS -d 1 \( -type f -or -type l \) 		\
				 -name 'main*.cpp';							\
			DIRS=$$(find $$DIRS -d 1 -type d);				\
		done												\
	)


# Tests
TEST_SRC_CPP_FILES =										\
	$(shell find '$(SRC_DIR_CURR)'							\
		\( -type f -or -type l \)							\
		-name 'test*.cpp')


# Target dynamic library
TARGET_FILE					= $(call get_bin_files,$(TARGET_NAME))

# Objects
OBJ_CPPS					= $(subst $(SRC_DIR_CURR)/,,$(SRC_CPP_FILES))
OBJ_FILES					= $(call get_obj_files,$(call cpp_to_obj,$(OBJ_CPPS)))

MAIN_OBJ_CPPS				= $(subst $(SRC_DIR_CURR)/,,$(MAIN_CPP_FILES))
MAIN_OBJ_FILES				= $(call get_obj_files,$(call cpp_to_obj,$(MAIN_OBJ_CPPS)))

# Tests
TEST_CPPS					= $(subst $(SRC_DIR_CURR)/,,$(TEST_SRC_CPP))
TEST_OBJ_FILES				= $(call get_obj_files,$(call cpp_to_obj,$(TEST_CPPS)))

TEST_TARGETS				= $(call get_targets,$(TEST_CPPS))
TEST_TARGET_FILES			= $(call get_test_files,$(TEST_TARGETS))


# Targets
export TARGET_TYPE			= Target
# TARGET_NAME is defined, when this make command is called
include $(MAKEFILE_TARGETS_ABS)
