# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for third-party parts of this program.


export LOGS_DIR				= $(shell pwd)/log

# Jobs number for building third-party submodules
# Uses /proc/cpuinfo for Linux,
#      sysctl for Mac OS,
#      psrinfo for Solaris
#      4 for others (or if error occured)
DEFAULT_JOBS			= 4

export JOBS				= $(shell													\
								grep -c '^processor' /proc/cpuinfo 2>/dev/null ||	\
								sysctl -n hw.ncpu 2>/dev/null ||					\
								psrinfo -p 2>/dev/null ||							\
								echo '$(DEFAULT_JOBS)'								\
							)


TPS							= $(shell find . -maxdepth 1 -mindepth 1 -type d -not -name 'log')

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
	mkdir -p '$(LOGS_DIR)'


clean:
	$(call for_each_tp,clean)


install:
	@echo "$(COLOR_RUN)Installing third-party files to \"$(PREFIX_THIRDPARTY)\"...$(COLOR_RESET)"
	$(call for_each_tp,install)


uninstall:
	rm -rf '$(PREFIX_THIRDPARTY)' 2>/dev/null || true
	@echo "$(COLOR_PASS)==> Third-party files removed.$(COLOR_RESET)"


update:
	$(call for_each_tp,update)
