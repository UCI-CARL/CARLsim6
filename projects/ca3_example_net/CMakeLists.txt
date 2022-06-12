project(ca3_snn_GPU)
cmake_minimum_required(VERSION 3.22)

FIND_PACKAGE (Boost REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# uncomment for standalone builds 
#FIND_PACKAGE (carlsim REQUIRED)

# Headers
    set(HDRS
        generateSETUPStateSTP.h
        generateCONFIGStateSTP.h 
    )

# Targets  
add_executable(ca3_snn_GPU
    ${HDRS}
    src/main_ca3_snn_GPU.cpp
    )

source_group("Header Files" FILES ${HDRS})

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(ca3_snn_GPU PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(ca3_snn_GPU PRIVATE

    carlsim
    carlsim-stopwatch
)
target_include_directories(ca3_snn_GPU PRIVATE
    ../../tools/stopwatch
)

target_link_directories(ca3_snn_GPU PRIVATE        
    ${CARLSIM_LIB_DIR}
    )
      
# Installation   
install(TARGETS ca3_snn_GPU
    EXPORT ca3_snn_GPU
    RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>samples
        COMPONENT ca3_snn_GPU
        )

#install(FILES run_ca3_snn_GPU DESTINATION $<$<CONFIG:Debug>:debug/>samples)          