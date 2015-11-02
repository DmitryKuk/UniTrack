# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This is the root Makefile of project.


export PROJECT_ROOT			= $(shell pwd)
export MK_DIR_ABS			= $(PROJECT_ROOT)/makefiles


include $(MK_DIR_ABS)/colors.mk
include $(MK_DIR_ABS)/config_internal.mk
include $(MK_DIR_ABS)/config.mk
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



# ---===           Project macros            ===---

GPP_PROJECT_DATA			+= -DPATH_CONFIG="\"$(PREFIX_CONFIG)/$(CONFIG_DIR)\""	\
							   -DPATH_WWW="\"$(PREFIX_WWW)/$(WWW_DIR)\""

# ---===        End of project macros        ===---



export GPP_HEADER_PATHS		+= -I'$(PREFIX_THIRDPARTY)/include' -I'$(MODULES_SRC_DIR_CURR)'
export GPP_LIB_PATHS		+= -L'$(PREFIX_THIRDPARTY)/lib' -L'$(LIB_DIR_CURR)'
export GPP_COMPILE_FLAGS	+= $(GPP_PROJECT_DATA)


export MODULES				= $(notdir $(shell find '$(MODULES_SRC_DIR_CURR)' -depth 1 -type d))
export TARGETS				= $(notdir $(shell find '$(TARGETS_SRC_DIR_CURR)' -depth 1 -type d))


# Targets
.PHONY:																					\
	all          targets        modules        tests        third-party 				\
	clean  clean-targets  clean-modules  clean-tests  clean-third-party  distclean		\
																						\
	install      install-bin    install-third-party    install-config    install-www	\
	uninstall  uninstall-bin  uninstall-third-party  uninstall-config  uninstall-www	\
	uninstall-all																		\
																						\
	run  check  run-tests																\
																						\
	one-step-make  upgrade  happy  git-pull


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
install: install-bin install-config install-www


install-bin:
	$(call for_each_target,install)
	$(call for_each_module,install)


install-third-party:
	$(MAKE) -C "$(THIRDPARTY_DIR_CURR)" install


install-config:
	@echo "$(COLOR_RUN)[Global]  Creating directories in \"$(PREFIX_CONFIG)\"...$(COLOR_RESET)"
	find '$(CONFIG_DIR)' -type d -not -name '.*' |												\
	while read DIR; do install -d "$(PREFIX_CONFIG)/$$DIR"; done
	
	@echo "$(COLOR_RUN)[Global]  Installing WWW files to \"$(PREFIX_CONFIG)\"...$(COLOR_RESET)"
	find '$(CONFIG_DIR)' -type f -not -name '.*' |												\
	while read FILE; do install "$$FILE" "$(PREFIX_CONFIG)/$$FILE"; done
	
	@echo "$(COLOR_PASS)==> [Global]  Config installed.$(COLOR_RESET)"


install-www:
	@echo "$(COLOR_RUN)[Global]  Creating directories in \"$(PREFIX_WWW)\"...$(COLOR_RESET)"
	find '$(WWW_DIR)' -type d -not -name '.*' |													\
	while read DIR; do install -d "$(PREFIX_WWW)/$$DIR"; done
	
	@echo "$(COLOR_RUN)[Global]  Installing WWW files to \"$(PREFIX_WWW)\"...$(COLOR_RESET)"
	find '$(WWW_DIR)' -type f -not -name '.*' |													\
	while read FILE; do install "$$FILE" "$(PREFIX_WWW)/$$FILE"; done
	
	@echo "$(COLOR_PASS)==> [Global]  WWW data installed.$(COLOR_RESET)"



# Uninstalling
uninstall: uninstall-bin uninstall-www


uninstall-bin:
	$(call for_each_target,uninstall)
	$(call for_each_module,uninstall)


uninstall-third-party:
	$(MAKE) -C "$(THIRDPARTY_DIR_CURR)" uninstall


uninstall-config:
	rm -r '$(PREFIX_CONFIG)' 2>/dev/null || true
	@echo "$(COLOR_PASS)==> [Global]  Config removed.$(COLOR_RESET)"


uninstall-www:
	rm -r '$(PREFIX_WWW)' 2>/dev/null || true
	@echo "$(COLOR_PASS)==> [Global]  WWW data removed.$(COLOR_RESET)"



uninstall-all: uninstall-bin uninstall-www uninstall-third-party uninstall-config



# Running
run: all
	$(call for_each_target,run)


# Tests targets
check: run-tests


run-tests:
	$(call for_each_target,run-tests)
	$(call for_each_module,run-tests)



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
