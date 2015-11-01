# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This is the root Makefile of project.


export PROJECT_ROOT			= $(shell pwd)
export MK_DIR_ABS			= $(PROJECT_ROOT)/makefiles


include $(MK_DIR_ABS)/colors.mk
include $(MK_DIR_ABS)/config.mk
include $(MK_DIR_ABS)/config_internal.mk
include $(MK_DIR_ABS)/platform.mk

# Helper functions
export MK_UTILITY_ABS 		= $(MK_DIR_ABS)/utility.mk
include $(MK_UTILITY_ABS)

# Common targets for modules and targets Makefiles
export MK_TARGETS_ABS		= $(MK_DIR_ABS)/targets.mk

# Recursive targets' and modules' targets
export MK_RECURSIVE_ABS		= $(MK_DIR_ABS)/recursive.mk
include $(MK_RECURSIVE_ABS)


# Current module pathes
SRC_DIR_CURR				= $(SRC_DIR_ABS)

BUILD_DIR_CURR				= $(BUILD_DIR_ABS)
BIN_DIR_CURR				= $(BIN_DIR_ABS)
LIB_DIR_CURR				= $(LIB_DIR_ABS)
OBJ_DIR_CURR				= $(OBJ_DIR_ABS)

TEST_DIR_CURR				= $(TEST_DIR_ABS)

CONFIG_DIR_CURR				= $(CONFIG_DIR_ABS)
WWW_DIR_CURR				= $(WWW_DIR_ABS)

THIRDPARTY_DIR_CURR			= $(THIRDPARTY_DIR_ABS)

MODULES_SRC_DIR_CURR		= $(MODULES_SRC_DIR_ABS)
MODULES_OBJ_DIR_CURR		= $(MODULES_OBJ_DIR_ABS)
MODULES_TEST_DIR_CURR		= $(MODULES_TEST_DIR_ABS)

TARGETS_SRC_DIR_CURR		= $(TARGETS_SRC_DIR_ABS)
TARGETS_OBJ_DIR_CURR		= $(TARGETS_OBJ_DIR_ABS)
TARGETS_TEST_DIR_CURR		= $(TARGETS_TEST_DIR_ABS)


export GPP_HEADER_PATHS		+= -I'$(PREFIX_THIRDPARTY)/include' -I'$(MODULES_SRC_DIR_CURR)'
export GPP_LIB_PATHS		+= -L'$(PREFIX_THIRDPARTY)/lib' -L'$(LIB_DIR_CURR)'
export GPP_COMPILE_FLAGS	+= $(GPP_PROJECT_DATA)


export MODULES				= $(notdir $(shell find '$(MODULES_SRC_DIR_CURR)/' -maxdepth 1 -type d))
export TARGETS				= $(notdir $(shell find '$(TARGETS_SRC_DIR_CURR)/' -maxdepth 1 -type d))


# Targets
.PHONY:																				\
	all          targets        modules        tests        third-party 			\
	clean  clean-targets  clean-modules  clean-tests  clean-third-party  distclean	\
	


.SILENT:



# Building
all: modules targets


targets: $(call get_bin_files,$(TARGETS))


modules: $(call get_lib_files,$(MODULES))


tests:
	$(call for_each_target,tests)
	$(call for_each_module,tests)


third-party:
	$(MAKE) -C "$(THIRDPARTY_DIR_CURR)"



# Cleaning
# Cleaning project targets and modules (not third-party!)
clean: clean-targets clean-modules


clean-targets:
	$(call for_each_target,clean)


clean-modules:
	$(call for_each_module,clean)


clean-tests:
	$(call for_each_target,clean-tests)
	$(call for_each_module,clean-tests)


clean-third-party:
	$(MAKE) -C "$(THIRDPARTY_DIR_CURR)" clean


# Cleaning all built files (submodules and third-party too!)
distclean:
	rm -rf $(BUILD_DIR_CURR)



# Installing
# install-third-party:
# 	$(MAKE) -C "$(THIRDPARTY_DIR_CURR)" install


