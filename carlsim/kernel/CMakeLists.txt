# Dependencies

    if(NOT CARLSIM_NO_CUDA)
        find_package(CUDA 11 REQUIRED)
    endif()

	if(UNIX AND NOT CARLSIM_NO_PTHREADS)
  		set(THREADS_PREFER_PTHREAD_FLAG ON)
		find_package(Threads REQUIRED)
	endif()

	if(NOT CARLSIM_STATIC)
		message(FATAL_ERROR "Invalid context, CMake will exit.")
	endif()

# Targets


    if(NOT CARLSIM_NO_CUDA)
		if(MSVC)
			# 2>cl : Command line warning D9002: ignoring unknown option '-fPIC'
	        cuda_add_library(carlsim-kernel-cuda
	            src/gpu_module/snn_gpu_module.cu
				OPTIONS
					${CARLSIM_CUDA_GENCODE}
	        )
		endif()
		if(UNIX)
	        cuda_add_library(carlsim-kernel-cuda
	            src/gpu_module/snn_gpu_module.cu
	            OPTIONS
	                -Xcompiler
	                -fPIC
					${CARLSIM_CUDA_GENCODE}
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
    else()
        target_compile_definitions(carlsim-kernel-cuda
            PUBLIC
                -D__CUDA${CUDA_VERSION_MAJOR}__
        )
    endif()

if(CARLSIM_NO_PTHREADS)
		target_compile_definitions(carlsim-kernel PUBLIC __NO_PTHREADS__)         
	endif()

    # Template 
    if(CARLSIM_LN_XYZ)
      target_compile_definitions(carlsim-kernel PUBLIC LN_XYZ_CPU)         
      if(NOT CARLSIM_NO_CUDA)
        target_compile_definitions(carlsim-kernel-cuda PUBLIC LN_XYZ_GPU)         
      endif()
    endif()

    if(CARLSIM_LN_EXT)
        target_compile_definitions(carlsim-kernel PUBLIC __LN_EXT__)               # target_precompile_headers 
    endif()

    if(CARLSIM_LN_FIRING)
        target_compile_definitions(carlsim-kernel PUBLIC LN_GET_FIRING)            # \sa carlsim.h
        target_compile_definitions(carlsim-kernel PUBLIC LN_UPDATE_CURSPIKES)      # \sa snn.h
    endif()

    if(CARLSIM_LN_FIRING_MT)
        target_compile_definitions(carlsim-kernel PUBLIC LN_GET_FIRING_MT)         # \sa carlsim.h
        target_compile_definitions(carlsim-kernel PUBLIC LN_UPDATE_CURSPIKES_MT)   # \sa snn.h
    endif()

    if(CARLSIM_LN_FIX_DA_DECAY)
      target_compile_definitions(carlsim-kernel PUBLIC LN_FIX_DA_DECAY_CPU)         
      if(NOT CARLSIM_NO_CUDA)
        target_compile_definitions(carlsim-kernel-cuda PUBLIC LN_FIX_DA_DECAY_GPU)         
      endif()
    endif()

    if(CARLSIM_LN_FIX_ALL_DECAY)
	  target_compile_definitions(carlsim-kernel PUBLIC LN_FIX_ALL_DECAY_CPU)
      if(NOT CARLSIM_NO_CUDA)
        target_compile_definitions(carlsim-kernel-cuda PUBLIC LN_FIX_ALL_DECAY_GPU)         
      endif()
    endif()

    if(CARLSIM_LN_FIX_CONNLIST)
        target_compile_definitions(carlsim-kernel PUBLIC LN_FIX_CONNLIST_)                
    endif()

    if(CARLSIM_LN_SETUP_NETWORK_MT)
      target_compile_definitions(carlsim-kernel PUBLIC LN_SETUP_NETWORK_MT)         
    endif()

    if(CARLSIM_LN_GPU_INFO)
      target_compile_definitions(carlsim-kernel PUBLIC LN_GPU_INFO)         
    endif()
         
    if(CARLSIM_LN_FIRING_DEBUG)
      #target_compile_definitions(carlsim-kernel PUBLIC DEBUG_GET_CURSPIKES2_LN)         
      #target_compile_definitions(carlsim-kernel PUBLIC DEBUG_GET_CURSPIKES2_LN_PERF)         
      target_compile_definitions(carlsim-kernel PUBLIC DEBUG_GET_CURSPIKES2_LN_MON)         
      target_compile_definitions(carlsim-kernel PUBLIC DEBUG_GET_CURSPIKES2_LN_CHECKS)         
    endif()      

    if(CARLSIM_LN_I_CALC_TYPES)
      target_compile_definitions(carlsim-kernel PUBLIC LN_I_CALC_TYPES)   
	  if(NOT CARLSIM_NO_CUDA)
		target_compile_definitions(carlsim-kernel-cuda PUBLIC LN_I_CALC_TYPES)
	  endif()
    endif()

    if(CARLSIM_LN_GPU_STDP)
	  if(NOT CARLSIM_NO_CUDA)
		target_compile_definitions(carlsim-kernel-cuda PUBLIC LN_GPU_STDP)
	  endif()
    endif()

	if(CARLSIM_JK_CA3_SSN) 
		target_compile_definitions(carlsim-kernel PUBLIC JK_CA3_SNN) 
		if(NOT CARLSIM_NO_CUDA)
			target_compile_definitions(carlsim-kernel-cuda PUBLIC JK_CA3_SNN)
		endif()
	endif()

	if(CARLSIM_FIX_SYNCTHREADS)
      if(NOT CARLSIM_NO_CUDA)
        target_compile_definitions(carlsim-kernel-cuda PUBLIC CS4_FIX_SYNCTHREADS)         
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
	        target_include_directories(carlsim-kernel-cuda
	            PUBLIC
					"C:/ProgramData/NVIDIA Corporation/CUDA Samples/v${CUDA_VERSION_STRING}/common/inc" 					
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