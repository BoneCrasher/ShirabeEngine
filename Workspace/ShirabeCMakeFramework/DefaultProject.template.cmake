#--------------------------------------------------------------------------------
# COMMON PROJECT TEMPLATE 
#
# DO NOT CHANGE, UNLESS THE FRAMEWORK IS EXTENDED!
# IF YOU WANT TO ADD A NEW PROJECT, COPY ANOTHER AND INCLUDE THIS!
#--------------------------------------------------------------------------------
# Basic template containing all public options for projects to be altered.
# This file also serves as the basic include for all project's default settings!
# 
# Basic "PropertySheet"-Functionality.
#--------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.9)

# Uncomment this: Specify target type of this project
# set(SHIRABE_TEMPLATE "Application") # or SharedLib, StaticLib, HeaderOnly

# Uncomment this: Include absolute path to cmake_environment.generated.cmake
# include(../cmake_environment.generated.cmake)
# include(SHIRABE_CommonProject.template.cmake)

# Use Unicode Charset?
set(SHIRABE_UNICODE_ENABLED FALSE)

# Please see SHIRABE_CMake_HandleCompilerOptions_Defaults.cmake for all available 
# SHIRABE_COMPILER__~-Flags!

# SHIRABE_API is a C-interface!
# set(SHIRABE_COMPILER__TREAT_C_AS_CPP_CODE YES)

if(SHIRABE_DEBUG)
	set(SHIRABE_COMPILER__ALLOW_SYSTEM_INTERNAL_FN    NO)
	set(SHIRABE_COMPILER__OPTIMIZE_ENTIRE_APPLICATION NO)
	set(SHIRABE_COMPILER__WARNING_LEVEL               4)
	set(SHIRABE_COMPILER__OPTIMIZATION_LEVEL          O_Debug)
else()
	set(SHIRABE_COMPILER__ALLOW_SYSTEM_INTERNAL_FN    YES)
	set(SHIRABE_COMPILER__OPTIMIZE_ENTIRE_APPLICATION YES)
	set(SHIRABE_COMPILER__WARNING_LEVEL               0)
	set(SHIRABE_COMPILER__OPTIMIZATION_LEVEL          O_Speed)
endif()

# Precompiled header settings
# Set to YES to use a PCH...
set(SHIRABE_PCH_ENABLED  NO)
# ...and uncomment this and set the appropriate filename.
# set(SHIRABE_PCH_FILENAME stdafx.h)

# Additional Preprocessor-Definitions
set(
    SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS
        # Anything else you need to define is to be APPENDED in the project file!
)

# Additional DEBUG Preprocessor-Definitions
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	append(
		SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS
			SHIRABE_DEBUG
	)
# Additional RELEASE Preprocessor-Definitions
else() 
	append(
		SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS
			SHIRABE_DISABLE_LOG
	)
endif()

# Put all include paths not automatically resolved by the standard project stucture
# or third party libraries in here:
append(
    SHIRABE_ADDITIONAL_INCLUDEPATHS
    # path/to/include/path
)

# Exclude specific filenames and headers. 
# Filenames are relative code /code/include and /code/source, 
#  so provide subdirectories where needed.
set(SHIRABE_EXCLUDED_HEADERS)
set(SHIRABE_EXCLUDED_SOURCES)

# Additional features
