project(benchmarkA)

# Targets  
add_executable(benchmarkA
    src/main_benchmarkA.cpp
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(benchmarkA PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(benchmarkA PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(benchmarkA PRIVATE
    ../../tools/stopwatch
)
           
# Installation   
install(TARGETS benchmarkA
    EXPORT benchmarkA
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmarkA 
        COMPONENT benchmarkA
        )

install(FILES run_benchmarkA_hpc
              run_benchmarkA_sdsc
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmarkA)
        
install(DIRECTORY scriptsA 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmarkA)
           