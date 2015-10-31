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
OBJ_DIR_CURR				= $(MODULES_OBJ_DIR_ABS)/$(TARGET_NAME)
TEST_DIR_CURR				= $(MODULES_TEST_DIR_ABS)/$(TARGET_NAME)
TARGETS_SRC_DIR_CURR		= $(TARGETS_SRC_DIR_ABS)/$(TARGET_NAME)


GPP_HEADER_PATHS			+= -I'$(TARGETS_SRC_DIR_CURR)'


# Sources and headers
HEADER_FILES =										\
	$(shell find '$(SRC_DIR_CURR)'					\
		\( -type f -or -type l \)					\
		-not -name 'test*'							\
		\( -name '*.h' -or -name '*.hpp' \))


SRC_CPP_FILES =										\
	$(shell find '$(SRC_DIR_CURR)'					\
		\( -type f -or -type l \)					\
		-not \( -name 'test*' -or -name 'main*' \)	\
		-name '*.cpp')


MAIN_CPP_FILES =									\
	$(shell find '$(SRC_DIR_CURR)'					\
		\( -type f -or -type l \)					\
		-not -name 'test*'							\
		-name '*.cpp')


# Tests
TEST_SRC_CPP_FILES =								\
	$(shell find '$(SRC_DIR_CURR)'					\
		\( -type f -or -type l \)					\
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
.PHONY: all clean clean-tests check dirs main tests run-tests
.SILENT:


all: dirs main


clean: clean-tests
	rm -rf $(OBJ_FILES) $(TARGET_FILE) 2>/dev/null || true


clean-tests:
	rm -rf $(TEST_OBJ_FILES) $(TEST_TARGET_FILES) 2>/dev/null || true


check: dirs run-tests


dirs:
	@echo mkdir -p $(sort $(dir $(TARGET_FILE) $(OBJ_FILES) $(MAIN_OBJ_FILES) \
						  $(TEST_OBJ_FILES) $(TEST_TARGET_FILES)))


main: $(TARGET_FILE)


tests: $(TEST_TARGET_FILES)


run-tests: tests
	if [ "X$(TEST_TARGETS)" != "X" ]; then																		\
		for T in $(TEST_TARGETS); do																			\
			echo "$(COLOR_RUN)[$(TARGET_NAME)]  Running test: $$T...$(COLOR_RESET)";							\
			$(call get_test_files,$$T);																			\
			STATUS=$$?;																							\
			if [ "X$$STATUS" == 'X0' ]; then																	\
				echo "$(COLOR_PASS)==> [$(TARGET_NAME)]  Test $$T passed.$(COLOR_RESET)";						\
			else																								\
				echo "$(COLOR_FAIL)==> [$(TARGET_NAME)]  Test $$T failed with code: $$STATUS.$(COLOR_RESET)";	\
			fi;																									\
		done;																									\
	fi


# Objects compilation (universal for main program and tests)
$(OBJ_DIR_CURR)/%.o: $(SRC_DIR_CURR)/%.cpp $(HEADER_FILES)
	@echo "    $(COLOR_RUN)[$(TARGET_NAME)]  Compiling: $(subst $(OBJ_DIR_CURR)/,,$@)...$(COLOR_RESET)"
	@echo $(call gpp_compile) -o '$@' '$<'


$(TARGET_FILE): $(HEADER_FILES) $(OBJ_FILES) $(MAIN_OBJ_FILES)
	@echo "    $(COLOR_RUN)[$(TARGET_NAME)]  Linking executable: $(subst $(BIN_DIR_CURR)/,,$@)...$(COLOR_RESET)"
	@echo $(call gpp_link) -o '$@' $(OBJ_FILES) $(MAIN_OBJ_FILES)


# Tests
$(TEST_DIR_CURR)/%: $(OBJ_DIR_CURR)/%.o $(HEADER_FILES) $(TARGET_FILE)
	@echo "    $(COLOR_RUN)[$(TARGET_NAME)]  Linking test: $(subst $(TEST_DIR_CURR)/,,$@)...$(COLOR_RESET)"
	@echo $(call gpp_link) -o '$@' '$<' $(OBJ_FILES)
