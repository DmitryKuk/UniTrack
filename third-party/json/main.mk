# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for third-party parts of this program.


JSON_PATH = json


.PHONY: all clean install update
.SILENT:


all:


clean:


install:
	DIR=$(PREFIX_THIRDPARTY)/include;													\
	echo "$(COLOR_RUN)Installing Nlohmann JSON to \"$$DIR\"...$(COLOR_RESET)";			\
	mkdir -p "$$DIR";																	\
	install "$(JSON_PATH)/src/json.hpp" "$$DIR";										\
	STATUS=$$?;																			\
	if [ "X$$STATUS" = 'X0' ]; then														\
		echo '$(COLOR_PASS)==> Nlohmann JSON installed successfully.$(COLOR_RESET)';	\
		true;																			\
	else																				\
		echo '$(COLOR_FAIL)==> Nlohmann JSON installation failed'						\
			 "(status: $$STATUS).$(COLOR_RESET)";										\
		false;																			\
	fi


update:
	cd '$(JSON_PATH)' && ( git submodule update --init; cd .. )
