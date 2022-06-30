project(carlsim)

cmake_minimum_required(VERSION 3.21.0)

# specify the C++ standard aligned to YARP
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#LN20201126  https://cmake.org/cmake/help/latest/guide/tutorial/index.html#id20
set(CMAKE_DEBUG_POSTFIX d)

# LN2021 enforce parallel build as default
if(MSVC)
  get_property(_USER_CMAKE_CXX_FLAGS CACHE CMAKE_CXX_FLAGS PROPERTY VALUE)
  string(FIND ${_USER_CMAKE_CXX_FLAGS} "/MP" _MSVC_MP_IDX)
  if(_MSVC_MP_IDX EQUAL -1)
    message("enforce parallel build")
    set(CMAKE_CXX_FLAGS "${_USER_CMAKE_CXX_FLAGS} /MP" CACHE STRING "enforce parallel build" FORCE)    
  endif()
endif()

# Options

    # all options have to be captured in the carlsim_conf.h.in, see below

    option(CARLSIM_NO_CUDA "Turn off CUDA support" OFF)         # more for boot straping and quality ensurance
    option(CARLSIM_NO_PTHREADS "Turn off PTHREADS" ON)          # since CARLsim5, Linux Release builds throw segmentation violation 
    option(CARLSIM_SHARED "Build CARLSIM als shared libray" ON)     # this is the primary supported way
    option(CARLSIM_STATIC "Build CARLSIM als static libray" OFF)     # for trouble shooting and special requirements (e.g. single image appliances)
    option(CARLSIM_BENCHMARKS "Build CARLSIM benchmark tests" ON)   # 
    option(CARLSIM_SAMPLES "Build CARLSIM samples" ON)          # not for minimal version
    option(CARLSIM_TUTORIALS "Build CARLSIM tutorials" OFF)      # not for minimal version and production     
    option(CARLSIM_PYCARL "Build CARLSIM PyCARL" ON)            # not for CR but SNN
    option(CARLSIM_TOOLS "Build CARLSIM Tools" ON)              # check for dependecies
    option(CARLSIM_TEST "Build CARLSIM Tests" ON)               # mandadory for development
         
    # an option translates to one or more compile definitions assigned to one or multiple targets
    # it can be set manually or by a super build customized to a specific profil
    option(CARLSIM_LN_EXT "LN's Extension" ON)	# a option is ON, if it is working (DONE), and can switched off by the context (s.build / profile)
    option(CARLSIM_LN_FIRING "Firing information for integration as plugin" ON) 
    #option(CARLSIM_LN_FIRING_MT "Multi Threaded Firing (experimental)" OFF)    # still experimental
    #option(CARLSIM_LN_FIRING_DEBUG "Debug Firing and its tests" OFF)   # optional
    option(CARLSIM_LN_FIX_ALL_DECAY "Fix NM4 Decay for CPU and GPU" ON )  # generalization of fix for DA Decay
    #option(CARLSIM_LN_FIX_DA_DECAY "Fix DA Decay for CPU and GPU" OFF )  # obsolete
    option(CARLSIM_LN_FIX_CONNLIST "Fix Connection List " OFF)    # \todo issue -> what was the status of VS2015
    #option(CARLSIM_LN_SETUP_NETWORK_MT  "Fix Performance Setup Network (experimental)" OFF)
    option(CARLSIM_LN_GPU_INFO "Enables Control which GPU to be used" ON)  # feature (buggy due dependency) 
	option(CARLSIM_LN_I_CALC_TYPES "Enables enhanced current calculation" ON)  # feature NM4R6 (experimental) )
	option(CARLSIM_LN_GPU_STDP "Enables GPU accelerated functions for STDP" ON)  # feature GPU device functions (experimental) )

	option(CARLSIM_JK_CA3_SSN "J. Kopsick extensions for Neuron-Type Specific CA3 SNN" OFF) # feature ported on basis of CARLsim4 branch

	set(CARLSIM_CUDA_GENCODE "" CACHE STRING "Enforce code generation for specific CUDA archtecture and device, e.g. -gencode arch=compute_86,code=sm_86 for RTX3090, 80 for A100, 61 for Titan Xp" )  
	
	option(CARLSIM_FIX_SYNCTHREADS "Avoid blocking synthread call for Ampere GPUs" ON)  # Ported from CARLsim 4 on A100  

