# Dependencies

include(FetchContent)
FetchContent_Declare(
	googletest
	# Specify the commit you depend on and update it regularly.
	URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
)


# Linking

target_link_libraries(carlsim-tests6
PRIVATE
    carlsim-spike-generators
    ${GTEST_LIBRARIES}	    
    GTest::gtest
)

if(MSVC)
    # Fixed Explorer Issue, see Release Notes of GTest Adapter
    # Generate Debug Information optimized for sharing and publishing (/DEBUG:FULL)
    target_link_options(carlsim-tests6 PRIVATE $<$<CONFIG:Debug>:/DEBUG:FULL>)
endif()

if(CARLSIM_TEST)
    target_link_libraries(carlsim-tests6
        PRIVATE
        gcov
    )
    target_compile_options(carlsim-tests6 PRIVATE -g -O0 -fprofile-arcs -ftest-coverage)
endif()

include(GoogleTest)
