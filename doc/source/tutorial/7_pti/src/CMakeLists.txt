project(tutorial-pti)

cmake_minimum_required(VERSION 3.21)


# Dependencies

  find_package(carlsim REQUIRED)

# Targets

    add_executable(TuneFiringRates
        main_TuneFiringRates.cpp
    )

    if(CARLSIM_NO_CUDA)
        target_compile_definitions(TuneFiringRates 
            PUBLIC
                -D__NO_CUDA__
            )
    endif()

# include 
    target_include_directories(TuneFiringRates PRIVATE
        ${CARLSIM_INCLUDE_DIR}
        )

# Linking

    target_link_directories(TuneFiringRates PRIVATE        
        ${CARLSIM_LIB_DIR}
        )

    if(NOT CARLSIM_NO_CUDA)
        target_link_directories(TuneFiringRates PRIVATE
        ${CARLSIM_CUDA_LIB_DIR}    
        )
    endif()

    target_link_libraries(TuneFiringRates PRIVATE
         carlsim$<$<CONFIG:Debug>:d>
         carlsim-stopwatch$<$<CONFIG:Debug>:d>
         carlsim-pti
    )

    if(NOT CARLSIM_NO_CUDA)
        target_link_libraries(TuneFiringRates PRIVATE
            cudart
            curand
        )
    endif()