# Dependencies

    if(NOT CARLSIM_NO_CUDA)
        find_package(CUDA 11 REQUIRED)
    endif()

	if(UNIX AND NOT CARLSIM_NO_PTHREADS)
  		set(THREADS_PREFER_PTHREAD_FLAG ON)
		find_package(Threads REQUIRED)
	endif()

# Targets
 
if(CARLSIM_SHARED)
    add_library(carlsim SHARED) 
 
    set(interface_SRCS     
        carlsim/interface/src/callback_core.cpp
        carlsim/interface/src/carlsim.cpp
        carlsim/interface/src/linear_algebra.cpp
        carlsim/interface/src/poisson_rate.cpp
        carlsim/interface/src/user_errors.cpp
    )
    set(interface_HDRS 
        carlsim/interface/inc/callback_core.h
        carlsim/interface/inc/callback.h
        carlsim/interface/inc/carlsim_datastructures.h
        carlsim/interface/inc/carlsim_definitions.h
        carlsim/interface/inc/carlsim.h
        carlsim/interface/inc/carlsim_api.h
        carlsim/interface/inc/carlsim_conf_api.h
        carlsim/interface/inc/carlsim_log_definitions.h
        carlsim/interface/inc/linear_algebra.h
        carlsim/interface/inc/poisson_rate.h
        carlsim/interface/inc/user_errors.h
    )
    set(kernel_SRCS      
        carlsim/kernel/src/print_snn_info.cpp
        carlsim/kernel/src/snn_cpu_module.cpp
        carlsim/kernel/src/snn_manager.cpp
        carlsim/kernel/src/spike_buffer.cpp
    )    
    set(kernel_HDRS
        carlsim/kernel/inc/cuda_version_control.h
        carlsim/kernel/inc/error_code.h
        carlsim/kernel/inc/snn_datastructures.h
        carlsim/kernel/inc/snn_definitions.h
        carlsim/kernel/inc/snn.h
        carlsim/kernel/inc/spike_buffer.h
    )
    set(monitor_SRCS      
        carlsim/monitor/connection_monitor_core.cpp
        carlsim/monitor/connection_monitor.cpp
        carlsim/monitor/group_monitor_core.cpp
        carlsim/monitor/group_monitor.cpp
        carlsim/monitor/neuron_monitor_core.cpp
        carlsim/monitor/neuron_monitor.cpp
        carlsim/monitor/spike_monitor_core.cpp
        carlsim/monitor/spike_monitor.cpp        
    )
    set(monitor_HDRS
        carlsim/monitor/connection_monitor_core.h
        carlsim/monitor/connection_monitor.h
        carlsim/monitor/group_monitor_core.h
        carlsim/monitor/group_monitor.h
        carlsim/monitor/neuron_monitor_core.h
        carlsim/monitor/neuron_monitor.h
        carlsim/monitor/spike_monitor_core.h
        carlsim/monitor/spike_monitor.h
    )
    
    source_group("Header Files\\Interface" FILES ${interface_HDRS})
    source_group("Header Files\\Kernel" FILES ${kernel_HDRS})    
    source_group("Header Files\\Monitor" FILES ${monitor_HDRS})
    
    target_sources(carlsim PRIVATE
        ${interface_SRCS} 
        ${interface_HDRS}
        ${kernel_SRCS} 
        ${kernel_HDRS}
        ${monitor_SRCS} 
        ${monitor_HDRS}
    )

    if(NOT CARLSIM_NO_CUDA)
	
		if(MSVC)
			# 2>cl : Command line warning D9002: ignoring unknown option '-fPIC'
			cuda_add_library(carlsim-cuda
				carlsim/kernel/src/gpu_module/snn_gpu_module.cu
				OPTIONS
					${CARLSIM_CUDA_GENCODE}
			)
		endif()

		if(UNIX)
			cuda_add_library(carlsim-cuda
            carlsim/kernel/src/gpu_module/snn_gpu_module.cu
            OPTIONS
                -Xcompiler
                -fPIC
				${CARLSIM_CUDA_GENCODE}
			)
		endif()
		
		if(NOT CARLSIM_CUDA_GENCODE) 
			set_property(TARGET carlsim-cuda PROPERTY CUDA_ARCHITECTURES all)
		else()
			message("CARLSIM CUDA GENCODE enforced by " ${CARLSIM_CUDA_GENCODE})
		endif()		

    endif()
     

