#------------------------------------------------------------#
#---------------- INITIAL PROJECT INVOKATION ----------------#
#------------------------------------------------------------#
cmake_minimum_required(VERSION 3.21)
project(CUSB_LIBRARY VERSION 0.1)
set(CUSB_SUPPORTED_COMPILERS "GNU")
option(CUSB_DISABLE_RUNTIME_ASSERTS "ON = runtime asserts disabled. OFF = runtime asserts enabled." OFF)

if(NOT CMAKE_C_COMPILER_ID IN_LIST CUSB_SUPPORTED_COMPILERS)
    message(WARNING "Using untested compiler. Currently supported compilers = ${CUSB_SUPPORTED_COMPILERS}")
endif()

#------------------------------------------------------------#
#---------------------- GET DEPENDENCIES --------------------#
#------------------------------------------------------------#
include(FetchContent)
FetchContent_Declare(
    ecu
    GIT_REPOSITORY https://github.com/ress059/ecu.git
    GIT_TAG        3d7461497cc1bdbf9a16e3241aa336574c8ce3b1 # Latest master. TODO: Update to latest once ecu released.
)
FetchContent_MakeAvailable(ecu)

#------------------------------------------------------------#
#----------------------- CUSB SETTINGS ----------------------#
#------------------------------------------------------------#
# Note this library is meant to be compiled with the target 
# application's toolchain.
add_library(cusb STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/dummy.c
)

# Example include in the Application would be #include "cusb/device.h" 
# in order to give larger apps some scope when including files.
target_include_directories(cusb
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/inc 
)

# CUSB library requires at least C99.
target_compile_features(cusb 
    PUBLIC 
        c_std_99    
)

# Store CUSB-specific warning flags in an interface target in case it is
# also needed by test builds. Enable all warnings and relevant debug info.
# An optimization flag is not specified on purpose so it can be defined by 
# the application. I.e. after importing cusb the application can do: 
# target_compile_options(cusb PRIVATE -O2). It is OK if the application does 
# not specify an optimization level - GCC will default it to -O0.
# Note that -Werror will only be passed for build tests.
add_library(cusb_warning_options INTERFACE)
target_compile_options(cusb_warning_options 
    INTERFACE 
        # C compiler flags. We know CUSB is a C library so we can use this generator expression.
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-fdiagnostics-color=always -ffunction-sections -fdata-sections -fstack-usage -fno-common>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wundef -Wshadow -Wstack-usage=200>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wcast-align -Wstrict-prototypes -Wstrict-overflow=2 -Wwrite-strings -Waggregate-return>
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-Wcast-qual -Wswitch-default -Wimplicit-fallthrough -Wnull-dereference -Wdouble-promotion>

        # Compiler flags specific to Debug builds
        $<$<AND:$<CONFIG:Debug>,$<OR:$<COMPILE_LANG_AND_ID:C,GNU>,$<COMPILE_LANG_AND_ID:CXX,GNU>>>:-g3>
)

# CUSB_DISABLE_RUNTIME_ASSERTS is a wrapper around ECU_DISABLE_RUNTIME_ASSERTS.
# Set ECU_DISABLE_RUNTIME_ASSERTS to whatever CUSB_DISABLE_RUNTIME_ASSERTS 
# is. ECU library will show warning if runtime asserts are disabled. It is
# not recommended to disable run-time asserts but we have this
# as an option to decrease overhead of the library.
set(ECU_DISABLE_RUNTIME_ASSERTS ${CUSB_DISABLE_RUNTIME_ASSERTS} CACHE BOOL "ON = runtime asserts disabled. OFF = runtime asserts enabled." FORCE)

# Note how CUSB-specific warning flags are private so it is not propogated
# to the user's application. However our internal tests can still use 
# cusb_warning_options target to get all of the warning flags.
target_link_libraries(cusb 
    PUBLIC
        ecu
    PRIVATE
        cusb_warning_options
)

#------------------------------------------------------------#
#---------------------- INTERNAL TESTS ----------------------#
#------------------------------------------------------------#
if(${CUSB_ENABLE_BUILD_TESTING})
    add_subdirectory(tests/build)
elseif(${CUSB_ENABLE_UNIT_TESTING})
    target_compile_definitions(cusb
        PUBLIC 
            CUSB_UNIT_TEST 
    )

    # CTest and enable_testing has to be in the root CMakeLists.txt. 
    # Tests will not be discovered if this is in tests/CMakeLists.txt....
    include(CTest)
    enable_testing()
    add_subdirectory(tests/unit)
elseif(${CUSB_ENABLE_INTEGRATION_TESTING})
    add_subdirectory(tests/integration)
endif()
