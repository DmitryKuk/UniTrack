# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This Makefile contains Mac OS X platform-dependent things.


export GPP					= clang++


# MacPorts installs boost and others into /opt/local
export GPP_HEADER_PATHS		+= -cxx-isystem/opt/local/include
export GPP_LIB_PATHS		+= -L/opt/local/lib

export OSX_VERSION			= $(shell sw_vers -productVersion)
export OSX_VERSION_MIN		= $(shell echo $(OSX_VERSION) | cut -d. -f1,2)
