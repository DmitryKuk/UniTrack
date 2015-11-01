# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.


# Helper functions
include $(MK_UTILITY_ABS)

# Recursive targets' and modules' targets
include $(MK_RECURSIVE_ABS)


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
	OBJ=$(subst $(OBJ_DIR_CURR)/,,$@);											\
	echo '    $(COLOR_RUN)[$(TARGET_TYPE): $(TARGET_NAME)] '					\
		 "Compiling: $$OBJ...$(COLOR_RESET)";									\
	$(call gpp_compile) -o '$@' '$<';											\
	STATUS=$$?;																	\
	if [ "X$$STATUS" != 'X0' ]; then 											\
		echo '$(COLOR_FAIL)==> [$(TARGET_TYPE): $(TARGET_NAME)] '				\
			 "Object file $$OBJ building failed"								\
			 "(status: $$STATUS).$(COLOR_RESET)";								\
		rm '$@' 2>/dev/null || true;											\
	fi


# Target executable linking
$(TARGET_FILE): $(HEADER_FILES) $(OBJ_FILES) $(MAIN_OBJ_FILES) $(call get_lib_files,$(MODULE_DEPS))
	echo '    $(COLOR_RUN)[$(TARGET_TYPE): $(TARGET_NAME)] '					\
		 'Linking executable: $(subst $(BIN_DIR_CURR)/,,$@)...$(COLOR_RESET)';	\
	$(call gpp_link) -o '$@' $(OBJ_FILES) $(MAIN_OBJ_FILES);					\
	STATUS=$$?;																	\
	if [ "X$$STATUS" == 'X0' ]; then											\
		echo '$(COLOR_PASS)==> [$(TARGET_TYPE): $(TARGET_NAME)] '				\
			 'Built successfully.$(COLOR_RESET)';								\
	else																		\
		echo '$(COLOR_FAIL)==> [$(TARGET_TYPE): $(TARGET_NAME)] '				\
			 "Building failed (status: $$STATUS).$(COLOR_RESET)";				\
	fi


# Target shared library linking
$(TARGET_LIB_FILE): $(HEADER_FILES) $(OBJ_FILES) $(call get_lib_files,$(MODULE_DEPS))
	echo '    $(COLOR_RUN)[$(TARGET_TYPE): $(TARGET_NAME)] '					\
		 'Linking shared lib: $(subst $(LIB_DIR_CURR)/,,$@)...$(COLOR_RESET)';	\
	$(call gpp_shared_lib) -o '$@' $(OBJ_FILES);								\
	STATUS=$$?;																	\
	if [ "X$$STATUS" == 'X0' ]; then											\
		echo '$(COLOR_PASS)==> [$(TARGET_TYPE): $(TARGET_NAME)] '				\
			 'Built successfully.$(COLOR_RESET)';								\
	else																		\
		echo '$(COLOR_FAIL)==> [$(TARGET_TYPE): $(TARGET_NAME)] '				\
			 "Building failed (status: $$STATUS).$(COLOR_RESET)";				\
	fi


# Tests
$(TEST_DIR_CURR)/%: $(OBJ_DIR_CURR)/%.o $(HEADER_FILES) $(TARGET_FILE)
	TEST=$(subst $(TEST_DIR_CURR)/,,$@);										\
	echo '    $(COLOR_RUN)[$(TARGET_TYPE): $(TARGET_NAME)] '					\
		 "Linking test: $$TEST...$(COLOR_RESET)";								\
	$(call gpp_link) -o '$@' '$<' $(OBJ_FILES);									\
	STATUS=$$?;																	\
	if [ "X$$STATUS" == 'X0' ]; then											\
		echo '$(COLOR_PASS)==> [$(TARGET_TYPE): $(TARGET_NAME)] '				\
			 'Test built successfully.$(COLOR_RESET)';							\
	else																		\
		echo '$(COLOR_FAIL)==> [$(TARGET_TYPE): $(TARGET_NAME)] '				\
			 "Test building failed (status: $$STATUS).$(COLOR_RESET)";			\
	fi
