# Targets

    add_library(carlsim-interface 
        src/callback_core.cpp
        src/carlsim.cpp
        src/linear_algebra.cpp
        src/poisson_rate.cpp
        src/user_errors.cpp
    )

# Properties

    # Since we build shared library enable position independent code
    set_property(TARGET carlsim-interface PROPERTY
        POSITION_INDEPENDENT_CODE TRUE)

    if(CARLSIM_LN_EXT)
        target_compile_definitions(carlsim-interface
            PUBLIC
                -D__LN_EXT__
        )
    endif()

	if(CARLSIM_LN_I_CALC_TYPES)
      target_compile_definitions(carlsim-interface PUBLIC LN_I_CALC_TYPES)         
    endif()



# Includes

    target_include_directories(carlsim-interface
        PUBLIC
            inc
    )

# Linking


    target_link_libraries(carlsim-interface
        PUBLIC
            carlsim-kernel
            ${CUDA_curand_LIBRARY}
    )

# Installation

    install(
        FILES
            inc/callback_core.h
            inc/callback.h
            inc/carlsim_datastructures.h
            inc/carlsim_definitions.h
            inc/carlsim.h
            inc/carlsim_log_definitions.h
            inc/linear_algebra.h
            inc/poisson_rate.h
            inc/user_errors.h
        DESTINATION include)

# LN
    install(TARGETS carlsim-interface DESTINATION lib)