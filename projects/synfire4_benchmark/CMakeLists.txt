project(synfire4)

# Targets  
add_executable(synfire4
    synfire4loopB/main.cpp

)
set_property(TARGET synfire4 PROPERTY PROJECT_LABEL synfire4_benchmark)


# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(synfire4 PUBLIC
        -D__NO_CUDA__ )
endif()


# Linking    
target_link_libraries(synfire4 PRIVATE
    carlsim
    carlsim-stopwatch
    carlsim-spike-generators
	carlsim-persistance
)

target_include_directories(synfire4 PRIVATE
    ../../tools/stopwatch
    ../../tools/spike_generators
    ../../tools/persistance
)
           
           
# Installation   
install(TARGETS synfire4
        EXPORT synfire4
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/synfire4
        COMPONENT synfire4)

install(DIRECTORY scripts 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/synfire4 )
           
install(DIRECTORY synfire4loopB
        DESTINATION $<$<CONFIG:Debug>:debug/>samples/synfire4 )
