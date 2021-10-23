project(image_processing)

# Targets  
add_executable(image_processing_dog
    src/main_dog.cpp
)
set_property(TARGET image_processing_dog PROPERTY PROJECT_LABEL tutorial_4_image_processing_dog)

add_executable(image_processing_smooth
    src/main_smooth.cpp
)
set_property(TARGET image_processing_smooth PROPERTY PROJECT_LABEL tutorial_4_image_processing_smooth)

# definitions     
if(CARLSIM_NO_CUDA)
    target_compile_definitions(image_processing_dog PUBLIC
        -D__NO_CUDA__ )
    target_compile_definitions(image_processing_smooth PUBLIC
        -D__NO_CUDA__ )
endif()

# Linking    
target_link_libraries(image_processing_dog PRIVATE
    carlsim
    carlsim-visual-stimulus
)
target_include_directories(image_processing_dog PRIVATE
    ../../../../tools/visual_stimulus
)

target_link_libraries(image_processing_smooth PRIVATE
    carlsim
    carlsim-visual-stimulus
)
target_include_directories(image_processing_smooth PRIVATE
    ../../../../tools/visual_stimulus
)
           
# Installation  
 
install(TARGETS image_processing_dog
        EXPORT image_processing_dog
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/4_image_processing 
        COMPONENT image_processing_dog)

install(TARGETS image_processing_smooth
        EXPORT image_processing_smooth
        RUNTIME 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/4_image_processing 
        COMPONENT image_processing_smooth)

install(DIRECTORY scripts 
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/4_image_processing )

install(DIRECTORY input
        DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/4_image_processing )
           