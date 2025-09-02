project(synfire)

# Targets  
add_executable(synfire
    src/synfire_network.cpp
)
set_property(TARGET synfire PROPERTY PROJECT_LABEL tutorial_10_synfire)

set(scripts_OAT      
	scripts/initOAT.m
	scripts/demoOAT.m
	#scripts/NormalDistributedStimulusPackage.m
)
	
source_group("Scripts\\OAT" FILES ${scripts_OAT})     
add_custom_target(synfire_scripts
	SOURCES ${scripts_OAT}
)
set_property(TARGET synfire_scripts PROPERTY PROJECT_LABEL tutorial_10_synfire_scripts)

add_dependencies(synfire synfire_scripts)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(synfire PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(synfire PRIVATE
    carlsim
    carlsim-stopwatch
    carlsim-spike-generators
)
target_include_directories(synfire PRIVATE
    ../../tools/stopwatch
    ../../tools/spike_generators
)
           
           
# Installation   
install(TARGETS synfire
        EXPORT synfire
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/10_synfire
        COMPONENT synfire)

install(DIRECTORY scripts 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/10_synfire )
           