# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile with config for logger module.


# Other modules this module depends on
MODULE_DEPS			 		= 


# External libs this module depends on
ifeq ($(SYSTEM),Darwin)
	EXTERNAL_LIBS			= boost_system-mt boost_filesystem-mt
else
	EXTERNAL_LIBS			= boost_system pthread boost_filesystem
endif
