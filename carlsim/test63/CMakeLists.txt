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
add_executable(carlsim-tests63
	carlsim_tests_common.cpp
	main.cpp
	norm_spike_gen.cpp
	cb_mon.cpp
	#perf_mon.cpp
	perf_mon_optim.cpp
	perf_mon_slice.cpp
	#perf_mon_slice_pcm.cpp
	perf_mon_threads.cpp
	perf_mon_integ.cpp
	perf_mon_ms_pdh.cpp
	perf_mon_intel_pcm.cpp
	perf_mon_partition.cpp
	#perf_mon_intel_msr.cpp
	#msr.cpp
	#pthreads_pool.cpp
	#pthreads_test.cpp
	#pthreads_worker.cpp
	#thread_worker.cpp
	thread_worker_copilot.cpp
	thread_worker_pool.cpp
)

#ISSUE: gtest flags gtest death test style lnk1120 unresolved externals     
#FIX: GTEST_LINKED_AS_SHARED_LIBRARY=1
target_compile_definitions(carlsim-tests63
    PUBLIC
	-DGTEST_LINKED_AS_SHARED_LIBRARY=1
)

# pthread see snn.h and carlsim.h
# //#define LN_GET_FIRING
# needs to be rebuild vs142
# and proviede as cMake -> cMake/YCMcontrib 

# Includes

target_include_directories(carlsim-tests63
PRIVATE
    ${GTEST_INCLUDE_DIRS}
)


# Linking

target_link_libraries(carlsim-tests63
PRIVATE
    carlsim-spike-generators
    ${GTEST_LIBRARIES}
#    if(CARLSIM_GH_ACTIONS)
#	GTest::gtest
#	gcov
#    endif()
	pthread
	#target_link_libraries(carlsim PRIVATE C:/Test/sourceforge/PTHREADS-BUILT/lib/pthreadVSE3.lib)
	#target_link_libraries(carlsim PRIVATE C:/Test/sourceforge/PTHREADS-BUILT/lib/pthreadVC3.lib)

	#"C:/Program Files/PCM/lib/pcm-lib.lib"
	"/home/vyomshah/pcm/build/lib/libpcm.so"

)



if(CARLSIM_GH_ACTIONS)
    target_link_libraries(carlsim-tests63
    PRIVATE
        GTest::gtest
        gcov
    )
endif()

if(MSVC)
    # Fixed Explorer Issue, see Release Notes of GTest Adapter
    # Generate Debug Information optimized for sharing and publishing (/DEBUG:FULL)
    target_link_options(carlsim-tests63 PRIVATE $<$<CONFIG:Debug>:/DEBUG:FULL>)
endif()

if(CARLSIM_GH_ACTIONS)
    target_compile_options(carlsim-tests63 PRIVATE -g -O0 -fprofile-arcs -ftest-coverage)
endif()

include(GoogleTest)
