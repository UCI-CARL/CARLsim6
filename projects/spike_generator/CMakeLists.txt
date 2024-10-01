project(spike_generator)

# Targets  
add_executable(spike_generator
    src/main_spike_generator.cpp
)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(spike_generator PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(spike_generator PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(spike_generator PRIVATE
    ../../tools/stopwatch
)
   
        
# Installation   
install(TARGETS spike_generator
    EXPORT carlsim
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples 
        COMPONENT spike_generator
        )

#install(FILES run_hello_world DESTINATION $<$<CONFIG:Debug>:debug/>samples)          