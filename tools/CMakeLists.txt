# Subdirectories

    if(CARLSIM_SHARED)
      add_subdirectory(spike_generators)
      add_subdirectory(stopwatch)
    endif()
    
    if(CARLSIM_TOOLS)
      add_subdirectory(simple_weight_tuner)
      add_subdirectory(offline_analysis_toolbox)
      add_subdirectory(pti)
    endif()
    
    if(CARLSIM_TOOLS OR CARLSIM_PYCARL)
      add_subdirectory(visual_stimulus)
    endif()