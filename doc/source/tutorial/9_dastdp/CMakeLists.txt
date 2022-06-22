project(dastdp)

# Targets  
add_executable(dastdp
    src/main_dastdp.cpp
)
set_property(TARGET dastdp PROPERTY PROJECT_LABEL tutorial_9_dastdp)

set(scripts_OAT      
	scripts/initOAT.m
	scripts/dastdpOAT.m
)
source_group("Scripts\\OAT" FILES ${scripts_OAT})     
add_custom_target(dastdp_scripts
	SOURCES ${scripts_OAT}
)
set_property(TARGET dastdp_scripts PROPERTY PROJECT_LABEL tutorial_9_dastdp_scripts)

add_dependencies(dastdp dastdp_scripts)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(dastdp PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(dastdp PRIVATE
    carlsim
    carlsim-spike-generators
)
target_include_directories(dastdp PRIVATE
    ../../tools/spike_generators
)
           
# Installation   
install(TARGETS dastdp
        EXPORT dastdp
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/9_dastdp 
        COMPONENT dastdp)

install(DIRECTORY scripts 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/9_dastdp )
           