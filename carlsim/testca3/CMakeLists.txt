# Dependencies

    find_package(GTest REQUIRED)

# Targets

    add_executable(carlsim-testsca3
        carlsim_tests_common.cpp      
        main.cpp
        ca3.cpp
    )

    #ISSUE: gtest flags gtest death test style lnk1120 unresolved externals     
    #FIX: GTEST_LINKED_AS_SHARED_LIBRARY=1
    target_compile_definitions(carlsim-testsca3
            PUBLIC
                -DGTEST_LINKED_AS_SHARED_LIBRARY=1
        )

# pthread see snn.h and carlsim.h
# //#define LN_GET_FIRING
# needs to be rebuild vs142
# and proviede as cMake -> cMake/YCMcontrib 

# Includes

    target_include_directories(carlsim-testsca3
        PRIVATE
            ${GTEST_INCLUDE_DIRS}
 #           C:/Test/github/PTHREADS-BUILT/include
    )

# Linking

    target_link_libraries(carlsim-testsca3
        PRIVATE
            carlsim-spike-generators
            ${GTEST_LIBRARIES}
#            C:/Test/github/gtest-1-10_dist/lib/gtest_main.lib
#            C:/Test/github/gtest-1-10_dist/lib/gtest.lib
#            C:/Test/github/PTHREADS-BUILT/lib/pthreadVC3d.lib
    )

if(MSVC)
    # Fixed Explorer Issue, see Release Notes of GTest Adapter
    # Generate Debug Information optimized for sharing and publishing (/DEBUG:FULL)
    target_link_options(carlsim-testsca3 PRIVATE $<$<CONFIG:Debug>:/DEBUG:FULL>)
endif()

# Release
# Param.  --gtest_filter=-*Gauss*:*eath*:*LN*
# Env.  path=C:\Test\github\carlsim6-build\Release;C:\Test\github\carlsim6-build\tools\spike_generators\Release;%path%
#
# Debug
# --gtest_filter=-*Gauss*:*Adv*:*LN*
# path=C:\Test\github\carlsim6-build\Debug;C:\Test\github\carlsim6-build\tools\spike_generators\Debug;%path%
