# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for MongoDB C++ driver third-party.

export SHELL				= /bin/bash


MONGO_CXX_DRIVER_PATH		= mongo-cxx-driver
export GPP_LIBS				+= -lmongoclient


MONGO_BUILD_LOG_FILE		= $(LOGS_DIR)/mongo-cxx-driver_build_log.txt
MONGO_INSTALL_LOG_FILE		= $(LOGS_DIR)/mongo-cxx-driver_install_log.txt


MONGO_FLAGS					= -C "$(MONGO_CXX_DRIVER_PATH)" "-j$(JOBS)"		\
							  --sharedclient --c++11=on --release --dbg=off	\
							  --prefix="$(PREFIX_THIRDPARTY)"

ifeq ($(SYSTEM),Darwin)
	# MacPorts installs boost and others into /opt/local
	MONGO_FLAGS				+= --cpppath='/opt/local/include'
	MONGO_FLAGS				+= --libpath='/opt/local/lib'
	MONGO_FLAGS				+= --extrapath='/opt/local'
	
	MONGO_FLAGS				+= --osx-version-min=$(OSX_VERSION_MIN)
endif


.PHONY: all clean install update
.SILENT:


all:
	echo -e '$(COLOR_RUN)Building MongoDB C++ driver in $(JOBS) job(s)...$(COLOR_RESET)';		\
	scons $(MONGO_FLAGS) >'$(MONGO_BUILD_LOG_FILE)' 2>&1;										\
	STATUS=$$?;																					\
	if [ "X$$STATUS" = 'X0' ]; then																\
		echo -e '$(COLOR_PASS)==> MongoDB C++ driver built successfully.$(COLOR_RESET)';		\
		true;																					\
	else																						\
		echo -e "$(COLOR_FAIL)==> MongoDB C++ driver building failed (status: $$STATUS).";		\
		echo -e '(See $(MONGO_BUILD_LOG_FILE) for details)$(COLOR_RESET)';						\
		false;																					\
	fi


clean:
	rm -rf '$(MONGO_CXX_DRIVER_PATH)/build' 2>/dev/null || true


install:
	DIR=$(PREFIX_THIRDPARTY);																	\
	echo -e "$(COLOR_RUN)Installing MongoDB C++ driver to \"$$DIR\"...$(COLOR_RESET)";			\
	mkdir -p "$$DIR";																			\
	scons $(MONGO_FLAGS) install >'$(MONGO_INSTALL_LOG_FILE)' 2>&1;								\
	STATUS=$$?;																					\
	if [ "X$$STATUS" = 'X0' ]; then																\
		echo -e '$(COLOR_PASS)==> MongoDB C++ driver installed successfully.$(COLOR_RESET)';	\
	else																						\
		echo -e "$(COLOR_FAIL)==> MongoDB C++ driver installation failed (status: $$STATUS).";	\
		echo -e '(See $(MONGO_INSTALL_LOG_FILE) for details)$(COLOR_RESET)';					\
	fi


update:
	cd '$(MONGO_CXX_DRIVER_PATH)' && ( git submodule update --init; cd .. )
