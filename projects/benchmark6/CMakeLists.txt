project(benchmark6)

# Targets  
add_executable(benchmark6
    src/main_benchmark6.cpp
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(benchmark6 PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(benchmark6 PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(benchmark6 PRIVATE
    ../../tools/stopwatch
)
           
# Installation   
install(TARGETS benchmark6
    EXPORT carlsim
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark6 
        COMPONENT benchmark6
        )

install(FILES run_benchmark6
              run_benchmark6.psh
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/benchmark6)          