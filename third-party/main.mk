# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for third-party parts of this program.


export LOGS_DIR				= $(shell pwd)/log

# Default jobs number for building third-party submodules
export DEFAULT_JOBS			= 4


TPS							= $(shell find . -depth 1 -type d -not -name 'log')

for_each_tp					=	\
	for TP in $(TPS); do		\
		$(MAKE) -C "$$TP" $(1);	\
	done


.PHONY: all prepare_to_build clean install uninstall update
.SILENT:


all: prepare_to_build
	$(call for_each_tp,all)


prepare_to_build:
	# Downloading submodules, if them was not cloned
	git submodule update --init
	
	# Directory for logs of thirdparty modules building and installing
	mkdir -p "$(LOGS_DIR)"


clean:
	$(call for_each_tp,clean)


install:
	@echo "$(COLOR_RUN)Installing third-party files to \"$(PREFIX_THIRDPARTY)\"...$(COLOR_RESET)"
	$(call for_each_tp,install)


uninstall:
	rm -rf $(PREFIX_THIRDPARTY) 2>/dev/null || true
	@echo "$(COLOR_PASS)==> Third-party files removed.$(COLOR_RESET)"


update:
	$(call for_each_tp,update)
