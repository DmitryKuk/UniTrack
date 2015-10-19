# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for third-party parts of this program.

include $(MAKEFILE_THIRDPARTY_CONFIG_ABS)

# Default jobs argument for building third-party modules
JOBS							= 4

MONGO_BUILD_LOG_FILE_ABS		= "$(MONGO_CXX_DRIVER_INSTALLED_ABS)/build_log.txt"


# Project library dir
LIBS_DIR_CURR					= $(LIBS_DIR_ABS)


MONGO_FLAGS						= -C "$(MONGO_CXX_DRIVER_PATH_ABS)"							\
								  --c++11=on --sharedclient --release --dbg=off "-j$(JOBS)"	\
								  --prefix="$(MONGO_CXX_DRIVER_INSTALLED_ABS)"

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
	# json: nothing to do
	
	# Mongo C++ driver building and customizing
	echo '$(COLOR_RUN)Building MongoDB C++ driver (in $(JOBS) job(s))...$(COLOR_RESET)';											\
	mkdir -p "$(MONGO_CXX_DRIVER_INSTALLED_ABS)";																					\
	scons $(MONGO_FLAGS) install >$(MONGO_BUILD_LOG_FILE_ABS) 2>&1 && (																\
		rm "$(MONGO_CXX_DRIVER_INSTALLED_ABS)/lib/libmongoclient.a" 2>/dev/null && (												\
			mv "$(MONGO_CXX_DRIVER_INSTALLED_ABS)/lib/libmongoclient.so" "$(LIBS_DIR_CURR)/libut_mongoclient.so" 2>/dev/null ||		\
			mv "$(MONGO_CXX_DRIVER_INSTALLED_ABS)/lib/libmongoclient.dylib" "$(LIBS_DIR_CURR)/libut_mongoclient.dylib" 2>/dev/null	\
		)																															\
	);																																\
	STATUS=$$?;																														\
	if [ "X$$STATUS" == "X0" ]; then																								\
		echo '$(COLOR_PASS)==> MongoDB C++ driver built successfully.$(COLOR_RESET)';												\
	else																															\
		echo '$(COLOR_FAIL)==> MongoDB C++ driver building failed.'																	\
			 '(See $(abspath $(MONGO_BUILD_LOG_FILE_ABS)) for details)$(COLOR_RESET)';												\
	fi;


clean:
	rm -rf "$(MONGO_CXX_DRIVER_INSTALLED_ABS)" "$(MONGO_CXX_DRIVER_PATH_ABS)/build" 2>/dev/null
	find "$(LIBS_DIR_CURR)" -d 1 -type f -name 'libut_mongoclient.*' -exec rm {} \;


update:
	cd JSON_PATH && git submodule update --init && cd ..
	cd MONGO_CXX_DRIVER_PATH && git submodule update --init && cd ..
