project(random_spnet)

# Targets  
add_executable(random_spnet
    src/main_random_spnet.cpp
)
# https://cmake.org/pipermail/cmake/2012-May/050269.html
set_property(TARGET random_spnet PROPERTY PROJECT_LABEL tutorial_2_random_spnet)


set(scripts_OAT      
	scripts/initOAT.m
	scripts/demoOAT.m
)
source_group("Scripts\\OAT" FILES ${scripts_OAT})     
add_custom_target(random_spnet_scripts
	SOURCES ${scripts_OAT}
)
set_property(TARGET random_spnet_scripts PROPERTY PROJECT_LABEL tutorial_2_random_spnet_scripts)

add_dependencies(random_spnet random_spnet_scripts)


# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(random_spnet PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(random_spnet PRIVATE
    carlsim
    carlsim-stopwatch
)
target_include_directories(random_spnet PRIVATE
    ../../tools/stopwatch
)
           
# Installation  
install(TARGETS random_spnet
        EXPORT random_spnet
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/2_random_spnet 
        COMPONENT random_spnet)

install(DIRECTORY scripts 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/2_random_spnet )
           