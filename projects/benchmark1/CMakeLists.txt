project(benchmark1)

# Targets  
add_executable(benchmark1
    src/main_benchmark1.cpp
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(benchmark1 PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(benchmark1 PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(benchmark1 PRIVATE
    ../../tools/stopwatch
)
   
        
# Installation   
install(TARGETS benchmark1
    EXPORT carlsim
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark1 
        COMPONENT benchmark1
        )

install(FILES run_benchmark1 DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark1 )          