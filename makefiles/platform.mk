# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This Makefile contains all info about compilers, system name and platform-dependent things.


# Target system name
SYSTEM						= $(shell uname -s)


# Use g++-5 because of c++14 features
GPP							= g++-5


# Compiler flags
GPP_COMPILE_FLAGS			= -pipe -O2 -Wall -std=c++14 -c $(EXTRA_CPP_FLAGS)

GPP_LINK_FLAGS				= -pipe -O2 -Wall $(EXTRA_LINK_FLAGS)
GPP_SHARED_LIB_FLAGS		= -pipe -O2 -Wall --shared $(EXTRA_SHARED_LIB_FLAGS)

# Flags to get headers dependencies
GPP_HEADER_DEPS_FLAGS		= -MM -MG -w -std=c++14


GPP_HEADER_PATHS			= 
GPP_LIB_PATHS				= 

GPP_LIBS					= 



ifeq ($(SYSTEM),Darwin)
	include $(MK_DIR)/platform_macosx.mk
else
	include $(MK_DIR)/platform_other.mk
endif
