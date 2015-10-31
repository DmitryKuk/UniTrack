# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This Makefile contains all info about compilers, system name and platform-dependent things.


# Target system name
export SYSTEM				= $(shell uname -s)


# Use g++-5 because of c++14 features
export GPP					= g++-5


# Compiler flags
export GPP_COMPILE_FLAGS	= -pipe -O2 -Wall -std=c++14 -c $(EXTRA_CPP_FLAGS)


export GPP_LINK_FLAGS		= -pipe -O2 -Wall $(EXTRA_LINK_FLAGS)
export GPP_SHARED_LIB_FLAGS	= -pipe -O2 -Wall --shared $(EXTRA_SHARED_LIB_FLAGS)


export GPP_HEADER_PATHS		= 
export GPP_LIB_PATHS		= 

export GPP_LIBS				= 



ifeq ($(SYSTEM),Darwin)
	include $(MAKEFILE_DIR_ABS)/platform_macosx.mk
else
	include $(MAKEFILE_DIR_ABS)/platform_other.mk
endif
