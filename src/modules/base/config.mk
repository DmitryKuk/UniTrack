# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile with config for base part of this program.


# Other modules this module depends on
export MODULE_DEPS	 		= 


# External libs this module depends on
ifeq ($(SYSTEM),Darwin)
	EXTERNAL_LIBS			= boost_system-mt boost_filesystem-mt
else
	EXTERNAL_LIBS			= boost_system pthread boost_filesystem
endif
export EXTERNAL_LIBS