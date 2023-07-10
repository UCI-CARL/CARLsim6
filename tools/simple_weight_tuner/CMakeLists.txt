
# Targets

    add_library(carlsim-simple-weight-tuner SHARED)

    target_compile_definitions(carlsim-simple-weight-tuner PRIVATE CARLSIM_DLL)


    set(SRCS simple_weight_tuner.cpp)
    set(HDRS simple_weight_tuner.h)

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
                 PREFIX "Header Files"
                 FILES ${HDRS})

    target_sources(carlsim-simple-weight-tuner PRIVATE ${SRCS} 
                                                       ${HDRS})       

# Properties

    # Since we build shared library enable position independent code
    set_property(TARGET carlsim-simple-weight-tuner PROPERTY
        POSITION_INDEPENDENT_CODE TRUE)

# Linking

    target_link_libraries(carlsim-simple-weight-tuner PRIVATE carlsim)

# Installation

    install(FILES DESTINATION include)

# LN
    set_property(TARGET carlsim-simple-weight-tuner PROPERTY PUBLIC_HEADER ${HDRS})

    install(TARGETS carlsim-simple-weight-tuner 
        EXPORT carlsim-simple-weight-tuner
        RUNTIME 
            DESTINATION bin 
            COMPONENT carlsim-simple-weight-tuner
        LIBRARY
            DESTINATION lib
            COMPONENT carlsim-simple-weight-tuner
        ARCHIVE
            DESTINATION lib
            COMPONENT carlsim-simple-weight-tuner
        PUBLIC_HEADER
            DESTINATION include
            COMPONENT carlsim-simple-weight-tuner          
     )