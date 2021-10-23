project(plasticity)

# Targets  
add_executable(plasticity
    src/main_plasticity.cpp
)
set_property(TARGET plasticity PROPERTY PROJECT_LABEL tutorial_3_plasticity)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(plasticity PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(plasticity PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(plasticity PRIVATE
    ../../tools/stopwatch
)
           
# Installation  
 
install(TARGETS plasticity
        EXPORT plasticity
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/3_plasticity 
        COMPONENT plasticity)

install(DIRECTORY scripts 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/3_plasticity )
           