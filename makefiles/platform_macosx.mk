# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This Makefile contains Mac OS X platform-dependent things.


export GPP					= clang++


# MacPorts installs boost and others into /opt/local
export GPP_HEADER_PATHS		+= -I/opt/local/include
export GPP_LIB_PATHS		+= -L/opt/local/lib

export OSX_VERSION			= $(shell sw_vers -productVersion)
export OSX_VERSION_MIN		= $(shell echo $(OSX_VERSION) | cut -d. -f1,2)


# Disable warnings in boost for new version of Clang
# export GPP_COMPILE_FLAGS	+= -Wno-unused-local-typedef
