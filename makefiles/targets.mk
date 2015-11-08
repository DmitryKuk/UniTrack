# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for server part of this program.

# $(warning Target $($(ID)_TARGET_LIB_FILE) $($(ID)_TARGET_FILE))

# Helper functions
include $(MK_UTILITY)


# Targets
# .PHONY: all start_message clean clean-tests check dirs main tests run-tests run install uninstall
# .SILENT:


# all: start_message dirs main


# start_message:
# 	@echo "$(COLOR_RUN)[$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "						\
# 		  "Building...$(COLOR_RESET)"


# clean: clean-tests
# 	rm -rf $($(ID)_OBJ_FILES) $($(ID)_MAIN_OBJ_FILES) $($(ID)_TARGET_FILE) $($(ID)_TARGET_LIB_FILE) 2>/dev/null || true


# clean-tests:
# 	rm -rf $($(ID)_TEST_OBJ_FILES) $($(ID)_TEST_TARGET_FILES) 2>/dev/null || true


# check: run-tests


# dirs:
# 	mkdir -p $(sort $(dir $($(ID)_TARGET_FILE) $($(ID)_TARGET_LIB_FILE)						\
# 						  $($(ID)_OBJ_FILES) $($(ID)_MAIN_OBJ_FILES)						\
# 						  $($(ID)_TEST_OBJ_FILES) $($(ID)_TEST_TARGET_FILES)))


# main: $($(ID)_TARGET_FILE) $($(ID)_TARGET_LIB_FILE)


# tests: $($(ID)_TEST_TARGET_FILES)


# run-tests: dirs tests
# 	if [ "X$(TEST_TARGETS)" != "X" ]; then										\
# 		for T in $(TEST_TARGETS); do											\
# 			echo "$(COLOR_RUN)[$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "				\
# 				 "Running test: $$T...$(COLOR_RESET)";							\
# 			$(call get_test_files,$$T);											\
# 			STATUS=$$?;															\
# 			if [ "X$$STATUS" == 'X0' ]; then									\
# 				echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "		\
# 					 "Test $$T passed.$(COLOR_RESET)";							\
# 			else																\
# 				echo "$(COLOR_FAIL)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "		\
# 					 "Test $$T failed with code: $$STATUS.$(COLOR_RESET)";		\
# 			fi;																	\
# 		done;																	\
# 	fi


# Objects compilation (universal for main program and tests)
$($(ID)_OBJ_DIR_CURR)/%.o: ID := $(ID)
$($(ID)_OBJ_DIR_CURR)/%.o: $($(ID)_SRC_DIR_CURR)/%.cpp $($(ID)_HEADER_FILES)
	OBJ=$(subst $($(ID)_OBJ_DIR_CURR)/,,$@);									\
	echo "$(COLOR_RUN)    [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "		\
		 "Compiling: $$OBJ...$(COLOR_RESET)";									\
	$(call gpp_compile) -o '$@' '$<';											\
	STATUS=$$?;																	\
	if [ "X$$STATUS" != 'X0' ]; then 											\
		echo '$(COLOR_FAIL)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] '	\
			 "Object file $$OBJ building failed"								\
			 "(status: $$STATUS).$(COLOR_RESET)";								\
		rm '$@' 2>/dev/null || true;											\
	fi

# $(warning $($(ID)_TARGET_FILE): $($(ID)_HEADER_FILES) $($(ID)_OBJ_FILES) $($(ID)_MAIN_OBJ_FILES) $(call get_lib_files,$($(ID)_MODULE_DEPS)))
# Target executable linking
$($(ID)_TARGET_FILE): ID := $(ID)
$($(ID)_TARGET_FILE): $($(ID)_HEADER_FILES) $($(ID)_OBJ_FILES) $($(ID)_MAIN_OBJ_FILES) $(call get_lib_files,$($(ID)_MODULE_DEPS))
	# echo "$(COLOR_PASS)Target $(ID):$(COLOR_RESET)"
	# echo "    TARGET_FILE     = $($(ID)_TARGET_FILE)"
	# echo "    HEADER_FILES    = $($(ID)_HEADER_FILES)"
	# echo "    OBJ_FILES       = $($(ID)_OBJ_FILES)"
	# echo "    MAIN_OBJ_FILES  = $($(ID)_MAIN_OBJ_FILES)"
	# echo "    MODULE_DEPS     = $(call get_libs,$($(ID)_MODULE_DEPS))"
	# echo "    EXTERNAL_LIBS   = $(call get_external_libs,$($(ID)_EXTERNAL_LIBS))"
	
	echo "$(COLOR_RUN)    [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "		\
		 "Linking executable: $(subst $(BIN_DIR)/,,$@)...$(COLOR_RESET)";		\
	$(call gpp_link) -o '$@' $($(ID)_OBJ_FILES) $($(ID)_MAIN_OBJ_FILES);		\
	STATUS=$$?;																	\
	if [ "X$$STATUS" == 'X0' ]; then											\
		echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "	\
			 "Built successfully.$(COLOR_RESET)";								\
	else																		\
		echo "$(COLOR_FAIL)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "	\
			 "Building failed (status: $$STATUS).$(COLOR_RESET)";				\
	fi


