# Dependencies

find_package(GTest)

if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Targets

    add_executable(carlsim-tests
        carlsim_tests_common.cpp
        coba.cpp
        connect.cpp
        conn_mon.cpp
        core.cpp
        cuba.cpp
	exec_stopwatch.cpp
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
            #if(CARLSIM_GH_ACTIONS)
			#GTest::gtest
			#gcov
            #endif()
    )

    if(CARLSIM_GH_ACTIONS)
    target_link_libraries(carlsim-tests
       PRIVATE
            GTest::gtest
            gcov
    )
    endif()



if(MSVC)
    # Fixed Explorer Issue, see Release Notes of GTest Adapter
    # Generate Debug Information optimized for sharing and publishing (/DEBUG:FULL)
    target_link_options(carlsim-tests PRIVATE $<$<CONFIG:Debug>:/DEBUG:FULL>)
endif()

if(CARLSIM_GH_ACTIONS)
    target_compile_options(carlsim-tests PRIVATE -g -O0 -fprofile-arcs -ftest-coverage)
endif()

include(GoogleTest)
