project(benchmarkB)

# Targets  
add_executable(benchmarkB
    src/main_benchmarkB.cpp
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(benchmarkB PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(benchmarkB PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(benchmarkB PRIVATE
    ../../tools/stopwatch
)
           
# Installation   
install(TARGETS benchmarkB
    EXPORT benchmarkB
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmarkB 
        COMPONENT benchmarkB
        )

install(FILES run_benchmarkB_hpc
              run_benchmarkB_sdsc
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmarkB)    

install(DIRECTORY scriptsB 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmarkB)              