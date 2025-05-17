#------------------------------------------------------------#
#----------------------- CUSB SETTINGS ----------------------#
#------------------------------------------------------------#
# Do the build test in C23. Pass -Werror when compiling CUSB
# source files and our build test code so CI fails when any
# build produces warnings. -Werror is purposefully not passed
# in the top-level CMake and only done here in our build tests.
target_compile_features(cusb 
    PUBLIC 
        c_std_23 
)

target_compile_options(cusb 
    PUBLIC 
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Werror>
)

#------------------------------------------------------------#
#------------------- BUILD TEST SETTINGS --------------------#
#------------------------------------------------------------#
add_executable(CUSB_BUILD_TEST 
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

target_compile_options(CUSB_BUILD_TEST
    PRIVATE
        # Save debug symbols.
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-g3>
)

# Have build test code use the same warning flags as CUSB, with the addition of -Werror.
target_link_libraries(CUSB_BUILD_TEST 
    PRIVATE 
        cusb
        cusb_warning_options
)
