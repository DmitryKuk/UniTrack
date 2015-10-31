# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This is Makefile for building the main part of program. This part
# of program has object files as targets, so there is no rules for
# linking executeble or static/dynamic library. See root Makefile
# and modules' Makefiles, they do that.

# NOTE: This Makefile is NOT for modules building, for that purpose
# see the root Makefile (../Makefile).


# Makefile advanced
include $(MAKEFILE_ADVANCED_ABS)


# Requirements (project modules linked automatically)
ifeq ($(SYSTEM),Darwin)
	GPP_LIBS				+= -lboost_system-mt -lboost_filesystem-mt
else
	GPP_LIBS				+= -lboost_system -lpthread -lboost_filesystem
endif


GPP_LIBS					+= -lut_base -lut_logger -lut_server -lut_templatizer


# Paths
SOURCES_DIR_CURR			= $(SOURCES_DIR_ABS)
BUILD_DIR_CURR				= $(BUILD_DIR_ABS)
LIBS_DIR_CURR				= $(LIBS_DIR_ABS)
OBJECTS_DIR_CURR			= $(OBJECTS_DIR_ABS)
TEST_DIR_CURR				= $(TEST_DIR_ABS)


# Sources and headers
HEADERS =																	\
	$(notdir $(shell find $(SOURCES_DIR_CURR)								\
		-maxdepth 1 \( -type f -or -type l \)								\
		-not \( -name 'test*' -or -name 'main*' \)							\
		\( -name '*.h' -or -name '*.hpp' \) ))


SOURCES_CPP =																\
	$(notdir $(shell find $(SOURCES_DIR_CURR)								\
		-maxdepth 1 \( -type f -or -type l \)								\
		-not \( -name 'test*' -or -name 'main*' \)							\
		-name '*.cpp' ))


# Tests
TEST_SOURCES_CPP =															\
	$(notdir $(shell find $(SOURCES_DIR_CURR)								\
		-maxdepth 1 \( -type f -or -type l \)								\
		-not -name 'main*'													\
		-name 'test*.cpp' ))


# Sources needed for only main targets
MAIN_SOURCES_CPP =															\
	$(notdir $(shell find $(SOURCES_DIR_CURR)								\
		-maxdepth 1 \( -type f -or -type l \)								\
		-not -name 'test*'													\
		-name 'main*.cpp' ))


# Files
HEADER_FILES				= $(call get_sources_files,$(HEADERS))
SOURCES_CPP_FILES			= $(call get_sources_files,$(SOURCES_CPP))
MAIN_SOURCES_CPP_FILES		= $(call get_sources_files,$(MAIN_SOURCES_CPP))

# Object files
OBJECTS						= $(call get_objects_cpp,$(SOURCES_CPP))
MAIN_OBJECTS				= $(call get_objects_cpp,$(MAIN_SOURCES_CPP))
TEST_OBJECTS				= $(call get_objects_cpp,$(TEST_SOURCES_CPP))

OBJECT_FILES				= $(call get_object_files,$(OBJECTS))
MAIN_OBJECT_FILES			= $(call get_object_files,$(MAIN_OBJECTS))
TEST_OBJECT_FILES			= $(call get_object_files,$(TEST_OBJECTS))

# Target files
TEST_TARGETS				= $(call get_targets,$(TEST_SOURCES_CPP))
TEST_TARGET_FILES			= $(call get_test_files,$(TEST_TARGETS))


# Targets
.PHONY: all clean clean-tests check dirs main tests run-tests
.SILENT:


all: dirs main


clean: clean-tests
	rm -rf $(OBJECT_FILES) $(MAIN_OBJECT_FILES) 2>/dev/null || true


clean-main:
	rm $(MAIN_TARGET) 2>/dev/null || true


clean-tests:
	rm -rf $(TEST_OBJECT_FILES) $(TEST_TARGET_FILES) 2>/dev/null || true


# Tests targets
check: dirs run-tests


dirs:
	mkdir -p $(sort $(dir $(OBJECT_FILES) $(TEST_OBJECT_FILES) \
						  $(TEST_TARGET_FILES)))


main: $(OBJECT_FILES) $(MAIN_OBJECT_FILES)


run: all
	echo "$(COLOR_RUN)Running program: $MAIN_TARGET...$(COLOR_RESET)";							\
	$(call get_target_files,$MAIN_TARGET);														\
	STATUS=$$?;																					\
	if [ "X$$STATUS" = 'X0' ]; then																\
		echo "$(COLOR_PASS)==> Program $MAIN_TARGET completed successfully.$(COLOR_RESET)";		\
	else																						\
		echo "$(COLOR_FAIL)==> Program $MAIN_TARGET failed with code: $$STATUS.$(COLOR_RESET)";	\
	fi


tests: $(TEST_TARGET_FILES)


run-tests: tests
	if [ "X$(TEST_TARGETS)" != "X" ]; then															\
		for T in $(TEST_TARGETS); do																\
			echo "$(COLOR_RUN)Running test: $$T (main)...$(COLOR_RESET)";							\
			$(call get_test_files,$$T);																\
			STATUS=$$?;																				\
			if [ "X$$STATUS" = 'X0' ]; then															\
				echo "$(COLOR_PASS)==> Test $$T (main) passed.$(COLOR_RESET)";						\
			else																					\
				echo "$(COLOR_FAIL)==> Test $$T (main) failed with code: $$STATUS.$(COLOR_RESET)";	\
			fi;																						\
		done;																						\
	fi


# Objects compilation (universal for main program and tests)
$(OBJECTS_DIR_CURR)/%.o: $(SOURCES_DIR_CURR)/%.cpp $(HEADER_FILES)
	@echo "    $(COLOR_RUN)Compiling: $(subst $(OBJECTS_DIR_CURR)/,,$@) (main)...$(COLOR_RESET)"
	$(call gpp_compile) -o '$@' '$<'


# Tests
$(TEST_DIR_CURR)/%: $(OBJECTS_DIR_CURR)/%.o $(HEADER_FILES) $(OBJECT_FILES)
	@echo "    $(COLOR_RUN)Linking test: $(subst $(TEST_DIR_CURR)/,,$@) (main)...$(COLOR_RESET)"
	$(call gpp_main_test_link) -o '$@' '$<' $(OBJECT_FILES)


# Main target
$(TARGET_FILE): $(OBJECTS) $(MAIN_OBJECTS)
	TARGET=$(subst $(BUILD_DIR_CURR)/,,$@);														\
	echo "$(COLOR_RUN)Linking target: $$TARGET (main)...$(COLOR_RESET)";						\
	$(call gpp_link) $(GPP_LIBS_CURR) -o "$@" $^;												\
	STATUS=$$?;																					\
	if [ "X$$STATUS" = 'X0' ]; then																\
		echo "$(COLOR_PASS)==> Target $$TARGET linked successfully.$(COLOR_RESET)";				\
	else																						\
		echo "$(COLOR_FAIL)==> Target $$TARGET linking failed.$(COLOR_RESET)";					\
	fi;