# install-bin:
# 	@echo "$(COLOR_RUN)Installing files to \"$(PREFIX_LIB)\"...$(COLOR_RESET)"
# 	install $(MODULE_FILES_ABS) $(PREFIX_LIB)
	
# 	@echo "$(COLOR_RUN)Installing files to \"$(PREFIX_TARGET)\"...$(COLOR_RESET)"
# 	install $(TARGET_FILE) $(PREFIX_TARGET)
	
# 	@echo "$(COLOR_PASS)==> Binaries installed.$(COLOR_RESET)"


# install-config:
# 	@echo "$(COLOR_RUN)Creating directories in \"$(PREFIX_CONFIG_FULL)\"...$(COLOR_RESET)"
# 	find $(CONFIG) -type d -not -name '.*' | while read DIR; do install -d "$(PREFIX_CONFIG_FULL)/$$DIR"; done
	
# 	@echo "$(COLOR_RUN)Installing files to \"$(PREFIX_CONFIG_FULL)\"...$(COLOR_RESET)"
# 	find $(CONFIG) -type f -not -name '.*' | while read FILE; do install "$$FILE" "$(PREFIX_CONFIG_FULL)/$$FILE"; done
	
# 	@echo "$(COLOR_PASS)==> Config installed.$(COLOR_RESET)"


# install-www:
# 	@echo "$(COLOR_RUN)Creating directories in \"$(PREFIX_WWW_FULL)\"...$(COLOR_RESET)"
# 	find $(WWW) -type d -not -name '.*' | while read DIR; do install -d "$(PREFIX_WWW_FULL)/$$DIR"; done
	
# 	@echo "$(COLOR_RUN)Installing files to \"$(PREFIX_WWW_FULL)\"...$(COLOR_RESET)"
# 	find $(WWW) -type f -not -name '.*' | while read FILE; do install "$$FILE" "$(PREFIX_WWW_FULL)/$$FILE"; done
	
# 	@echo "$(COLOR_PASS)==> WWW data installed.$(COLOR_RESET)"


# install: install-bin install-config install-www


# uninstall-third-party:
# 	$(MAKE) -C "$(THIRDPARTY_DIR_CURR)" uninstall


# uninstall-bin:
# 	rm $(addprefix $(PREFIX_TARGET)/,$(MAIN_TARGET)) 2>/dev/null || true
# 	rm $(addprefix $(PREFIX_LIB)/,$(MODULE_FILES) $(THIRDPARTY_FILES)) 2>/dev/null || true
# 	@echo "$(COLOR_PASS)==> Binaries removed.$(COLOR_RESET)"


# uninstall-config:
# 	rm -r $(PREFIX_CONFIG_FULL) 2>/dev/null || true
# 	@echo "$(COLOR_PASS)==> Config removed.$(COLOR_RESET)"


# uninstall-www:
# 	rm -r $(PREFIX_WWW_FULL) 2>/dev/null || true
# 	@echo "$(COLOR_PASS)==> WWW data removed.$(COLOR_RESET)"


# uninstall: uninstall-bin


# uninstall-all: uninstall uninstall-third-party uninstall-config uninstall-www


# one-step-make:
# 	# Third-party
# 	$(MAKE) third-party
	
# 	@echo "$(COLOR_RUN)Please, enter the password (if need) for installation of third-party " \
# 		  "modules or press Ctrl+C...$(COLOR_RESET)"
# 	sudo make install-third-party
	
	
# 	# The main program
# 	$(MAKE)


# upgrade:
# 	# Uninstallation
# 	@echo "$(COLOR_RUN)Please, enter the password (if need) for uninstallation " \
# 		  "of the old version or press Ctrl+C...$(COLOR_RESET)"
# 	sudo make uninstall-third-party uninstall-bin uninstall-www
	
	
# 	# Cleaning the built files
# 	$(MAKE) distclean
	
	
# 	# Downloading the new version
# 	$(MAKE) git-pull
	
	
# 	# Building and installation
# 	$(MAKE) one-step-make
	