# Properties

    set_property(TARGET carlsim PROPERTY VERSION "6.0.0")
	if(UNIX)
		# Since we build shared library enable position independent code
#		set_property(TARGET carlsim PROPERTY POSITION_INDEPENDENT_CODE TRUE)   
	endif()
	
# Definitions
      
    target_compile_definitions(carlsim PRIVATE CARLSIM_DLL)


    if(CARLSIM_NO_CUDA)
        target_compile_definitions(carlsim
            PUBLIC
                -D__NO_CUDA__
        )
    else()
        target_compile_definitions(carlsim-cuda
            PUBLIC
                -D__CUDA${CUDA_VERSION_MAJOR}__
        )
    endif()

	if(CARLSIM_NO_PTHREADS)
		target_compile_definitions(carlsim PUBLIC __NO_PTHREADS__)         
	endif()

    # Template 
    if(CARLSIM_LN_XYZ)
      target_compile_definitions(carlsim PUBLIC LN_XYZ_CPU)         
      if(NOT CARLSIM_NO_CUDA)
        target_compile_definitions(carlsim PUBLIC LN_XYZ_GPU)         
      endif()
    endif()

    if(CARLSIM_LN_EXT)
        target_compile_definitions(carlsim PUBLIC __LN_EXT__)               # target_precompile_headers 
    endif()

    if(CARLSIM_LN_FIRING)
        target_compile_definitions(carlsim PUBLIC LN_GET_FIRING)            # \sa carlsim.h
        target_compile_definitions(carlsim PUBLIC LN_UPDATE_CURSPIKES)      # \sa snn.h
    endif()

    if(CARLSIM_LN_FIRING_MT)
        target_compile_definitions(carlsim PUBLIC LN_GET_FIRING_MT)         # \sa carlsim.h
        target_compile_definitions(carlsim PUBLIC LN_UPDATE_CURSPIKES_MT)   # \sa snn.h
    endif()

    if(CARLSIM_LN_FIX_DA_DECAY)
      target_compile_definitions(carlsim PUBLIC LN_FIX_DA_DECAY_CPU)         
      if(NOT CARLSIM_NO_CUDA)
        target_compile_definitions(carlsim-cuda PUBLIC LN_FIX_DA_DECAY_GPU)         
      endif()
    endif()

    if(CARLSIM_LN_FIX_ALL_DECAY)
	  target_compile_definitions(carlsim PUBLIC LN_FIX_ALL_DECAY_CPU)
      if(NOT CARLSIM_NO_CUDA)
        target_compile_definitions(carlsim-cuda PUBLIC LN_FIX_ALL_DECAY_GPU)         
      endif()
    endif()

    if(CARLSIM_LN_FIX_CONNLIST)
        target_compile_definitions(carlsim PUBLIC LN_FIX_CONNLIST_)                
    endif()

    if(CARLSIM_LN_SETUP_NETWORK_MT)
      target_compile_definitions(carlsim PUBLIC LN_SETUP_NETWORK_MT)         
    endif()

    if(CARLSIM_LN_GPU_INFO)
      target_compile_definitions(carlsim PUBLIC LN_GPU_INFO)         
    endif()
         
    if(CARLSIM_LN_FIRING_DEBUG)
      #target_compile_definitions(carlsim PUBLIC DEBUG_GET_CURSPIKES2_LN)         
      #target_compile_definitions(carlsim PUBLIC DEBUG_GET_CURSPIKES2_LN_PERF)         
      target_compile_definitions(carlsim PUBLIC DEBUG_GET_CURSPIKES2_LN_MON)         
      target_compile_definitions(carlsim PUBLIC DEBUG_GET_CURSPIKES2_LN_CHECKS)         
    endif()      

    if(CARLSIM_LN_I_CALC_TYPES)
      target_compile_definitions(carlsim PUBLIC LN_I_CALC_TYPES)   
	  if(NOT CARLSIM_NO_CUDA)
		target_compile_definitions(carlsim-cuda PUBLIC LN_I_CALC_TYPES)
	  endif()
    endif()

    if(CARLSIM_LN_GPU_STDP)
	  if(NOT CARLSIM_NO_CUDA)
		target_compile_definitions(carlsim-cuda PUBLIC LN_GPU_STDP)
	  endif()
    endif()

	if(CARLSIM_JK_CA3_SSN) 
		target_compile_definitions(carlsim PUBLIC JK_CA3_SNN) 
		if(NOT CARLSIM_NO_CUDA)
			target_compile_definitions(carlsim-cuda PUBLIC JK_CA3_SNN)
		endif()
	endif()

	if(CARLSIM_FIX_SYNCTHREADS)
      if(NOT CARLSIM_NO_CUDA)
        target_compile_definitions(carlsim-cuda PUBLIC CS4_FIX_SYNCTHREADS)         
      endif()
    endif()
	

