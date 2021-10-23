project(compartments)

# Targets  
add_executable(compartments
    src/main_compartments.cpp
)
set_property(TARGET compartments PROPERTY PROJECT_LABEL tutorial_8_compartments)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(compartments PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(compartments PRIVATE
    carlsim
)

          
# Installation  
 
install(TARGETS compartments
        EXPORT compartments
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/8_compartments 
        COMPONENT compartments)

           