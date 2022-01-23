project(benchmark1)
cmake_minimum_required(VERSION 3.19)

# Dependencies
  find_package(carlsim REQUIRED)    
        
# Targets  
add_executable(benchmark1
    ../src/main_benchmark1.cpp
)

# include 
target_include_directories(benchmark1 PRIVATE
    ${CARLSIM_INCLUDE_DIR}
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(benchmark1 PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_directories(benchmark1 PRIVATE        
    ${CARLSIM_LIB_DIR}
    )
    
if(NOT CARLSIM_NO_CUDA)
    target_link_directories(benchmark1 PRIVATE
    ${CARLSIM_CUDA_LIB_DIR}    
    )
endif()
    
target_link_libraries(benchmark1 PRIVATE
        carlsim$<$<CONFIG:Debug>:d>.lib
        carlsim-stopwatch$<$<CONFIG:Debug>:d>.lib
)
    
if(NOT CARLSIM_NO_CUDA)
    target_link_libraries(benchmark1 PRIVATE
        cudart
        curand
    )
endif()
        

  
# TODO  cMake debugging environment
# https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-160
# https://docs.microsoft.com/en-us/cpp/build/customize-cmake-settings?view=msvc-160
# The options that you choose in the editor are written to a file called CMakeSettings.json. 
# This file provides command-line arguments and environment variables that are passed to CMake when you build the projects. 
# Visual Studio never modifies CMakeLists.txt automatically; by using CMakeSettings.json 
# you can customize the build through Visual Studio while leaving the CMake project files untouched 
# so that others on your team can consume them with whatever tools they're using   