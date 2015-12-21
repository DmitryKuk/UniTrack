# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This Makefile contains some useful functions, that can simplify writing Makefiles for submodules.
# NOTE: the root Makefile exports MK_UTILITY varialbe, so don't forget:
# include $(MK_UTILITY)


# Helper functions
# Usage example: $(call get_src_files,main.cpp)
get_src_files			= $(addprefix $($(ID)_SRC_DIR_CURR)/,$(1))
get_obj_files			= $(addprefix $($(ID)_OBJ_DIR_CURR)/,$(1))
get_test_files			= $(addprefix $($(ID)_TEST_DIR_CURR)/,$(1))

get_bin_files			= $(addprefix $(BIN_DIR)/,$(1))
get_lib_files			= $(addsuffix .so,$(addprefix $(LIB_DIR)/lib$(PROJECT_LIB_PREFIX),$(1)))


cpp_to_obj				= $(1:.cpp=.o)
obj_to_dep				= $(1:.o=.d)
lib_to_module			= $(basename $(subst lib$(PROJECT_LIB_PREFIX),,$(notdir $1)))
bin_to_target			= $(notdir $(1))

get_targets				= $(basename $(1))
get_libs				= $(addprefix -l$(PROJECT_LIB_PREFIX),$(1))
get_external_libs		= $(addprefix -l,$(1))



# Compile and link commands. Usage: $(call gpp_compile,OUTPUT_FILE,INPUT_FILES)
gpp_compile				= $(GPP) $(GPP_COMPILE_FLAGS) $($(ID)_GPP_COMPILE_FLAGS)		\
								 $(GPP_HEADER_PATHS) $($(ID)_GPP_HEADER_PATHS)			\
								 -MMD -MT '$(1)' -MF '$(call obj_to_dep,$(1))'			\
								 -o '$(1)' $(2)

gpp_shared_lib			= $(GPP) $(GPP_SHARED_LIB_FLAGS) $($(ID)_GPP_SHARED_LIB_FLAGS)	\
								 $(GPP_LIB_PATHS) $($(ID)_GPP_LIB_PATHS)				\
								 $(GPP_LIBS) $($(ID)_GPP_LIBS)							\
								 -o '$(1)' $(2)

gpp_link				= $(GPP) $(GPP_LINK_FLAGS) $($(ID)_GPP_LINK_FLAGS)				\
								 $(GPP_LIB_PATHS) $($(ID)_GPP_LIB_PATHS)				\
								 $(GPP_LIBS) $($(ID)_GPP_LIBS)							\
								 -o '$(1)' $(2)


target_name_from_this	= $(shell basename $$( dirname '$(1)' ))


# Usage: in run and run-tests recipies write:											\
	$(def_print_status_table);															\
	NUM_SUCCESS=[Number of passed tests or successfully exited programs];				\
	NUM_FAIL=[Number of failed tests or unsuccessfully exited programs];				\
	print_status_table
def_print_status_table	=																\
	function print_status_table() {														\
		HEADER_SUCCESS="Success"; HEADER_FAIL="Fail"; HEADER_TOTAL="Total";				\
		let NUM_TOTAL=NUM_SUCCESS+NUM_FAIL;												\
		PERC_SUCCESS="$$( echo "100*$$NUM_SUCCESS/$$NUM_TOTAL" | bc )";					\
		let PERC_FAIL=100-PERC_SUCCESS;													\
		PERC_SUCCESS+='%'; PERC_FAIL+='%';												\
																						\
		LEN=0;																			\
		for I in $$HEADER_SUCCESS $$HEADER_SUCCESS $$NUM_SUCCESS $$NUM_FAIL $$NUM_TOTAL	\
				 $$PERC_SUCCESS $$PERC_FAIL; do											\
			L=$${\#I}; (( $$L > $$LEN )) && LEN=$$L;									\
		done;																			\
																						\
		FMT="";																			\
		FMT+="==> Status:  |  $(COLOR_PASS)%$${LEN}s$(COLOR_RESET)  |  ";				\
		FMT+="$(COLOR_FAIL)%$${LEN}s$(COLOR_RESET)  |  %$${LEN}s  |\n";					\
		FMT+="             |  $(COLOR_PASS)%$${LEN}s$(COLOR_RESET)  |  ";				\
		FMT+="$(COLOR_FAIL)%$${LEN}s$(COLOR_RESET)  |  %$${LEN}s  |\n";					\
		FMT+="             |  $(COLOR_PASS)%$${LEN}s$(COLOR_RESET)  |  ";				\
		FMT+="$(COLOR_FAIL)%$${LEN}s$(COLOR_RESET)  |  %$${LEN}s  |\n";					\
																						\
		printf "$$FMT" "$$HEADER_SUCCESS" "$$HEADER_FAIL" "$$HEADER_TOTAL"				\
			   "$$NUM_SUCCESS" "$$NUM_FAIL" "$$NUM_TOTAL"								\
			   "$$PERC_SUCCESS" "$$PERC_FAIL" "100%";									\
	}
