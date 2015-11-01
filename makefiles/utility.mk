# Author: Dmitry Kukovinets (d1021976@gmail.com)

# This Makefile contains some useful functions, that can simplify writing Makefiles for submodules.
# NOTE: the root Makefile exports MK_UTILITY_ABS varialbe, so don't forget:
# include $(MK_UTILITY_ABS)


# Helper functions
# Usage example: $(call get_src_files,main.cpp)
get_src_files			= $(addprefix $(SRC_DIR_CURR)/,$(1))
get_bin_files			= $(addprefix $(BIN_DIR_CURR)/,$(1))
get_lib_files			= $(addsuffix .so,$(addprefix $(LIB_DIR_CURR)/lib$(PROJECT_LIB_PREFIX),$(1)))
get_obj_files			= $(addprefix $(OBJ_DIR_CURR)/,$(1))

cpp_to_obj				= $(1:.cpp=.o)
lib_to_module			= $(basename $(subst lib$(PROJECT_LIB_PREFIX),,$(notdir $1)))

get_test_files			= $(addprefix $(TEST_DIR_CURR)/,$(1))
get_targets				= $(basename $(1))
get_libs				= $(addprefix -l$(PROJECT_LIB_PREFIX),$(1))
get_external_libs		= $(addprefix -l,$(1))


gpp_compile				= $(GPP) $(GPP_COMPILE_FLAGS) $(GPP_HEADER_PATHS)
gpp_shared_lib			= $(GPP) $(GPP_SHARED_LIB_FLAGS) $(GPP_LIB_PATHS) $(GPP_LIBS)
gpp_link				= $(GPP) $(GPP_LINK_FLAGS) $(GPP_LIB_PATHS) $(GPP_LIBS)
