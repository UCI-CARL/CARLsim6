project(basic_concepts)

# Targets  
add_executable(basic_concepts
    src/main_basic_concepts.cpp
)
set_property(TARGET basic_concepts PROPERTY PROJECT_LABEL tutorial_1_basic_concepts)

set(scripts_OAT      
	scripts/initOAT.m
	scripts/demoOAT.m
	scripts/ffcurve.m
)
source_group("Scripts\\OAT" FILES ${scripts_OAT})     
add_custom_target(basic_concepts_scripts
	SOURCES ${scripts_OAT}
)
set_property(TARGET basic_concepts_scripts PROPERTY PROJECT_LABEL tutorial_1_basic_concepts_scripts)

add_dependencies(basic_concepts basic_concepts_scripts)

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
           