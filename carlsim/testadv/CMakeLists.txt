# Dependencies

# @TODO GH_ACTIONS
#include(FetchContent)
#FetchContent_Declare(
#  googletest
#  # Specify the commit you depend on and update it regularly.
#  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
#)
## For Windows: Prevent overriding the parent project's compiler/linker settings
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)

find_package(GTest REQUIRED)

# Targets

    add_executable(carlsim-testsadv
        carlsim_tests_common.cpp      
        connect_adv.cpp
        main.cpp
        multi_runtimes.cpp
        stdp_adv.cpp
        axonplast2.cpp
		nrngrps_limit.cpp
    )

    #ISSUE: gtest flags gtest death test style lnk1120 unresolved externals     
    #FIX: GTEST_LINKED_AS_SHARED_LIBRARY=1
    target_compile_definitions(carlsim-testsadv
            PUBLIC
                -DGTEST_LINKED_AS_SHARED_LIBRARY=1
        )

# pthread see snn.h and carlsim.h
# //#define LN_GET_FIRING
# needs to be rebuild vs142
# and proviede as cMake -> cMake/YCMcontrib 

# Includes

    target_include_directories(carlsim-testsadv
        PRIVATE
            ${GTEST_INCLUDE_DIRS}
			../../tools/stopwatch
    )

# Linking

    target_link_libraries(carlsim-testsadv
        PRIVATE
            carlsim-spike-generators
			carlsim-stopwatch
            ${GTEST_LIBRARIES}
            GTest::gtest
    )

if(MSVC)
    # Fixed Explorer Issue, see Release Notes of GTest Adapter
    # Generate Debug Information optimized for sharing and publishing (/DEBUG:FULL)
    target_link_options(carlsim-testsadv PRIVATE $<$<CONFIG:Debug>:/DEBUG:FULL>)
endif()

# @TODO GH_ACTIONS
#include(GoogleTest)
#

# Release
# Param.  --gtest_filter=-*Gauss*:*eath*:*LN*
# Env.  path=C:\Test\github\carlsim6-build\Release;C:\Test\github\carlsim6-build\tools\spike_generators\Release;%path%
#
# Debug
# --gtest_filter=-*Gauss*:*Adv*:*LN*
# path=C:\Test\github\carlsim6-build\Debug;C:\Test\github\carlsim6-build\tools\spike_generators\Debug;%path%
