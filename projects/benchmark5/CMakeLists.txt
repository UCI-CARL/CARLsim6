project(benchmark5)

# Targets  
add_executable(benchmark5
    src/main_benchmark5.cpp
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(benchmark5 PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(benchmark5 PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(benchmark5 PRIVATE
    ../../tools/stopwatch
)
   
        
# Installation   
install(TARGETS benchmark5
    EXPORT carlsim
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark5 
        COMPONENT benchmark5
        )

install(FILES run_benchmark5
              run_benchmark5_comet
              run_benchmark5_hpc
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark5)          