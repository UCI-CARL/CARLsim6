project(paramter_tuning_interface)

# Targets  
add_executable(TuneFiringRates
    src/main_TuneFiringRates.cpp
)
set_property(TARGET TuneFiringRates PROPERTY PROJECT_LABEL tutorial_7_parameter_tuning_interface)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(TuneFiringRates PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(TuneFiringRates PRIVATE
    carlsim
    carlsim-simple-weight-tuner
    carlsim-spike-generators
	carlsim-pti
)
target_include_directories(TuneFiringRates PRIVATE
    ../../../../tools/simple_weight_tuner
    ../../../../tools/spike_generators
    ../../../../tools/pti
)

# Installation  
install(TARGETS TuneFiringRates
        EXPORT TuneFiringRates
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/7_parameter_tuning_interface 
        COMPONENT TuneFiringRates)

install(FILES src/leap_TuneFiringRates.py
		DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/7_parameter_tuning_interface )