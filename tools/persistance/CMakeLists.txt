
# Targets

    if(CARLSIM_STATIC)
        add_library(carlsim-persistance STATIC)
    else()
        add_library(carlsim-persistance SHARED)
    endif()

    target_compile_definitions(carlsim-persistance PRIVATE CARLSIM_DLL)

    set(HDRS carlsim_persistance_api.h
	     file_connection_generator.h
             )

    set(SRCS file_connection_generator.cpp)

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
                 PREFIX "Header Files"
                 FILES ${HDRS})

    target_sources(carlsim-persistance PRIVATE ${SRCS} 
                                             ${HDRS})

# Linking

    if(CARLSIM_STATIC)
        target_link_libraries(carlsim-persistance PRIVATE -static carlsim)
    else()
        target_link_libraries(carlsim-persistance PRIVATE carlsim)
    endif()


# Installation

    set_property(TARGET carlsim-persistance PROPERTY PUBLIC_HEADER ${HDRS})

    install(TARGETS carlsim-persistance 
        EXPORT carlsim-persistance
        RUNTIME 
            DESTINATION bin 
            COMPONENT carlsim-persistance
        LIBRARY
            DESTINATION lib
            COMPONENT carlsim-persistance
        ARCHIVE
            DESTINATION lib
            COMPONENT carlsim-persistance
        PUBLIC_HEADER
            DESTINATION include
            COMPONENT carlsim-persistance          
     )
