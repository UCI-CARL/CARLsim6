
# Targets

    if(CARLSIM_STATIC)
        add_library(carlsim-stopwatch STATIC)
    else()
        add_library(carlsim-stopwatch)
    endif()

    target_compile_definitions(carlsim-stopwatch PRIVATE CARLSIM_DLL)

    set(SRCS stopwatch.cpp)
    set(HDRS stopwatch.h
             carlsim_stopwatch_api.h)

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
                 PREFIX "Header Files"
                 FILES ${HDRS})

    target_sources(carlsim-stopwatch PRIVATE ${SRCS} 
                                             ${HDRS})

# Properties

#    # Since we build shared library enable position independent code
#    set_property(TARGET carlsim-stopwatch PROPERTY
#        POSITION_INDEPENDENT_CODE TRUE)

# Linking

#target_include_directories(carlsim-stopwatch PRIVATE ../../carlsim/interface/inc)

    if(CARLSIM_STATIC)
        target_link_libraries(carlsim-stopwatch PRIVATE -static carlsim)
    else()
        target_link_libraries(carlsim-stopwatch PRIVATE carlsim)
    endif()

# Installation

    set_property(TARGET carlsim-stopwatch PROPERTY PUBLIC_HEADER ${HDRS})

    install(TARGETS carlsim-stopwatch 
        EXPORT carlsim-stopwatch
        RUNTIME 
            DESTINATION bin 
            COMPONENT carlsim-stopwatch
        LIBRARY
            DESTINATION lib
            COMPONENT carlsim-stopwatch
        ARCHIVE
            DESTINATION lib
            COMPONENT carlsim-stopwatch
        PUBLIC_HEADER
            DESTINATION include
            COMPONENT carlsim-stopwatch          
     )
