project(ca3_snn_GPU)

FIND_PACKAGE (Boost REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# Targets  
add_executable(ca3_snn_GPU
    src/main_ca3_snn_GPU.cpp
    )

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(ca3_snn_GPU PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(ca3_snn_GPU PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(ca3_snn_GPU PRIVATE
    ../../tools/stopwatch
)
   
        
# Installation   
install(TARGETS ca3_snn_GPU
    EXPORT carlsim
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples 
        COMPONENT ca3_snn_GPU
        )

#install(FILES run_hello_world DESTINATION $<$<CONFIG:Debug>:debug/>samples)          