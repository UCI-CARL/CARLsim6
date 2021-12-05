# Dependencies

    find_package(GTest REQUIRED)

# Targets

    add_executable(carlsim-tests
        carlsim_tests_common.cpp
        coba.cpp
        connect.cpp
        conn_mon.cpp
        core.cpp
        cuba.cpp
        group_mon.cpp
        interface.cpp
        main.cpp
        poiss_rate.cpp
        spike_gen.cpp
        spike_mon.cpp
        stdp.cpp
        stp.cpp
    )

    #ISSUE: gtest flags gtest death test style lnk1120 unresolved externals     
    #FIX: GTEST_LINKED_AS_SHARED_LIBRARY=1
    target_compile_definitions(carlsim-tests
            PUBLIC
                -DGTEST_LINKED_AS_SHARED_LIBRARY=1
        )



# Includes

    target_include_directories(carlsim-tests
        PRIVATE
            ${GTEST_INCLUDE_DIRS}
    )

# Linking

    target_link_libraries(carlsim-tests
        PRIVATE
            carlsim-spike-generators
            ${GTEST_LIBRARIES}
#            C:/Test/github/gtest-1-10_dist/lib/gtest_main.lib
#            C:/Test/github/gtest-1-10_dist/lib/gtest.lib
    )


if(MSVC)
    # Fixed Explorer Issue, see Release Notes of GTest Adapter
    # Generate Debug Information optimized for sharing and publishing (/DEBUG:FULL)
    target_link_options(carlsim-tests PRIVATE $<$<CONFIG:Debug>:/DEBUG:FULL>)
endif()
