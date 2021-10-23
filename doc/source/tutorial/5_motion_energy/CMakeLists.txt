project(motion_energy)

# Dependents
# https://github.com/UCI-CARL/MotionEnergy/releases
# -> feature cMake, contrib, ... mbeyerler did the support and upgrade to windows!!!


# Targets  
add_executable(motion_energy
    src/main_motion_energy.cpp
)
set_property(TARGET motion_energy PROPERTY PROJECT_LABEL tutorial_5_motion_energy)


# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(motion_energy PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(motion_energy PRIVATE
    carlsim
    carlsim-visual-stimulus
)
target_include_directories(motion_energy PRIVATE
    ../../../../tools/visual_stimulus
)

          
# Installation  
install(TARGETS motion_energy
        EXPORT motion_energy
        RUNTIME 
        DESTINATION tutorials$<$<CONFIG:Debug>:_debug>/5_motion_energy 
        COMPONENT motion_energy)

install(DIRECTORY scripts 
        DESTINATION tutorials$<$<CONFIG:Debug>:_debug>/4_motion_energy )

install(DIRECTORY input
        DESTINATION tutorials$<$<CONFIG:Debug>:_debug>/4_motion_energy )
           