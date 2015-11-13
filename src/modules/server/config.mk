# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile with config for server module.


# Other modules this module depends on
MODULE_DEPS			 		= base logger


# External libs this module depends on
ifeq ($(SYSTEM),Darwin)
	EXTERNAL_LIBS			= boost_system-mt boost_filesystem-mt
else
	EXTERNAL_LIBS			= boost_system pthread boost_filesystem
endif


# Disable warnings in Boost.Asio for new version of Clang
COMPILE_FLAGS				= -Wno-unused-local-typedef
