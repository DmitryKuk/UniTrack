# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.


THIS_MAKEFILE				:= $(call where-am-i)

# Target name: logger, unitrack, etc...
TARGET_NAME					:= $(call target_name_from_this,$(THIS_MAKEFILE))


# Helper functions
include $(MK_UTILITY)


# Target config
MODULE_DEPS					:= 
EXTERNAL_LIBS				:= 
COMPILE_FLAGS				:= 
SHARED_LIB_FLAGS			:= 
LINK_FLAGS					:= 
-include $(dir $(THIS_MAKEFILE))/config.mk


# Unique target data
ID							:= target_$(TARGET_NAME)
$(ID)_TARGET_NAME			:= $(TARGET_NAME)
$(ID)_TARGET_TYPE			:= Target


$(ID)_MODULE_DEPS			:= $(MODULE_DEPS)
$(ID)_EXTERNAL_LIBS			:= $(EXTERNAL_LIBS)
$(ID)_GPP_COMPILE_FLAGS		:= $(COMPILE_FLAGS)
$(ID)_GPP_LINK_FLAGS		:= $(LINK_FLAGS)


$(ID)_GPP_LIBS				:= $(call get_external_libs,$($(ID)_EXTERNAL_LIBS))	\
							   $(call get_libs,$($(ID)_MODULE_DEPS))


# Paths
$(ID)_SRC_DIR_CURR			:= $(TARGETS_SRC_DIR)/$($(ID)_TARGET_NAME)
$(ID)_OBJ_DIR_CURR			:= $(TARGETS_OBJ_DIR)/$($(ID)_TARGET_NAME)
$(ID)_TEST_DIR_CURR			:= $(TARGETS_TEST_DIR)/$($(ID)_TARGET_NAME)


$(ID)_GPP_HEADER_PATHS		:= -I'$($(ID)_SRC_DIR_CURR)'


# Sources and headers
$(ID)_HEADER_FILES :=														\
	$(shell find '$($(ID)_SRC_DIR_CURR)'									\
		\( -type f -or -type l \)											\
		-not -name 'test*'													\
		\( -name '*.h' -or -name '*.hpp' \))


# .cpp files in BFS order
$(ID)_SRC_CPP_FILES :=														\
	$(shell																	\
		DIRS=$($(ID)_SRC_DIR_CURR);											\
		while [ "X$$DIRS" != "X" ]; do										\
			find $$DIRS -maxdepth 1 -mindepth 1 \( -type f -or -type l \)	\
				 -not \( -name 'test*' -or -name 'main*' \)					\
				 -name '*.cpp';												\
			DIRS=$$(find $$DIRS -maxdepth 1 -mindepth 1 -type d);			\
		done																\
	)


# main*.cpp files in BFS order
$(ID)_MAIN_CPP_FILES :=														\
	$(shell																	\
		DIRS=$($(ID)_SRC_DIR_CURR);											\
		while [ "X$$DIRS" != "X" ]; do										\
			find $$DIRS -maxdepth 1 -mindepth 1 \( -type f -or -type l \)	\
				 -name 'main*.cpp';											\
			DIRS=$$(find $$DIRS -maxdepth 1 -mindepth 1 -type d);			\
		done																\
	)


# Tests
$(ID)_TEST_SRC_CPP_FILES :=													\
	$(shell find '$($(ID)_SRC_DIR_CURR)'									\
		\( -type f -or -type l \)											\
		-name 'test*.cpp')


# Target dynamic library
$(ID)_TARGET_FILE			:= $(call get_bin_files,$($(ID)_TARGET_NAME))

# Objects
$(ID)_OBJ_CPPS				:= $(subst $($(ID)_SRC_DIR_CURR)/,,$($(ID)_SRC_CPP_FILES))
$(ID)_OBJ_FILES				:= $(call get_obj_files,$(call cpp_to_obj,$($(ID)_OBJ_CPPS)))

$(ID)_MAIN_OBJ_CPPS			:= $(subst $($(ID)_SRC_DIR_CURR)/,,$($(ID)_MAIN_CPP_FILES))
$(ID)_MAIN_OBJ_FILES		:= $(call get_obj_files,$(call cpp_to_obj,$($(ID)_MAIN_OBJ_CPPS)))

# Tests
$(ID)_TEST_CPPS				:= $(subst $($(ID)_SRC_DIR_CURR)/,,$($(ID)_TEST_SRC_CPP_FILES))
$(ID)_TEST_OBJ_FILES		:= $(call get_obj_files,$(call cpp_to_obj,$($(ID)_TEST_CPPS)))

$(ID)_TEST_TARGETS			:= $(call get_targets,$($(ID)_TEST_CPPS))
$(ID)_TEST_TARGET_FILES		:= $(call get_test_files,$($(ID)_TEST_TARGETS))


# Global list of test targets
TEST_TARGET_FILES			:= $(TEST_TARGET_FILES) $($(ID)_TEST_TARGET_FILES)


# Targets
include $(MK_TARGETS)
