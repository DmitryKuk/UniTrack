# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile with config for logic module.


# Other modules this module depends on
MODULE_DEPS		 			= logger templatizer


# External libs this module depends on
ifeq ($(SYSTEM),Darwin)
	EXTERNAL_LIBS			= boost_system-mt boost_filesystem-mt
else
	EXTERNAL_LIBS			= boost_system pthread boost_filesystem
endif
EXTERNAL_LIBS				+= mongoclient


# This need because of mongo legacy driver
COMPILE_FLAGS				= -Wno-deprecated-declarations
