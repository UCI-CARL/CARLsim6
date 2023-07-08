# Targets

    if(CARLSIM_STATIC)
        add_library(carlsim-visual-stimulus STATIC)
    else()
        add_library(carlsim-visual-stimulus)
    endif()

    target_compile_definitions(carlsim-visual-stimulus PRIVATE CARLSIM_DLL)

    
    set(SRCS visual_stimulus.cpp)
    set(HDRS visual_stimulus.h)

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
                 PREFIX "Header Files"
                 FILES ${HDRS})

    target_sources(carlsim-visual-stimulus PRIVATE ${SRCS} 
                                             ${HDRS})

# Properties

    # Since we build shared library enable position independent code
    set_property(TARGET carlsim-visual-stimulus PROPERTY
        POSITION_INDEPENDENT_CODE TRUE)

# Linking

    if(CARLSIM_STATIC)
        target_link_libraries(carlsim-visual-stimulus PRIVATE -static carlsim)
    else()
        target_link_libraries(carlsim-visual-stimulus PRIVATE carlsim)
    endif()

# Installation

    set_property(TARGET carlsim-visual-stimulus PROPERTY PUBLIC_HEADER ${HDRS})

    install(TARGETS carlsim-visual-stimulus 
        EXPORT carlsim-visual-stimulus
        RUNTIME 
            DESTINATION bin 
            COMPONENT carlsim-visual-stimulus
        LIBRARY
            DESTINATION lib
            COMPONENT carlsim-visual-stimulus
        ARCHIVE
            DESTINATION lib
            COMPONENT carlsim-visual-stimulus
        PUBLIC_HEADER
            DESTINATION include
            COMPONENT carlsim-visual-stimulus          
     )
