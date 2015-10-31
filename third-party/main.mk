# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for third-party parts of this program.

include $(MAKEFILE_THIRDPARTY_CONFIG_ABS)


# Default jobs argument for building third-party modules
JOBS							= 4

LOGS_DIR						= log

MONGO_BUILD_LOG_FILE			= $(LOGS_DIR)/mongo-cxx-driver_build_log.txt
MONGO_INSTALL_LOG_FILE			= $(LOGS_DIR)/mongo-cxx-driver_install_log.txt


# Project library dir
LIBS_DIR_CURR					= $(LIBS_DIR_ABS)


MONGO_FLAGS						= -C "$(MONGO_CXX_DRIVER_PATH)" "-j$(JOBS)"		\
								  --sharedclient --c++11=on --release --dbg=off	\
								  --prefix="$(PREFIX_THIRDPARTY)"

ifeq ($(SYSTEM),Darwin)
	# MacPorts installs boost and others into /opt/local
	MONGO_FLAGS					+= --cpppath='/opt/local/include'
	MONGO_FLAGS					+= --libpath='/opt/local/lib'
	MONGO_FLAGS					+= --extrapath='/opt/local'
	
	MONGO_FLAGS					+= --osx-version-min=$(OSX_VERSION_MIN)
endif


.PHONY: all clean update
.SILENT:


all:
	# Downloading submodules, if them was not cloned
	git submodule update --init
	
	
	# Dir for logs of thirdparty modules building and installing
	mkdir -p $(LOGS_DIR)
	
	
	# json: nothing to do
	
	
	# Mongo C++ driver building
	echo '$(COLOR_RUN)Building MongoDB C++ driver (in $(JOBS) job(s))...$(COLOR_RESET)';	\
	scons $(MONGO_FLAGS) >$(MONGO_BUILD_LOG_FILE) 2>&1;										\
	STATUS=$$?;																				\
	if [ "X$$STATUS" == "X0" ]; then														\
		echo '$(COLOR_PASS)==> MongoDB C++ driver built successfully.$(COLOR_RESET)';		\
	else																					\
		echo '$(COLOR_FAIL)==> MongoDB C++ driver building failed.'							\
			 '(See $(abspath $(MONGO_BUILD_LOG_FILE)) for details)$(COLOR_RESET)';			\
	fi;


install:
	@echo "$(COLOR_RUN)Installing third-party files to \"$(PREFIX_THIRDPARTY)\"...$(COLOR_RESET)"
	
	mkdir -p "$(PREFIX_THIRDPARTY)"/{include,lib}
	
	
	STATUS_GLOBAL=0;																		\
	echo '$(COLOR_RUN)Installing Nlohmann JSON...$(COLOR_RESET)';							\
	install "$(JSON_PATH)/src/json.hpp" "$(PREFIX_THIRDPARTY)/include";						\
	STATUS=$$?;																				\
	if [ "X$$STATUS" == "X0" ]; then														\
		echo '$(COLOR_PASS)==> Nlohmann JSON installed successfully.$(COLOR_RESET)';		\
	else																					\
		STATUS_GLOBAL=1;																	\
		echo '$(COLOR_FAIL)==> Nlohmann JSON installation failed.';							\
	fi;																						\
																							\
																							\
	echo '$(COLOR_RUN)Installing MongoDB C++ driver...$(COLOR_RESET)';						\
	scons $(MONGO_FLAGS) install >$(MONGO_INSTALL_LOG_FILE) 2>&1;							\
	STATUS=$$?;																				\
	if [ "X$$STATUS" == "X0" ]; then														\
		echo '$(COLOR_PASS)==> MongoDB C++ driver installed successfully.$(COLOR_RESET)';	\
	else																					\
		STATUS_GLOBAL=1;																	\
		echo '$(COLOR_FAIL)==> MongoDB C++ driver installation failed.'						\
			 '(See $(abspath $(MONGO_INSTALL_LOG_FILE)) for details)$(COLOR_RESET)';		\
	fi;																						\
																							\
																							\
	if [ "X$$STATUS_GLOBAL" == "X0" ]; then													\
		echo "$(COLOR_PASS)==> Third-party files installed.$(COLOR_RESET)";					\
	else																					\
		echo '$(COLOR_FAIL)==> Third-party files installation failed.';						\
	fi


uninstall:
	rm -rf $(PREFIX_THIRDPARTY) 2>/dev/null || true
	@echo "$(COLOR_PASS)==> Third-party files removed.$(COLOR_RESET)"


clean:
	rm -rf "$(MONGO_CXX_DRIVER_PATH)/build" 2>/dev/null 2>/dev/null || true


update:
	cd JSON_PATH && git submodule update --init && cd ..
	cd MONGO_CXX_DRIVER_PATH && git submodule update --init && cd ..
