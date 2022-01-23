project(benchmark4)

# Targets  
add_executable(benchmark4
    src/main_benchmark4.cpp
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(benchmark4 PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(benchmark4 PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(benchmark4 PRIVATE
    ../../tools/stopwatch
)
   
        
# Installation   
install(TARGETS benchmark4
    EXPORT carlsim
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark4 
        COMPONENT benchmark4
        )

install(FILES run_benchmark4 DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark4)          