$(warning $(ID)_TARGET_FILE     =  $($(ID)_TARGET_FILE))
$(warning $(ID)_TARGET_LIB_FILE =  $($(ID)_TARGET_LIB_FILE))
$(warning $(ID)_HEADER_FILES    =  $($(ID)_HEADER_FILES))
$(warning $(ID)_OBJ_FILES       =  $($(ID)_OBJ_FILES))
$(warning $(ID)_MODULE_DEPS     =  $(call get_lib_files,$($(ID)_MODULE_DEPS)))
$(warning ---)

# Target shared library linking
$($(ID)_TARGET_LIB_FILE): ID := $(ID)
$($(ID)_TARGET_LIB_FILE): $($(ID)_HEADER_FILES) $($(ID)_OBJ_FILES) $(call get_lib_files,$($(ID)_MODULE_DEPS))
	# echo "$(COLOR_PASS)Lib $(ID):$(COLOR_RESET)  $($(ID)_TARGET_LIB_FILE): $($(ID)_HEADER_FILES) $($(ID)_OBJ_FILES) $(call get_lib_files,$($(ID)_MODULE_DEPS))"
	# echo $^ | tr ' ' '\n' | grep '.*\.o'
	# echo
	# echo "$(COLOR_PASS)Lib $(ID):$(COLOR_RESET)"
	# echo "    TARGET_LIB_FILE = $($(ID)_TARGET_LIB_FILE)"
	# echo "    HEADER_FILES    = $($(ID)_HEADER_FILES)"
	# echo "    OBJ_FILES       = $($(ID)_OBJ_FILES)"
	# echo "    MODULE_DEPS     = $(call get_libs,$($(ID)_MODULE_DEPS))"
	# echo "    EXTERNAL_LIBS   = $(call get_external_libs,$($(ID)_EXTERNAL_LIBS))"
	
	echo "$(COLOR_RUN)    [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "		\
		 "Linking shared lib: $(subst $(LIB_DIR)/,,$@)...$(COLOR_RESET)";		\
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
# $($(ID)_TEST_DIR_CURR)/%: $($(ID)_OBJ_DIR_CURR)/%.o $($(ID)_HEADER_FILES) $($(ID)_TARGET_FILE)
	# TEST=$(subst $($(ID)_TEST_DIR_CURR)/,,$@);								\
	# echo "$(COLOR_RUN)    [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "		\
	# 	 "Linking test: $$TEST...$(COLOR_RESET)";								\
	# echo $(call gpp_link) -o '$@' '$<' $($(ID)_OBJ_FILES);					\
	# STATUS=$$?;																\
	# if [ "X$$STATUS" == 'X0' ]; then											\
	# 	echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "	\
	# 		 "Test built successfully.$(COLOR_RESET)";							\
	# else																		\
	# 	echo "$(COLOR_FAIL)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "	\
	# 		 "Test building failed (status: $$STATUS).$(COLOR_RESET)";			\
	# fi



# Run, install/uninstall targets
# ifeq ($($(ID)_TARGET_TYPE),Target)


# run: all
# 	echo "$(COLOR_RUN)[$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "						\
# 		 "Running...$(COLOR_RESET)";											\
# 	OLD_DIR=$$( pwd );															\
# 	cd '$(PROJECT_ROOT)' && ( $($(ID)_TARGET_FILE); cd "$$OLD_DIR" );					\
# 	STATUS=$$?;																	\
# 	if [ "X$$STATUS" == 'X0' ]; then											\
# 		echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "				\
# 			 "Done successfully.$(COLOR_RESET)";								\
# 	else																		\
# 		echo "$(COLOR_FAIL)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "				\
# 			 "Running failed (status: $$STATUS).$(COLOR_RESET)";				\
# 	fi


# install: all
# 	@echo "$(COLOR_RUN)    [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "					\
# 		  "Installing executable to "$(PREFIX_BIN)"...$(COLOR_RESET)"
# 	install '$($(ID)_TARGET_FILE)' '$(PREFIX_BIN)'
	
# 	@echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "					\
# 		  "Executable installed.$(COLOR_RESET)"


# uninstall:
# 	@echo "$(COLOR_RUN)    [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "					\
# 		  "Uninstalling executable from "$(PREFIX_BIN)"...$(COLOR_RESET)"
	
# 	rm '$(PREFIX_BIN)/$(notdir $($(ID)_TARGET_FILE))'
	
# 	@echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "					\
# 		  "Executable uninstalled.$(COLOR_RESET)"


# else ifeq ($($(ID)_TARGET_TYPE),Module)


# install: all
# 	@echo "$(COLOR_RUN)    [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "					\
# 		  "Installing shared lib to "$(PREFIX_LIB)"...$(COLOR_RESET)"
	
# 	install '$($(ID)_TARGET_LIB_FILE)' '$(PREFIX_LIB)'
	
# 	@echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "					\
# 		  "Shared lib installed.$(COLOR_RESET)"


# uninstall:
# 	@echo "$(COLOR_RUN)    [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "					\
# 		  "Uninstalling shared lib from "$(PREFIX_LIB)"...$(COLOR_RESET)"
	
# 	rm '$(PREFIX_LIB)/$(notdir $($(ID)_TARGET_LIB_FILE))'
	
# 	@echo "$(COLOR_PASS)==> [$($(ID)_TARGET_TYPE): $($(ID)_TARGET_NAME)] "					\
# 		  "Shared lib uninstalled.$(COLOR_RESET)"


# endif
