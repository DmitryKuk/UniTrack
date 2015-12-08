# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This is the root Makefile of project.


# Helper function. Returns name of current makefile
where-am-i					= $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))


# Directory with makefiles
MK_DIR						= makefiles


include $(MK_DIR)/colors.mk
include $(MK_DIR)/config_internal.mk
include $(MK_DIR)/config.mk
include $(MK_DIR)/platform.mk

# Helper functions
MK_UTILITY	 				= $(MK_DIR)/utility.mk
include $(MK_UTILITY)

# Common targets for modules and targets Makefiles
MK_TARGETS					= $(MK_DIR)/targets.mk


# ---===           Project macros            ===---

GPP_PROJECT_DATA			+= -DPATH_CONFIG="\"$(PREFIX_CONFIG)/$(CONFIG_DIR)\""

# ---===        End of project macros        ===---



GPP_HEADER_PATHS			+= -I'$(PREFIX_THIRDPARTY)/include' -I'$(MODULES_SRC_DIR)'
GPP_LIB_PATHS				+= -L'$(PREFIX_THIRDPARTY)/lib' -L'$(LIB_DIR)'
GPP_COMPILE_FLAGS			+= $(GPP_PROJECT_DATA)


MODULES						= $(notdir $(shell find '$(MODULES_SRC_DIR)' -maxdepth 1 -mindepth 1 -type d))
TARGETS						= $(notdir $(shell find '$(TARGETS_SRC_DIR)' -maxdepth 1 -mindepth 1 -type d))


# Global lists of test targets
TEST_TARGET_FILES			= 


# Targets for building all
CHILD_MAKEFILES				= $(shell find "$(SRC_DIR)" -name 'Makefile')
include $(CHILD_MAKEFILES)


# Targets
.PHONY:																					\
	all  dirs  targets  modules  tests  third-party										\
	clean  clean-third-party  distclean													\
																						\
	install      install-bin    install-third-party    install-config    install-www	\
	uninstall  uninstall-bin  uninstall-third-party  uninstall-config  uninstall-www	\
	uninstall-all																		\
																						\
	run  check  run-tests																\
																						\
	one-step-make  upgrade  happy  git-pull  lines


.SILENT:


.DEFAULT_GOAL				:= all


# Building
all: modules targets


dirs:
	mkdir -p "$(BUILD_DIR)" "$(BIN_DIR)" "$(LIB_DIR)" "$(TEST_DIR)" &&						\
	FIFO_NAME="$(BUILD_DIR)/dirs_fifo" &&													\
	mkfifo "$$FIFO_NAME" && (																\
		find "$(SRC_DIR)" -type d | tee "$$FIFO_NAME" | sed 's,^$(SRC_DIR),$(OBJ_DIR),' &	\
		sed 's,^$(SRC_DIR),$(TEST_DIR),' "$$FIFO_NAME"										\
	) |																						\
	xargs -n 1 mkdir -p;																	\
	rm "$$FIFO_NAME"


targets: dirs $(call get_bin_files,$(TARGETS))


modules: dirs $(call get_lib_files,$(MODULES))


tests: dirs $(TEST_TARGET_FILES)


third-party:
	$(MAKE) -C '$(THIRDPARTY_DIR)'



# Cleaning
# Cleaning project targets and modules (not third-party!)
clean:
	rm -rf '$(BUILD_DIR)'


clean-third-party:
	$(MAKE) -C '$(THIRDPARTY_DIR)' clean


# Cleaning all built files (submodules and third-party too!)
distclean: clean clean-third-party



# Installing
install: install-bin install-config install-www


