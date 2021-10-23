project(basic_concepts)

# Targets  
add_executable(basic_concepts
    src/main_basic_concepts.cpp
)
set_property(TARGET basic_concepts PROPERTY PROJECT_LABEL tutorial_1_basic_concepts)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(basic_concepts PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(basic_concepts PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(basic_concepts PRIVATE
    ../../tools/stopwatch
)
           
# Installation  
 

install(TARGETS basic_concepts
        EXPORT basic_concepts
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/1_basic_concepts 
        COMPONENT basic_concepts)

install(DIRECTORY scripts 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/1_basic_concepts )
           