# Includes

    target_include_directories(carlsim
        PUBLIC
            carlsim/interface/inc
            carlsim/kernel/inc
            carlsim/monitor
    )

    if(NOT CARLSIM_NO_CUDA)
        target_include_directories(carlsim-cuda
            PUBLIC
                carlsim/kernel/inc
                carlsim/interface/inc
                carlsim/monitor
                ${CUDA_INCLUDE_DIRS}
                ${CUDA_TOOLKIT_ROOT_DIR}/samples/common/inc
          )
		if(MSVC)
			target_include_directories(carlsim-cuda
				PUBLIC
					"C:/ProgramData/NVIDIA Corporation/CUDA Samples/v${CUDA_VERSION_STRING}/common/inc"            
			)
		endif()
    endif()


# Linking 

    if(NOT CARLSIM_NO_CUDA)
        target_link_libraries(carlsim
            PUBLIC
               carlsim-cuda
                ${CUDA_curand_LIBRARY}
        )
    endif()

	if(UNIX AND NOT CARLSIM_NO_PTHREADS)
		target_link_libraries(carlsim PRIVATE Threads::Threads)
	endif()
	

# Installation

    #install(TARGETS carlsim DESTINATION bin)

    configure_file(cmake/carlsim-config.cmake.in cmake/carlsim-config.cmake @ONLY)  #TODO CMAKE_BUILD_DIR, see FinSpikes
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/carlsim-config.cmake DESTINATION lib/cmake/carlsim)

    configure_file(carlsim/interface/inc/carlsim_conf.h.in ${CMAKE_CURRENT_BINARY_DIR}/carlsim/interface/inc/carlsim_conf.h @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/carlsim/interface/inc/carlsim_conf.h DESTINATION include )

    set_property(TARGET carlsim PROPERTY PUBLIC_HEADER 
            ${interface_HDRS} 
            ${kernel_HDRS} 
            ${monitor_HDRS} 
            )

    install(TARGETS carlsim
        EXPORT carlsim
        RUNTIME 
            DESTINATION bin 
            COMPONENT carlsim
        LIBRARY
            DESTINATION lib
            COMPONENT carlsim
        ARCHIVE
            DESTINATION lib
            COMPONENT carlsim
        PUBLIC_HEADER
            DESTINATION include
            COMPONENT carlsim
     )    
        
    # unmanaged symbol
    # https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#imported-targets

    #add_library(carlsim STATIC carlsim/dummy.cpp )

endif()

# Subdirectories

  add_subdirectory(carlsim)
   
  if(CARLSIM_TOOLS)
	add_subdirectory(tools)
  endif()
   
  if(CARLSIM_SAMPLES)
	add_subdirectory(projects)
  endif()
  
  if(CARLSIM_PYCARL)
    add_subdirectory(pycarl)   
  endif()  

if(CARLSIM_TUTORIALS)  
  add_subdirectory(doc)
endif()
