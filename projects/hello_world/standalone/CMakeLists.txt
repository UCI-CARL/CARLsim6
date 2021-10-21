project(carlsim-hello-world)
cmake_minimum_required(VERSION 3.19.4)


# Dependencies

  find_package(carlsim REQUIRED)

# Targets

    add_executable(hello_world
        src/main_hello_world.cpp
    )

    if(CARLSIM_NO_CUDA)
        target_compile_definitions(hello_world 
            PUBLIC
                -D__NO_CUDA__
            )
    endif()

# include 
    target_include_directories(hello_world PRIVATE
        ${CARLSIM_INCLUDE_DIR}
        )

# Linking

    target_link_directories(hello_world PRIVATE        
        ${CARLSIM_LIB_DIR}
        )

    if(NOT CARLSIM_NO_CUDA)
        target_link_directories(hello_world PRIVATE
        ${CARLSIM_CUDA_LIB_DIR}    
        )
    endif()

    target_link_libraries(hello_world PRIVATE
         carlsim$<$<CONFIG:Debug>:d>.lib
         carlsim-stopwatch$<$<CONFIG:Debug>:d>.lib
#        carlsim-interface
#       carlsim-kernel
#       carlsim-monitor
#        carlsim-simple-weight-tuner
#        carlsim-spike-generators
#        carlsim-stopwatch
#        carlsim-visual-stimulus
    )

    if(NOT CARLSIM_NO_CUDA)
        target_link_libraries(hello_world PRIVATE
            carlsim-kernel-cuda
            cudart
            curand
        )
    endif()

