project(benchmark2)

# Targets  
add_executable(benchmark2
    src/main_benchmark2.cpp
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(benchmark2 PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(benchmark2 PRIVATE
    carlsim
    carlsim-stopwatch
    carlsim-simple-weight-tuner
)
target_include_directories(benchmark2 PRIVATE
    ../../tools/stopwatch
    ../../tools/simple_weight_tuner
)
           
# Installation   
install(TARGETS benchmark2
    EXPORT benchmark2
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark2 
        COMPONENT benchmark2
        )
        
install(FILES run_benchmark2
              fireTarget2 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark2)        