project(simple_weight_tuner)

# Targets  
add_executable(simple_weight_tuner
    src/main_simple_weight_tuner.cpp
)
set_property(TARGET simple_weight_tuner PROPERTY PROJECT_LABEL tutorial_6_simple_weight_tuner)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(simple_weight_tuner PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(simple_weight_tuner PRIVATE
    carlsim
    carlsim-simple-weight-tuner
    carlsim-spike-generators
)
target_include_directories(simple_weight_tuner PRIVATE
    ../../../../tools/simple_weight_tuner
    ../../../../tools/spike_generators
)

# Installation  
install(TARGETS simple_weight_tuner
        EXPORT simple_weight_tuner
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/6_simple_weight_tuner 
        COMPONENT simple_weight_tuner)
