project(plasticity)

# Targets  
add_executable(plasticity
    src/main_plasticity.cpp
)
set_property(TARGET plasticity PROPERTY PROJECT_LABEL tutorial_3_plasticity)

set(scripts_OAT      
	scripts/initOAT.m
	scripts/plasticityOAT.m
)
source_group("Scripts\\OAT" FILES ${scripts_OAT})     
add_custom_target(plasticity_scripts
	SOURCES ${scripts_OAT}
)
set_property(TARGET plasticity_scripts PROPERTY PROJECT_LABEL tutorial_3_plasticity_scripts)

add_dependencies(plasticity plasticity_scripts)


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
           