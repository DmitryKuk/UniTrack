# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.


# Helper functions
include $(MK_UTILITY)


# Objects compilation (universal for main program and tests)
$($(ID)_OBJ_DIR_CURR)/%.o: ID := $(ID)
$($(ID)_OBJ_DIR_CURR)/%.o: $($(ID)_SRC_DIR_CURR)/%.cpp $($(ID)_HEADER_FILES)
	OBJ=$(subst $($(ID)_OBJ_DIR_CURR)/,,$@);									\
	echo "$(COLOR_RUN)    Compiling:"											\
		 ' [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]:'						\
		 " $$OBJ...$(COLOR_RESET)";												\
	$(call gpp_compile) -o '$@' '$<';											\
	STATUS=$$?;																	\
	if [ "X$$STATUS" != 'X0' ]; then 											\
		echo '$(COLOR_FAIL)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] '	\
			 "Object file $$OBJ building failed"								\
			 "(status: $$STATUS).$(COLOR_RESET)";								\
		rm '$@' 2>/dev/null || true;											\
	fi


# Target executable linking
$($(ID)_TARGET_FILE): ID := $(ID)
$($(ID)_TARGET_FILE): $($(ID)_HEADER_FILES) $($(ID)_OBJ_FILES) $($(ID)_MAIN_OBJ_FILES) $(call get_lib_files,$($(ID)_MODULE_DEPS))
	echo "$(COLOR_RUN)Linking executable:"										\
		 ' [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]:'						\
		 " $(subst $(BIN_DIR)/,,$@)...$(COLOR_RESET)";							\
	$(call gpp_link) -o '$@' $($(ID)_OBJ_FILES) $($(ID)_MAIN_OBJ_FILES);		\
	STATUS=$$?;																	\
	if [ "X$$STATUS" == 'X0' ]; then											\
		echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "	\
			 "Built successfully.$(COLOR_RESET)";								\
	else																		\
		echo "$(COLOR_FAIL)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "	\
			 "Building failed (status: $$STATUS).$(COLOR_RESET)";				\
	fi


# Target shared library linking
$($(ID)_TARGET_LIB_FILE): ID := $(ID)
$($(ID)_TARGET_LIB_FILE): $($(ID)_HEADER_FILES) $($(ID)_OBJ_FILES) $(call get_lib_files,$($(ID)_MODULE_DEPS))
	echo "$(COLOR_RUN)Linking shared lib:"										\
		 ' [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]:'						\
		 " $(subst $(LIB_DIR)/,,$@)...$(COLOR_RESET)";							\
	$(call gpp_shared_lib) -o '$@' $($(ID)_OBJ_FILES);							\
	STATUS=$$?;																	\
	if [ "X$$STATUS" == 'X0' ]; then											\
		echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "	\
			 "Built successfully.$(COLOR_RESET)";								\
	else																		\
		echo "$(COLOR_FAIL)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "	\
			 "Building failed (status: $$STATUS).$(COLOR_RESET)";				\
	fi


# Tests
$($(ID)_TEST_DIR_CURR)/%: $($(ID)_OBJ_DIR_CURR)/%.o $($(ID)_HEADER_FILES) $($(ID)_TARGET_FILE)
	TEST=$(subst $($(ID)_TEST_DIR_CURR)/,,$@);									\
	echo "$(COLOR_RUN)Linking test:"											\
		 ' [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]:'						\
		 " $$TEST...$(COLOR_RESET)";											\
	$(call gpp_link) -o '$@' '$<' $($(ID)_OBJ_FILES);							\
	STATUS=$$?;																	\
	if [ "X$$STATUS" == 'X0' ]; then											\
		echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "	\
			 "Test built successfully.$(COLOR_RESET)";							\
	else																		\
		echo "$(COLOR_FAIL)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "	\
			 "Test building failed (status: $$STATUS).$(COLOR_RESET)";			\
	fi
