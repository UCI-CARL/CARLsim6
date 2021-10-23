project(random_spnet)

# Targets  
add_executable(random_spnet
    src/main_random_spnet.cpp
)

# https://cmake.org/pipermail/cmake/2012-May/050269.html
set_property(TARGET random_spnet PROPERTY PROJECT_LABEL tutorial_2_random_spnet)

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
           