install-bin:
	for T in $(TARGETS); do															\
		echo "$(COLOR_RUN)Installing: $(PREFIX_BIN)/$$T...$(COLOR_RESET)";			\
		install "$(BIN_DIR)/$$T" '$(PREFIX_BIN)';									\
	done
	
	for T in $(MODULES); do															\
		echo "$(COLOR_RUN)Installing: $(PREFIX_LIB)/lib$$T.so...$(COLOR_RESET)";	\
		install "$(LIB_DIR)/lib$(PROJECT_LIB_PREFIX)$$T.so" '$(PREFIX_BIN)';		\
	done
	
	@echo "$(COLOR_RUN)==> Executables and shared libs installed.$(COLOR_RESET)"


install-third-party:
	$(MAKE) -C '$(THIRDPARTY_DIR)' install


install-config:
	@echo "$(COLOR_RUN)Creating directories in \"$(PREFIX_CONFIG)\"...$(COLOR_RESET)"
	find '$(CONFIG_DIR)' -type d -not -name '.*' |									\
	while read DIR; do install -d "$(PREFIX_CONFIG)/$$DIR"; done
	
	@echo "$(COLOR_RUN)Installing WWW files to \"$(PREFIX_CONFIG)\"...$(COLOR_RESET)"
	find '$(CONFIG_DIR)' -type f -not -name '.*' |									\
	while read FILE; do install "$$FILE" "$(PREFIX_CONFIG)/$$FILE"; done
	
	@echo "$(COLOR_PASS)==> Config installed.$(COLOR_RESET)"


install-www:
	@echo "$(COLOR_RUN)Creating directories in \"$(PREFIX_WWW)\"...$(COLOR_RESET)"
	find '$(WWW_DIR)' -type d -not -name '.*' |										\
	while read DIR; do install -d "$(PREFIX_WWW)/$$DIR"; done
	
	@echo "$(COLOR_RUN)Installing WWW files to \"$(PREFIX_WWW)\"...$(COLOR_RESET)"
	find '$(WWW_DIR)' -type f -not -name '.*' |										\
	while read FILE; do install "$$FILE" "$(PREFIX_WWW)/$$FILE"; done
	
	@echo "$(COLOR_PASS)==> WWW data installed.$(COLOR_RESET)"


# install-log:
# 	@echo "$(COLOR_RUN)Creating directory: \"$(PREFIX_LOG)\"...$(COLOR_RESET)"
# 	install -d '$(PREFIX_LOG)'
# 	@echo "$(COLOR_PASS)==> Log directory installed.$(COLOR_RESET)"



# Uninstalling
uninstall: uninstall-bin uninstall-www


uninstall-bin:
	rm $(addprefix $(PREFIX_BIN)/,$(TARGETS))
	rm $(addprefix $(PREFIX_LIB)/lib$(PROJECT_LIB_PREFIX),$(addsuffix .so,$(MODULES)))
	@echo "$(COLOR_PASS)==> Executables and shared libs removed.$(COLOR_RESET)"


uninstall-third-party:
	$(MAKE) -C '$(THIRDPARTY_DIR)' uninstall


uninstall-config:
	rm -r '$(PREFIX_CONFIG)'
	@echo "$(COLOR_PASS)==> Config removed.$(COLOR_RESET)"


uninstall-www:
	rm -r '$(PREFIX_WWW)'
	@echo "$(COLOR_PASS)==> WWW data removed.$(COLOR_RESET)"


# uninstall-log:
# 	rm -r '$(PREFIX_LOG)'
# 	@echo "$(COLOR_PASS)==> Log directory removed.$(COLOR_RESET)"



uninstall-all: uninstall-bin uninstall-www uninstall-third-party uninstall-config



# Running
run: targets
	$(def_print_status_table);														\
	NUM_SUCCESS=0; NUM_FAIL=0;														\
	for T in $(TARGETS); do															\
		echo "$(COLOR_RUN)Running: $$T...$(COLOR_RESET)";							\
		"./$(BIN_DIR)/$$T";															\
		STATUS=$$?;																	\
		if [ "X$$STATUS" = 'X0' ]; then												\
			echo "$(COLOR_PASS)==> Done successfully: $$T.$(COLOR_RESET)";			\
			let ++NUM_SUCCESS;														\
		else																		\
			echo "$(COLOR_FAIL)==> Running failed: $$T"								\
				 "(status: $$STATUS).$(COLOR_RESET)";								\
			let ++NUM_FAIL;															\
		fi;																			\
	done;																			\
	print_status_table



