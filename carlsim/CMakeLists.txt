

# Includes
    include_directories(interface/inc)
    include_directories(monitor)

# Subdirectories

if(CARLSIM_STATIC)
    add_subdirectory(interface)
    add_subdirectory(kernel)
    add_subdirectory(monitor)
endif()

if(CARLSIM_TEST)    
    add_subdirectory(test)
    add_subdirectory(testadv)
    add_subdirectory(test6)
endif()

# add benchmark as embedded sub project
# \sa standalone sub folder
if(CARLSIM_BENCHMARKS)    
    add_subdirectory(benchmark)
endif()    