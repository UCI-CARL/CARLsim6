# Targets

    add_library(carlsim-pti SHARED)

    target_compile_definitions(carlsim-pti PRIVATE CARLSIM_DLL)
    
    set(SRCS    PTI.cpp
                Util.cpp
                ParameterInstances.cpp
                Logger.cpp )

    set(HDRS    carlsim_pti_api.h
				PTI.h				
                Util.h
                ParameterInstances.h
                Logger.h
                Experiment.h )

    target_sources(carlsim-pti  PRIVATE ${SRCS} 
                                        ${HDRS})

# Properties

    # Since we build shared library enable position independent code
    set_property(TARGET carlsim-pti PROPERTY
        POSITION_INDEPENDENT_CODE TRUE)

# Includes

    target_include_directories(carlsim-pti
        PUBLIC
            .
    )

# Linking

    target_link_libraries(carlsim-pti PUBLIC carlsim)

# Installation

  set_property(TARGET carlsim-pti  PROPERTY PUBLIC_HEADER ${HDRS})

    install(TARGETS carlsim-pti  
        EXPORT carlsim-pti 
        RUNTIME 
            DESTINATION bin 
            COMPONENT carlsim-pti 
        LIBRARY
            DESTINATION lib
            COMPONENT carlsim-pti 
        ARCHIVE
            DESTINATION lib
            COMPONENT carlsim-pti 
        PUBLIC_HEADER
            DESTINATION include
            COMPONENT carlsim-pti           
     )