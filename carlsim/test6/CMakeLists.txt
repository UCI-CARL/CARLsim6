# Dependencies

    find_package(GTest REQUIRED)

# Targets

    add_executable(carlsim-tests6
        adk13.cpp
        adk13_alpha1.cpp
        carlsim_tests_common.cpp
		    main.cpp
        mstp.cpp
        pka_plc_mod.cpp
        receptors.cpp
		nrn_mon.cpp
    )

    #ISSUE: gtest flags gtest death test style lnk1120 unresolved externals     
    #FIX: GTEST_LINKED_AS_SHARED_LIBRARY=1
    target_compile_definitions(carlsim-tests6
            PUBLIC
                -DGTEST_LINKED_AS_SHARED_LIBRARY=1
        )

# pthread see snn.h and carlsim.h
# //#define LN_GET_FIRING
# needs to be rebuild vs142
# and proviede as cMake -> cMake/YCMcontrib 

# Includes

    target_include_directories(carlsim-tests6
        PRIVATE
            ${GTEST_INCLUDE_DIRS}
    #       C:/Test/github/PTHREADS-BUILT/include
    )


# Linking

    target_link_libraries(carlsim-tests6
        PRIVATE
            carlsim-spike-generators
            ${GTEST_LIBRARIES}
    )

if(MSVC)
    # Fixed Explorer Issue, see Release Notes of GTest Adapter
    # Generate Debug Information optimized for sharing and publishing (/DEBUG:FULL)
    target_link_options(carlsim-tests6 PRIVATE $<$<CONFIG:Debug>:/DEBUG:FULL>)
endif()

