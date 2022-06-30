project(motion_energy)

# depends on external project
# https://github.com/UCI-CARL/MotionEnergy

if(NOT CARLSIM_NO_CUDA)

	# Targets  
	add_executable(motion_energy
		src/main_motion_energy.cpp
	)
	set_property(TARGET motion_energy PROPERTY PROJECT_LABEL tutorial_5_motion_energy)

	set(scripts_OAT      
		scripts/initOAT.m
		scripts/demoOAT.m
		scripts/createGratingVideo.m
	)
	source_group("Scripts\\OAT" FILES ${scripts_OAT})     
	add_custom_target(motion_energy_scripts
		SOURCES ${scripts_OAT}
	)
	set_property(TARGET motion_energy_scripts PROPERTY PROJECT_LABEL tutorial_5_motion_energy_scripts)

	if(MSVC)
		# 2>cl : Command line warning D9002: ignoring unknown option '-fPIC'
		cuda_add_library(carlsim-motion-energy
			../../../../external/MotionEnergy/cppME/cppME/src/motion_energy.cu
			OPTIONS
				${CARLSIM_CUDA_GENCODE}
		)
	endif()

	if(UNIX)
		cuda_add_library(carlsim-motion-energy
			../../../../external/MotionEnergy/cppME/cppME/src/motion_energy.cu
			OPTIONS
				-Xcompiler
				-fPIC
				${CARLSIM_CUDA_GENCODE}
		)
	endif()

	if(NOT CARLSIM_CUDA_GENCODE) 
		set_property(TARGET carlsim-motion-energy PROPERTY CUDA_ARCHITECTURES all)
	else()
		message("CARLSIM CUDA GENCODE enforced by " ${CARLSIM_CUDA_GENCODE})
	endif()	

	target_include_directories(carlsim-motion-energy 
		PUBLIC
			inc
			../../../../external//MotionEnergy/cppME/cppME/inc
			${CUDA_INCLUDE_DIRS}
			${CUDA_TOOLKIT_ROOT_DIR}/samples/common/inc
		)
	if(MSVC)
		target_include_directories(carlsim-motion-energy 
			PUBLIC
				"C:/ProgramData/NVIDIA Corporation/CUDA Samples/v${CUDA_VERSION_STRING}/common/inc" 					
			)
	endif()

	add_dependencies(motion_energy motion_energy_scripts carlsim-motion-energy)

	# Linking    
	target_link_libraries(motion_energy PRIVATE
		carlsim
		carlsim-visual-stimulus
		carlsim-motion-energy
	)
	target_include_directories(motion_energy PRIVATE
		../../../../tools/visual_stimulus
		../../../../external//MotionEnergy/cppME/cppME/inc
	)
          
	# Installation  
	install(TARGETS motion_energy
			EXPORT motion_energy
			RUNTIME 
			DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/5_motion_energy 
			COMPONENT motion_energy)

	install(DIRECTORY scripts 
			DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/5_motion_energy )

	install(DIRECTORY input
			DESTINATION $<$<CONFIG:Debug>:debug/>tutorials/5_motion_energy )
   
endif()
