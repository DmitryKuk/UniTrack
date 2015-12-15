# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile with config for logic module.


# Other modules this module depends on
MODULE_DEPS		 			= base logger templatizer server


# External libs this module depends on
ifeq ($(SYSTEM),Darwin)
	EXTERNAL_LIBS			= boost_system-mt boost_filesystem-mt
else
	EXTERNAL_LIBS			= boost_system pthread boost_filesystem
endif
EXTERNAL_LIBS				+= mongoclient cryptopp


# Disable warnings in Boost.Asio for new version of Clang and mongo driver
COMPILE_FLAGS				= -Wno-unused-local-typedef -Wno-deprecated-declarations