# Tests targets
check: run-tests


run-tests: tests
	$(def_print_status_table);														\
	NUM_SUCCESS=0; NUM_FAIL=0;														\
	for T in $(TEST_TARGET_FILES); do												\
		TNAME=$$( echo "$$T" | sed 's,^$(TEST_DIR)/,,' );							\
		echo "$(COLOR_RUN)Running: $$TNAME...$(COLOR_RESET)";						\
		"./$$T";																	\
		STATUS=$$?;																	\
		if [ "X$$STATUS" = 'X0' ]; then												\
			let ++NUM_SUCCESS;														\
		else																		\
			echo "$(COLOR_FAIL)==> Test failed: $$TNAME"							\
				 "(status: $$STATUS).$(COLOR_RESET)";								\
			let ++NUM_FAIL;															\
		fi;																			\
	done;																			\
	print_status_table



# Other
one-step-make:
	# Third-party
	$(MAKE) third-party
	
	@echo "$(COLOR_RUN)Please, enter the password (if need) for installation of third-party " \
		  "modules or press Ctrl+C...$(COLOR_RESET)"
	sudo make install-third-party
	
	
	# The main program
	$(MAKE)


upgrade:
	# Uninstallation
	@echo "$(COLOR_RUN)Please, enter the password (if need) for uninstallation " \
		  "of the old version or press Ctrl+C...$(COLOR_RESET)"
	sudo make uninstall-bin uninstall-www uninstall-third-party
	
	
	# Cleaning the built files
	$(MAKE) distclean clean-third-party
	
	
	# Downloading the new version
	$(MAKE) git-pull
	
	
	# Building and installation
	$(MAKE) one-step-make
	
	@echo "$(COLOR_RUN)Please, enter the password (if need) for installation " \
		  "or press Ctrl+C...$(COLOR_RESET)"
	sudo make install-bin install-www
	
	
	@echo "$(COLOR_PASS)==> Successfully upgraded.$(COLOR_RESET)"


happy: git-pull
	$(MAKE) one-step-make
	
	@echo "$(COLOR_RUN)Please, enter the password (if need) for installation " \
		  "or press Ctrl+C...$(COLOR_RESET)"
	sudo make install
	
	@echo "$(COLOR_PASS)==> Successfully built and installed.$(COLOR_RESET)"
	
	
	@echo "$(COLOR_PASS)NOTE:$(COLOR_RESET) To work with $(PROJECT_NAME) try following commands:" \
		  "$(addprefix \n    ,$(MAIN_TARGET))"
	
	@echo "$(COLOR_PASS)NOTE:$(COLOR_RESET) Next times you can simply do:"
	@echo '    make upgrade'


git-pull:
	echo '$(COLOR_RUN)Downloading new version...$(COLOR_RESET)';								\
	git pull --recurse-submodules=yes;															\
	STATUS=$$?;																					\
	if [ "X$$STATUS" = 'X0' ]; then																\
		echo '$(COLOR_PASS)==> New version downloaded.$(COLOR_RESET)';							\
	else																						\
		echo "$(COLOR_FAIL)==> Download failed with status: $$STATUS.$(COLOR_RESET)";			\
	fi


lines:
	function lines_count() {																	\
		( find -E $$2 $$3 -type f -regex "$$1" -exec grep -ch '^' {} \; |						\
	 	  tr "\n" '+' && echo '0' ) | bc;														\
	};																							\
	echo "Total lines of all C++ sources: $$( lines_count '.*\.(cpp|h|hpp)' $(SRC_DIR) )";		\
	echo "Total lines of all Makefiles:   $$( lines_count '.*\.mk' $(MK_DIR) $(SRC_DIR) )"
