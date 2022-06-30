project(oat)

find_package(Matlab)

set(scripts_OAT      
  ConnectionMonitor.m
  ConnectionReader.m
  GroupMonitor.m
  GroupReader.m
  NetworkMonitor.m
  NeuronMonitor.m
  NeuronReader.m
  SimulationReader.m
  SpikeReader.m
  Utilities.m
)
source_group("Matlab Source\\OAT" FILES ${scripts_OAT})    

# Targets  
add_custom_target(oat
	SOURCES ${scripts_OAT}
)
set_property(TARGET oat PROPERTY PROJECT_LABEL offline-analyis-toolboox)

           
# Installation          
install(FILES ${scripts_OAT}
     DESTINATION $<$<CONFIG:Debug>:debug/>tools/offline_analysis_toolbox )
           