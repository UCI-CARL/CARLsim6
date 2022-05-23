project(ca3_snn_GPU_example)

FIND_PACKAGE (Boost REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# Headers
    set(HDRS
        generateSETUPStateSTP.h
        generateCONFIGStateSTP.h 
	)

# Targets  
add_executable(ca3_snn_GPU_example
	${HDRS}
    src/main_ca3_snn_GPU.cpp
    )

source_group("Header Files" FILES ${HDRS})
# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(ca3_snn_GPU_example PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(ca3_snn_GPU_example PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(ca3_snn_GPU_example PRIVATE
    ../../tools/stopwatch
)
   
        
# Installation   
install(TARGETS ca3_snn_GPU_example
    EXPORT carlsim
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples 
        COMPONENT ca3_snn_GPU_example
        )

#install(FILES run_hello_world DESTINATION $<$<CONFIG:Debug>:debug/>samples)          
