# Dependencies

    if(NOT CARLSIM_NO_CUDA)
        find_package(CUDA 11 REQUIRED)
    endif()

# Targets


    if(NOT CARLSIM_NO_CUDA)
		if(MSVC)
			# 2>cl : Command line warning D9002: ignoring unknown option '-fPIC'
	        cuda_add_library(carlsim-kernel-cuda
	            src/gpu_module/snn_gpu_module.cu
	            OPTIONS
	                -gencode arch=compute_61,code=sm_61
	        )
		endif()
		if(UNIX)
	        cuda_add_library(carlsim-kernel-cuda
	            src/gpu_module/snn_gpu_module.cu
	            OPTIONS
	                -Xcompiler
	                -fPIC
	                -gencode arch=compute_61,code=sm_61
	        )

		endif()
    endif()

    add_library(carlsim-kernel 
        src/print_snn_info.cpp
        src/snn_cpu_module.cpp
        src/snn_manager.cpp
        src/spike_buffer.cpp
    )

# Properties

    # Since we build shared library enable position independent code
    set_property(TARGET carlsim-kernel PROPERTY
        POSITION_INDEPENDENT_CODE TRUE)

# Definitions

    if(CARLSIM_NO_CUDA)
        target_compile_definitions(carlsim-kernel
            PUBLIC
                -D__NO_CUDA__
        )
		if(CARLSIM_LN_I_CALC_TYPES)
		  target_compile_definitions(carlsim-kernel PUBLIC LN_I_CALC_TYPES)         
		endif()
    else()
        target_compile_definitions(carlsim-kernel-cuda
            PUBLIC
                -D__CUDA${CUDA_VERSION_MAJOR}__
        )
		if(CARLSIM_LN_I_CALC_TYPES)   
		  target_compile_definitions(carlsim-kernel-cuda PUBLIC LN_I_CALC_TYPES)         
		endif()
    endif()


# Includes

    # CUDA 11 Windows   
    #set(CUDA_SAMPLES_DIR "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.2")
    #"${CUDA_SAMPLES_DIR}/common/inc"
                
    if(NOT CARLSIM_NO_CUDA)
        target_include_directories(carlsim-kernel-cuda
            PUBLIC
                inc
                ${CUDA_INCLUDE_DIRS}
                ${CUDA_TOOLKIT_ROOT_DIR}/samples/common/inc
          )
		if(MSVC)
			# patch cMake.CUDA
	        target_include_directories(carlsim-kernel-cuda
	            PUBLIC
	                "C:/ProgramData/NVIDIA Corporation/CUDA Samples/v11.2/common/inc"            
	          )
		endif()
    endif()

    target_include_directories(carlsim-kernel
        PUBLIC
            inc
    )

# Linking

	if(UNIX)
	    set(THREADS_PREFER_PTHREAD_FLAG ON)
		find_package(Threads REQUIRED)
		target_link_libraries(carlsim-kernel PRIVATE Threads::Threads)
    endif()

    if(NOT CARLSIM_NO_CUDA)
        target_link_libraries(carlsim-kernel-cuda
            ${CUDA_curand_LIBRARY}
        )
        target_link_libraries(carlsim-kernel
            PUBLIC
                carlsim-kernel-cuda
        )
    endif()

    target_link_libraries(carlsim-kernel
            PRIVATE
#                carlsim-monitor
#                carlsim-interface
    )


# Installation

    install(
        FILES
            inc/cuda_version_control.h
            inc/error_code.h
            inc/snn_datastructures.h
            inc/snn_definitions.h
            inc/snn.h
            inc/spike_buffer.h
        DESTINATION include)

    # LN
    install(TARGETS carlsim-kernel DESTINATION lib)

    if(NOT CARLSIM_NO_CUDA)
        install(TARGETS carlsim-kernel-cuda DESTINATION lib)
    endif()