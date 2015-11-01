# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.


MODULE_NAME					= $(TARGET_NAME)


# Helper functions
include $(MK_UTILITY_ABS)

# Module config
-include config.mk


GPP_LIBS					+= $(call get_external_libs,$(EXTERNAL_LIBS))
GPP_LIBS					+= $(call get_libs,$(MODULE_DEPS))


# Paths
SRC_DIR_CURR				= $(MODULES_SRC_DIR_ABS)/$(MODULE_NAME)
LIB_DIR_CURR				= $(LIB_DIR_ABS)
OBJ_DIR_CURR				= $(MODULES_OBJ_DIR_ABS)/$(MODULE_NAME)
TEST_DIR_CURR				= $(MODULES_TEST_DIR_ABS)/$(MODULE_NAME)


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


# Tests
TEST_SRC_CPP_FILES =										\
	$(shell find '$(SRC_DIR_CURR)'							\
		\( -type f -or -type l \)							\
		-name 'test*.cpp')


# Target dynamic library
TARGET_LIB_FILE				= $(call get_lib_files,$(MODULE_NAME))

# Objects
OBJ_CPPS					= $(subst $(SRC_DIR_CURR)/,,$(SRC_CPP_FILES))
OBJ_FILES					= $(call get_obj_files,$(call cpp_to_obj,$(OBJ_CPPS)))

# Tests
TEST_CPPS					= $(subst $(SRC_DIR_CURR)/,,$(TEST_SRC_CPP))
TEST_OBJ_FILES				= $(call get_obj_files,$(call cpp_to_obj,$(TEST_CPPS)))

TEST_TARGETS				= $(call get_targets,$(TEST_CPPS))
TEST_TARGET_FILES			= $(call get_test_files,$(TEST_TARGETS))


# Targets
export TARGET_TYPE			= Module
include $(MK_TARGETS_ABS)
