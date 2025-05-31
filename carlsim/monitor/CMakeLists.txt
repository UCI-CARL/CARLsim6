# Targets

    add_library(carlsim-monitor
        coba_monitor_core.cpp
        coba_monitor.cpp
        connection_monitor_core.cpp
        connection_monitor.cpp
        group_monitor_core.cpp
        group_monitor.cpp
        neuron_monitor_core.cpp
        neuron_monitor.cpp
        performance_monitor_core.cpp
        performance_monitor.cpp
        spike_monitor_core.cpp
        spike_monitor.cpp
    )

# Properties

    # Since we build shared library enable position independent code
    set_property(TARGET carlsim-monitor PROPERTY
        POSITION_INDEPENDENT_CODE TRUE)

# Includes

    target_include_directories(carlsim-monitor
        PUBLIC
            .
    )

# Linking

    target_link_libraries(carlsim-monitor
        PRIVATE
            carlsim-kernel
            carlsim-interface
    )

# Installation

    install(
        FILES
            coba_monitor_core.h
            coba_monitor.h
            connection_monitor_core.h
            connection_monitor.h
            group_monitor_core.h
            group_monitor.h
            performance_monitor_core.h
            performance_monitor.h
            neuron_monitor_core.h
            neuron_monitor.h
            spike_monitor_core.h
            spike_monitor.h
        DESTINATION include)

# LN
    install(TARGETS carlsim-monitor DESTINATION lib)