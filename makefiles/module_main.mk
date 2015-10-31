# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.


# Helper functions
include $(MAKEFILE_UTILITY_ABS)

# Module config
include config.mk


GPP_LIBS					+= $(call get_external_libs,$(EXTERNAL_LIBS))
GPP_LIBS					+= $(call get_libs,$(MODULE_DEPS))


# Paths
SRC_DIR_CURR				= $(MODULES_SRC_DIR_ABS)/$(MODULE_NAME)
LIB_DIR_CURR				= $(LIB_DIR_ABS)
OBJ_DIR_CURR				= $(MODULES_OBJ_DIR_ABS)/$(MODULE_NAME)
TEST_DIR_CURR				= $(MODULES_TEST_DIR_ABS)/$(MODULE_NAME)


# Sources and headers
HEADER_FILES =									\
	$(shell find '$(SRC_DIR_CURR)'				\
		\( -type f -or -type l \)				\
		-not -name 'test*'						\
		\( -name '*.h' -or -name '*.hpp' \))


SRC_CPP_FILES =									\
	$(shell find '$(SRC_DIR_CURR)'				\
		\( -type f -or -type l \)				\
		-not -name 'test*'						\
		-name '*.cpp')


# Tests
TEST_SRC_CPP_FILES =							\
	$(shell find '$(SRC_DIR_CURR)'				\
		\( -type f -or -type l \)				\
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
.PHONY: all clean clean-tests check dirs main tests run-tests
.SILENT:


all: dirs main


clean: clean-tests
	rm -rf $(OBJ_FILES) $(TARGET_LIB_FILE) 2>/dev/null || true


clean-tests:
	rm -rf $(TEST_OBJ_FILES) $(TEST_TARGET_FILES) 2>/dev/null || true


check: dirs run-tests


dirs:
	mkdir -p $(sort $(dir $(TARGET_LIB_FILE) $(OBJ_FILES) \
						  $(TEST_OBJ_FILES) $(TEST_TARGET_FILES)))


main: $(TARGET_LIB_FILE)


tests: $(TEST_TARGET_FILES)


run-tests: tests
	if [ "X$(TEST_TARGETS)" != "X" ]; then																		\
		for T in $(TEST_TARGETS); do																			\
			echo "$(COLOR_RUN)[$(MODULE_NAME)]  Running test: $$T...$(COLOR_RESET)";							\
			$(call get_test_files,$$T);																			\
			STATUS=$$?;																							\
			if [ "X$$STATUS" == 'X0' ]; then																	\
				echo "$(COLOR_PASS)==> [$(MODULE_NAME)]  Test $$T passed.$(COLOR_RESET)";						\
			else																								\
				echo "$(COLOR_FAIL)==> [$(MODULE_NAME)]  Test $$T failed with code: $$STATUS.$(COLOR_RESET)";	\
			fi;																									\
		done;																									\
	fi


# Objects compilation (universal for main program and tests)
$(OBJ_DIR_CURR)/%.o: $(SRC_DIR_CURR)/%.cpp $(HEADER_FILES)
	@echo "    $(COLOR_RUN)[$(MODULE_NAME)]  Compiling: $(subst $(OBJ_DIR_CURR)/,,$@)...$(COLOR_RESET)"
	$(call gpp_compile) -o '$@' '$<'


$(TARGET_LIB_FILE): $(HEADER_FILES) $(OBJ_FILES)
	@echo "    $(COLOR_RUN)[$(MODULE_NAME)]  Linking shared lib: $(subst $(LIB_DIR_CURR)/,,$@)...$(COLOR_RESET)"
	$(call gpp_shared_lib) -o '$@' $(OBJ_FILES)


# Tests
$(TEST_DIR_CURR)/%: $(OBJ_DIR_CURR)/%.o $(HEADER_FILES) $(TARGET_LIB_FILE)
	@echo "    $(COLOR_RUN)[$(MODULE_NAME)]  Linking test: $(subst $(TEST_DIR_CURR)/,,$@)...$(COLOR_RESET)"
	$(call gpp_link) $(call get_libs,$(MODULE_NAME)) -o '$@' '$<'