# 	@echo "$(COLOR_RUN)Please, enter the password (if need) for installation " \
# 		  "or press Ctrl+C...$(COLOR_RESET)"
# 	sudo make install-bin install-www
	
	
# 	@echo "$(COLOR_PASS)==> Successfully upgraded.$(COLOR_RESET)"


# happy: git-pull
# 	$(MAKE) one-step-make
	
# 	@echo "$(COLOR_RUN)Please, enter the password (if need) for installation " \
# 		  "or press Ctrl+C...$(COLOR_RESET)"
# 	sudo make install
	
# 	@echo "$(COLOR_PASS)==> Successfully built and installed.$(COLOR_RESET)"
	
	
# 	@echo "$(COLOR_PASS)NOTE:$(COLOR_RESET) To work with $(PROJECT_NAME) try following commands:" \
# 		  "$(addprefix \n    ,$(MAIN_TARGET))"
	
# 	@echo "$(COLOR_PASS)NOTE:$(COLOR_RESET) Next times you can simply do:"
# 	@echo '    make upgrade'


# git-pull:
# 	echo '$(COLOR_RUN)Downloading new version...$(COLOR_RESET)';								\
# 	git pull --recurse-submodules=yes;															\
# 	STATUS=$$?;																					\
# 	if [ "X$$STATUS" = 'X0' ]; then																\
# 		echo '$(COLOR_PASS)==> New version downloaded.$(COLOR_RESET)';							\
# 	else																						\
# 		echo "$(COLOR_FAIL)==> Download failed with status: $$STATUS.$(COLOR_RESET)";			\
# 	fi


# # Tests targets
# check: run-tests


# dirs:
# 	mkdir -p $(BUILD_DIR_CURR) $(LIB_DIR_CURR) $(OBJ_DIR_CURR) $(TEST_DIR_CURR)


# objects:
# 	echo '$(COLOR_RUN)Building objects...$(COLOR_RESET)';										\
# 	$(MAKE) -C $(SRC_DIR_CURR);																\
# 	STATUS=$$?;																					\
# 	if [ "X$$STATUS" = 'X0' ]; then																\
# 		echo '$(COLOR_PASS)==> Objects built successfully.$(COLOR_RESET)';						\
# 	else																						\
# 		echo '$(COLOR_FAIL)==> Objects building failed.$(COLOR_RESET)';							\
# 	fi;


# modules: dirs
# 	for T in $(MODULES); do																		\
# 		echo "$(COLOR_RUN)Building module: $$T...$(COLOR_RESET)";								\
# 		$(MAKE) -C "$(call get_sources_files,$$T)" MODULE_NAME="$$T";							\
# 		STATUS=$$?;																				\
# 		if [ "X$$STATUS" = 'X0' ]; then															\
# 			echo "$(COLOR_PASS)==> Module $$T built successfully.$(COLOR_RESET)";				\
# 		else																					\
# 			echo "$(COLOR_FAIL)==> Module $$T building failed.$(COLOR_RESET)";					\
# 		fi;																						\
# 	done


# third-party:
# 	$(MAKE) -C "$(THIRDPARTY_DIR_CURR)"


# run: all
# 	$(MAKE) -C $(SRC_DIR_CURR) run


# # Building tests for submodules too
# tests: modules objects
# 	$(MAKE) -C "$(SRC_DIR_CURR)" tests;														\
# 	for T in $(MODULES); do																		\
# 		$(MAKE) -C "$(call get_sources_files,$$T)" MODULE_NAME="$$T" tests;						\
# 	done


# # Running tests for submodules too
# run-tests: dirs tests
# 	$(MAKE) -C "$(SRC_DIR_CURR)" run-tests;													\
# 	for T in $(MODULES); do																		\
# 		$(MAKE) -C "$(call get_sources_files,$$T)" MODULE_NAME="$$T" run-tests;					\
# 	done


# # Building of all module files
# $(MODULE_FILES_ABS): modules
