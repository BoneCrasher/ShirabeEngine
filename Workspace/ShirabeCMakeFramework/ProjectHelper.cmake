#
# This file performs the basic per-project initialization once setupProject is called
# with the respective parameters.
#
macro(setupProject module_name target_type)
	set(SHIRABE_PROJECT_ID ${module_name})
	set(SHIRABE_TEMPLATE   ${target_type})

	message(STATUS "Setting up project ${module_name}")

	# Add the make/cmake subdir of the project to the module path to allow finding all configurations.
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/make/cmake)

	if(MSVC)
		if(${CMAKE_CONFIGURATION_TYPES} STREQUAL "Debug")
			set(SHIRABE_DEBUG ON)
		endif()
	else()
		if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
			set(SHIRABE_DEBUG ON)
		endif()
	endif()
	message(STATUS "Debug-Mode?: ${SHIRABE_DEBUG}")
	#
	# Default defines:
	#
	# Additional DEBUG Preprocessor-Definitions
	if(SHIRABE_DEBUG)
		append(
			SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS
				SHIRABE_DEBUG
		)
	# Additional RELEASE Preprocessor-Definitions
	else() 
		append(
			SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS
				NO_LOG
		)
	endif()
endmacro(setupProject)