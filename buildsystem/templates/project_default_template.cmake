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
cmake_minimum_required(VERSION 3.10)
# Uncomment for your project
# include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/<your workspace>.environment.cmake)
# include(project_default_template)

# Uncomment this: Specify target type of this project
# set(SHIRABE_TEMPLATE "Application") # or SharedLib, StaticLib, HeaderOnly

# Uncomment this: Include absolute path to cmake_environment.generated.cmake
# include(../cmake_environment.generated.cmake)
# include(SHIRABE_CommonProject.template.cmake)

# Use Unicode Charset?
set(SHIRABE_UNICODE_ENABLED TRUE)

# Additional Preprocessor-Definitions
set(SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS)
set(SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS_DEBUG)
set(SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS_RELEASE)

append(
    SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS_DEBUG
        SHIRABE_DEBUG
)
append(
    SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS_RELEASE
        NO_LOG
)

# Please see ${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/core/compiler_options_default.cmake for all available
# SHIRABE_COMPILER__~-Flags!

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
set(SHIRABE_PCH_ENABLED  YES)
# ...and uncomment this and set the appropriate filename.
set(SHIRABE_PCH_FILENAME SHIRABE_pch.h)

# Additional plain text compiler options, which are not covered by the buildsystem's compiler mapping
set(SHIRABE_ADDITIONAL_MSVC_CL_COMMON_FLAGS)
set(SHIRABE_ADDITIONAL_MSVC_CL_CFLAGS)
set(SHIRABE_ADDITIONAL_MSVC_CL_CXXFLAGS)
set(SHIRABE_ADDITIONAL_GCC_CL_COMMON_FLAGS)
set(SHIRABE_ADDITIONAL_GCC_CL_CFLAGS)
set(SHIRABE_ADDITIONAL_GCC_CL_CXXFLAGS)

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

# Uncomment for your project
# include(project_setup)
