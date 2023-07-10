
find_package(Python3 COMPONENTS Interpreter Development)
if(NOT Python3_INCLUDE_DIRS OR NOT Python3_LIBRARY_DIRS)
	message(FATAL_ERROR "Python3 required, CMake will exit.")
endif()

find_package(SWIG 4.0 COMPONENTS python) 
include(UseSWIG)

set_property(SOURCE source/carlsim.i PROPERTY CPLUSPLUS ON)

swig_add_library(pycarl 
        TYPE SHARED
        LANGUAGE python 
        SOURCES source/carlsim.i )

 target_include_directories(pycarl PRIVATE ${Python3_INCLUDE_DIRS})

swig_link_libraries(pycarl PRIVATE 
	python3
    carlsim
    carlsim-visual-stimulus
    carlsim-spike-generators
)

target_link_directories(pycarl PRIVATE ${Python3_LIBRARY_DIRS})

