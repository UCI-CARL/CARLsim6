project(benchmark)

# Targets  
add_executable(benchmark
    benchmark_neurons.cpp
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(benchmark PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking   
if(CARLSIM_STATIC)
    target_link_libraries(benchmark PRIVATE
        -static
        carlsim
        carlsim-stopwatch
    )
else()
    target_link_libraries(benchmark PRIVATE
        carlsim
        carlsim-stopwatch
    )
endif()

target_include_directories(benchmark PRIVATE
    ../../tools/stopwatch
)

        
# Installation
   
install(TARGETS benchmark
    EXPORT carlsim
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples
        COMPONENT benchmark
        )

install( FILES 
  run_benchmark  
  DESTINATION $<$<CONFIG:Debug>:debug/>samples )

  
# TODO  cMake debugging environment
# https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-160
# https://docs.microsoft.com/en-us/cpp/build/customize-cmake-settings?view=msvc-160
# The options that you choose in the editor are written to a file called CMakeSettings.json. 
# This file provides command-line arguments and environment variables that are passed to CMake when you build the projects. 
# Visual Studio never modifies CMakeLists.txt automatically; by using CMakeSettings.json 
# you can customize the build through Visual Studio while leaving the CMake project files untouched 
# so that others on your team can consume them with whatever tools they're using   
