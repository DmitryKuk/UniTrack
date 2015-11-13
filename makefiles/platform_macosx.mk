# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This Makefile contains Mac OS X platform-dependent things.


GPP							= clang++


# MacPorts installs boost and others into /opt/local
GPP_HEADER_PATHS			+= -I/opt/local/include
GPP_LIB_PATHS				+= -L/opt/local/lib

OSX_VERSION					= $(shell sw_vers -productVersion)
OSX_VERSION_MIN				= $(shell echo $(OSX_VERSION) | cut -d. -f1,2)
