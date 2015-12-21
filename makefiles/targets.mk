# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.


# Helper functions
include $(MK_UTILITY)


# Objects compilation (universal for main program and tests)
$($(ID)_OBJ_DIR_CURR)/%.o: ID := $(ID)
$($(ID)_OBJ_DIR_CURR)/%.o: $($(ID)_SRC_DIR_CURR)/%.cpp
	OBJ=$(subst $($(ID)_OBJ_DIR_CURR)/,,$@);										\
	echo -e "$(COLOR_RUN)    Compiling:"											\
			" [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]:"						\
			" $$OBJ...$(COLOR_RESET)";												\
	$(call gpp_compile,$@,'$<');													\
	STATUS=$$?;																		\
	if [ "X$$STATUS" != 'X0' ]; then 												\
		echo -e '$(COLOR_FAIL)==> Object file building failed:'						\
				" [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]:"					\
				" $$OBJ (status: $$STATUS).$(COLOR_RESET)";							\
	fi


# Target executable linking
$($(ID)_TARGET_FILE): ID := $(ID)
$($(ID)_TARGET_FILE): $($(ID)_OBJ_FILES) $($(ID)_MAIN_OBJ_FILES)					\
					  $(call get_lib_files,$($(ID)_MODULE_DEPS))
	echo -e "$(COLOR_RUN)Linking executable:"										\
			' [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]:'						\
			" $(subst $(BIN_DIR)/,,$@)...$(COLOR_RESET)";							\
	$(call gpp_link,$@,$($(ID)_OBJ_FILES) $($(ID)_MAIN_OBJ_FILES));					\
	STATUS=$$?;																		\
	if [ "X$$STATUS" = 'X0' ]; then													\
		echo -e "$(COLOR_PASS)==> Built successfully:"								\
				" [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)].$(COLOR_RESET)";		\
	else																			\
		echo -e "$(COLOR_FAIL)==> Building failed:"									\
				" [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]"						\
				"(status: $$STATUS).$(COLOR_RESET)";								\
	fi


# Target shared library linking
$($(ID)_TARGET_LIB_FILE): ID := $(ID)
$($(ID)_TARGET_LIB_FILE): $($(ID)_OBJ_FILES)										\
						  $(call get_lib_files,$($(ID)_MODULE_DEPS))
	echo -e "$(COLOR_RUN)Linking shared lib:"										\
			' [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]:'						\
			" $(subst $(LIB_DIR)/,,$@)...$(COLOR_RESET)";							\
	$(call gpp_shared_lib,$@,$($(ID)_OBJ_FILES));									\
	STATUS=$$?;																		\
	if [ "X$$STATUS" = 'X0' ]; then													\
		echo -e "$(COLOR_PASS)==> Built successfully:"								\
				" [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)].$(COLOR_RESET)";		\
	else																			\
		echo -e "$(COLOR_FAIL)==> Building failed:"									\
				" [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]"						\
				"(status: $$STATUS).$(COLOR_RESET)";								\
	fi


# Tests
$($(ID)_TEST_DIR_CURR)/%: ID := $(ID)
$($(ID)_TEST_DIR_CURR)/%: $($(ID)_OBJ_DIR_CURR)/%.o									\
						  $($(ID)_OBJ_FILES) $($(ID)_TARGET_LIB_FILE)				\
						  $(call get_lib_files,$($(ID)_MODULE_DEPS))
	TEST=$(subst $($(ID)_TEST_DIR_CURR)/,,$@);										\
	echo -e "$(COLOR_RUN)Linking test:"												\
			' [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]:'						\
			" $$TEST...$(COLOR_RESET)";												\
	$(call gpp_link,$@,'$<' $($(ID)_OBJ_FILES));									\
	STATUS=$$?;																		\
	if [ "X$$STATUS" = 'X0' ]; then													\
		echo -e "$(COLOR_PASS)==> Test built successfully:"							\
				" [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)].$(COLOR_RESET)";		\
	else																			\
		echo -e "$(COLOR_FAIL)==> Test building failed:"							\
				" [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)]"						\
				"(status: $$STATUS).$(COLOR_RESET)";								\
	fi


# Try to include dependencies makefiles
-include $(call obj_to_dep,$($(ID)_OBJ_FILES))
