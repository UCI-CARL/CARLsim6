project(benchmark3)

# Targets  
add_executable(benchmark3
    src/main_benchmark3.cpp
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(benchmark3 PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(benchmark3 PRIVATE
    carlsim
    carlsim-stopwatch
    carlsim-simple-weight-tuner
)
target_include_directories(benchmark3 PRIVATE
    ../../tools/stopwatch
    ../../tools/simple_weight_tuner
)
           
# Installation   
install(TARGETS benchmark3
    EXPORT benchmark3
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark3 
        COMPONENT benchmark3
        )
        
install(FILES 
        run_benchmark3 
        run_benchmark3.ps1 
        fireTarget3 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark3)
