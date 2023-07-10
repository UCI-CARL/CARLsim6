# Targets

    add_library(carlsim-spike-generators SHARED)

    target_compile_definitions(carlsim-spike-generators PRIVATE CARLSIM_DLL)
    
    set(SRCS    interactive_spikegen.cpp
                periodic_spikegen.cpp
                pre_post_group_spikegen.cpp
                spikegen_from_file.cpp
                spikegen_from_vector.cpp)

    set(HDRS    interactive_spikegen.h
                periodic_spikegen.h
                pre_post_group_spikegen.h
                spikegen_from_file.h
                spikegen_from_vector.h )

    target_sources(carlsim-spike-generators  PRIVATE ${SRCS} 
                                             ${HDRS})

# Properties

    # Since we build shared library enable position independent code
    set_property(TARGET carlsim-spike-generators PROPERTY
        POSITION_INDEPENDENT_CODE TRUE)

# Includes

    target_include_directories(carlsim-spike-generators
        PUBLIC
            .
    )

# Linking

    target_link_libraries(carlsim-spike-generators PUBLIC carlsim)

# Installation

  set_property(TARGET carlsim-spike-generators  PROPERTY PUBLIC_HEADER ${HDRS})

    install(TARGETS carlsim-spike-generators  
        EXPORT carlsim-spike-generators 
        RUNTIME 
            DESTINATION bin 
            COMPONENT carlsim-spike-generators 
        LIBRARY
            DESTINATION lib
            COMPONENT carlsim-spike-generators 
        ARCHIVE
            DESTINATION lib
            COMPONENT carlsim-spike-generators 
        PUBLIC_HEADER
            DESTINATION include
            COMPONENT carlsim-spike-generators           
     )