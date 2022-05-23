project(ca3_snn_GPU_sync_arch)

FIND_PACKAGE (Boost REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# Headers
    set(HDRS
        generateSETUPStateSTP.h
        generateCONFIGStateSTP.h 
	)

# Targets  
add_executable(ca3_snn_GPU_sync_arch
	${HDRS}
    src/main_ca3_snn_GPU.cpp
    )

source_group("Header Files" FILES ${HDRS})
# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(ca3_snn_GPU_sync_arch PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(ca3_snn_GPU_sync_arch PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(ca3_snn_GPU_sync_arch PRIVATE
    ../../../tools/stopwatch
)
   
        
# Installation   
install(TARGETS ca3_snn_GPU_sync_arch
    EXPORT carlsim
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples 
        COMPONENT ca3_snn_GPU_sync_arch
        )

#install(FILES run_hello_world DESTINATION $<$<CONFIG:Debug>:debug/>samples)          
