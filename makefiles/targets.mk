# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.


# Helper functions
include $(MAKEFILE_UTILITY_ABS)


# Targets
.PHONY: all clean clean-tests check dirs main tests run-tests
.SILENT:


all: dirs main


clean: clean-tests
	rm -rf $(OBJ_FILES) $(MAIN_OBJ_FILES) $(TARGET_FILE) $(TARGET_LIB_FILE) 2>/dev/null || true


clean-tests:
	rm -rf $(TEST_OBJ_FILES) $(TEST_TARGET_FILES) 2>/dev/null || true


check: dirs run-tests


dirs:
	mkdir -p $(sort $(dir $(TARGET_FILE) $(TARGET_LIB_FILE)						\
						  $(OBJ_FILES) $(MAIN_OBJ_FILES)						\
						  $(TEST_OBJ_FILES) $(TEST_TARGET_FILES)))


main: $(TARGET_FILE) $(TARGET_LIB_FILE)


tests: $(TEST_TARGET_FILES)


run-tests: tests
	if [ "X$(TEST_TARGETS)" != "X" ]; then										\
		for T in $(TEST_TARGETS); do											\
			echo "$(COLOR_RUN)[$(TARGET_TYPE): $(TARGET_NAME)] "				\
				 "Running test: $$T...$(COLOR_RESET)";							\
			$(call get_test_files,$$T);											\
			STATUS=$$?;															\
			if [ "X$$STATUS" == 'X0' ]; then									\
				echo "$(COLOR_PASS)==> [$(TARGET_TYPE): $(TARGET_NAME)] "		\
					 "Test $$T passed.$(COLOR_RESET)";							\
			else																\
				echo "$(COLOR_FAIL)==> [$(TARGET_TYPE): $(TARGET_NAME)] "		\
					 "Test $$T failed with code: $$STATUS.$(COLOR_RESET)";		\
			fi;																	\
		done;																	\
	fi


# Objects compilation (universal for main program and tests)
$(OBJ_DIR_CURR)/%.o: $(SRC_DIR_CURR)/%.cpp $(HEADER_FILES)
	@echo "    $(COLOR_RUN)[$(TARGET_TYPE): $(TARGET_NAME)] "					\
		  "Compiling: $(subst $(OBJ_DIR_CURR)/,,$@)...$(COLOR_RESET)"
	# @echo $(call gpp_compile) -o '$@' '$<'
	# sleep 0.5


# Target executable linking
$(TARGET_FILE): $(HEADER_FILES) $(OBJ_FILES) $(MAIN_OBJ_FILES) $(MODULE_DEPS)
	@echo "    $(COLOR_RUN)[$(TARGET_TYPE): $(TARGET_NAME)] "					\
		  "Linking executable: $(subst $(BIN_DIR_CURR)/,,$@)...$(COLOR_RESET)"
	# @echo $(call gpp_link) -o '$@' $(OBJ_FILES) $(MAIN_OBJ_FILES)
	# sleep 0.5


# Target shared library linking
$(TARGET_LIB_FILE): $(HEADER_FILES) $(OBJ_FILES) $(MODULE_DEPS)
	@echo "PROJECT_LIB_PREFIX: $(PROJECT_LIB_PREFIX)"
	@echo "    $(COLOR_RUN)[$(TARGET_TYPE): $(TARGET_NAME)] "					\
		  "Linking shared lib: $(subst $(LIB_DIR_CURR)/,,$@)...$(COLOR_RESET)"
	# @echo $(call gpp_shared_lib) -o '$@' $(OBJ_FILES)
	# sleep 0.5


# Tests
$(TEST_DIR_CURR)/%: $(OBJ_DIR_CURR)/%.o $(HEADER_FILES) $(TARGET_FILE)
	@echo "    $(COLOR_RUN)[$(TARGET_TYPE): $(TARGET_NAME)] "					\
		  "Linking test: $(subst $(TEST_DIR_CURR)/,,$@)...$(COLOR_RESET)"
	# @echo $(call gpp_link) -o '$@' '$<' $(OBJ_FILES)
	# sleep 0.5


%:
	@echo $(MAKE) -C "$(MODULES_SRC_DIR_ABS)/$@"
	$(MAKE) -C "$(MODULES_SRC_DIR_ABS)/$@" MODULE_NAME=$@
