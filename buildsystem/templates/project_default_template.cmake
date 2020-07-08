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

cmake_policy (SET CMP0022 NEW)

#---------------------------------------------------------------------------------------------------------------------------------------------------
# PROJECT ID
#---------------------------------------------------------------------------------------------------------------------------------------------------

# This line will adapt the actual "directory" of a subproject as
# the project ID.
# E.g: ~/SmartRaySDK/Base/SHIRABE_kernel --> SHIRABE_kernel
get_filename_component(SHIRABE_PROJECT_ID ${CMAKE_CURRENT_SOURCE_DIR} NAME)
if(SHIRABE_PROJECT_ID_OVERRIDE)
        set(SHIRABE_PROJECT_ID ${SHIRABE_PROJECT_ID_OVERRIDE})
endif()

LogStatus(
        MESSAGES
        " "
        "----------------------------------- Generic Project Script for ${SHIRABE_PROJECT_ID} -----------------------------------"
        " ")

#---------------------------------------------------------------------------------------------------------------------------------------------------
# PROPERTY SHEETS
#---------------------------------------------------------------------------------------------------------------------------------------------------

set(SHIRABE_PROJECT_PROPSHEET_ID       ${SHIRABE_PROJECT_ID}.prop)
set(SHIRABE_PROJECT_PROPSHEET_FILEPATH ${SHIRABE_BUILD_SYSTEM_DIR}/propertysheets/${SHIRABE_PROJECT_PROPSHEET_ID}.cmake)

LogStatus(
        MESSAGES
        " "
        "#### PROPERTY SHEETS ####"
        " "
        "Checking for property sheets...")

# Try to find a property sheet for the project
if(EXISTS ${SHIRABE_PROJECT_PROPSHEET_FILEPATH})
        LogStatus(MESSAGES "-> Found property sheet for project ${SHIRABE_PROJECT_ID}")
        include(${SHIRABE_PROJECT_PROPSHEET_ID})
else()
        LogStatus(MESSAGES "-> No property sheet for project ${SHIRABE_PROJECT_ID} found.")
endif()

#---------------------------------------------------------------------------------------------------------------------------------------------------
# PROJECT CORE SETUP
#---------------------------------------------------------------------------------------------------------------------------------------------------

LogStatus(
        MESSAGES
        " "
        "#### PROJECT CORE SETUP ####"
        " ")

# Check, if the project is added as a subcomponent in another project's context.
# If so, do not use the "project(...)" macro, to avoid multiple entries for the
# same subproject in the target list.
#
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
        message(STATUS "Project ${SHIRABE_PROJECT_ID} is sub-project. Won't add another project(...)-command.")
else()
        project(${SHIRABE_PROJECT_ID})
endif()

# Some basic conditions: Where?, What?, How?
set(SHIRABE_PROJECT_DIR               ${CMAKE_CURRENT_SOURCE_DIR}) # Directory of the CMake-Projectfile, which included this file
set(SHIRABE_MODULE_NAME               ${SHIRABE_PROJECT_ID})            # Project-name = Module-name!
set(SHIRABE_MODULE_TARGET_OUTPUT_NAME ${SHIRABE_PROJECT_ID})            # Target-Output-name = Module-name!
set(SHIRABE_PROJECT_TEMPLATE          ${SHIRABE_TEMPLATE})              # Application | StaticLib | SharedLib

set(SHIRABE_PROJECT_TEMPLATE_${SHIRABE_PROJECT_ID} ${SHIRABE_TEMPLATE} PARENT_SCOPE)
# message(STATUS "Globally set project template id: ${SHIRABE_PROJECT_TEMPLATE_${SHIRABE_PROJECT_ID}}")

set(SHIRABE_PROJECT_LANGUAGE          CXX)                         # Always assume C++ unless overwritten
if(SHIRABE_LANGUAGE)
        set(SHIRABE_PROJECT_LANGUAGE ${SHIRABE_LANGUAGE})
endif()


# Take the upper case module name as library key for build related module aware macros.
string(TOUPPER ${SHIRABE_MODULE_NAME} SHIRABE_LIBRARY_KEY)

#---------------------------------------------------------------------------------------------------------------------------------------------------
# Basic environments
#---------------------------------------------------------------------------------------------------------------------------------------------------

LogStatus(MESSAGES "Searching for modules in: ${CMAKE_MODULE_PATH}")
LogStatus(MESSAGES "Working directory of project '${SHIRABE_PROJECT_ID}': ${SHIRABE_PROJECT_DIR}")

#---------------------------------------------------------------------------------------------------------------------------------------------------
# PROJECT TARGET CONFIG
#---------------------------------------------------------------------------------------------------------------------------------------------------

LogStatus(
        MESSAGES
        " "
        "#### PROJECT TARGET CONFIG ####"
        " ")

# Determine, wether we build a static lib or a shared lib and adapt properties
set(SHIRABE_BUILD_APPLICATION OFF)
set(SHIRABE_BUILD_STATICLIB   OFF)
set(SHIRABE_BUILD_SHAREDLIB   OFF)
set(SHIRABE_HEADER_ONLY       OFF)

if("${SHIRABE_PROJECT_TEMPLATE}" STREQUAL "Application")
        set(SHIRABE_BUILD_APPLICATION ON)
elseif("${SHIRABE_PROJECT_TEMPLATE}" STREQUAL "StaticLib")
        set(SHIRABE_BUILD_STATICLIB  ON)
elseif("${SHIRABE_PROJECT_TEMPLATE}" STREQUAL "SharedLib")
        set(SHIRABE_BUILD_SHAREDLIB ON)
elseif("${SHIRABE_PROJECT_TEMPLATE}" STREQUAL "HeaderOnly")
        set(SHIRABE_HEADER_ONLY ON)
endif()

LogStatus(
        MESSAGES
        "Build Mode: "
        "-> Build App:       ${SHIRABE_BUILD_APPLICATION}"
        "-> Build SharedLib: ${SHIRABE_BUILD_SHAREDLIB}"
        "-> Build StaticLib: ${SHIRABE_BUILD_STATICLIB}"
        " ")

if(WIN32)
        set(SHIRABE_PROJECT_TARGET_SUFFIX_STATIC .lib)
        set(SHIRABE_PROJECT_TARGET_SUFFIX_SHARED .dll)
        set(SHIRABE_PROJECT_TARGET_SUFFIX_APP    .exe)
else()
        set(SHIRABE_PROJECT_TARGET_SUFFIX_STATIC .a)
        set(SHIRABE_PROJECT_TARGET_SUFFIX_SHARED .so)
        set(SHIRABE_PROJECT_TARGET_SUFFIX_APP)
endif(WIN32)

#---------------------------------------------------------------------------------------------------------------------------------------------------
# Debug or not, das ist hier die Frage...
#
# Important: Visual Studio 2017 CMake will not specify CMAKE_BUILD_TYPE properly.
#            It uses some strange custom bullshit, which is why we have to define
#            it manually invoking CMake!
#---------------------------------------------------------------------------------------------------------------------------------------------------
set(SHIRABE_DEBUG OFF)
set(SHIRABE_TEST  OFF)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(SHIRABE_DEBUG ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(SHIRABE_DEBUG OFF)
endif()

LogStatus(MESSAGES "CMAKE BUILD TYPE: ${CMAKE_BUILD_TYPE}")

#---------------------------------------------------------------------------------------------------------------------------------------------------
# Determine final module name, platform and config
#
# Will write values into:
#  SHIRABE_ADDRESSMODEL_64BIT
#  SHIRABE_PLATFORM_TARGET
#  SHIRABE_PLATFORM_CONFIG
#  SHIRABE_MODULE_OUTPUT_NAME
#---------------------------------------------------------------------------------------------------------------------------------------------------
setupTargetAndConfig(${SHIRABE_MODULE_NAME})

set(SHIRABE_MODULE_TARGET_OUTPUT_NAME ${SHIRABE_MODULE_OUTPUT_NAME})

LogStatus(
        MESSAGES
        " "
        "SHIRABE_PLATFORM_TARGET:        ${SHIRABE_PLATFORM_TARGET}"
        "SHIRABE_PLATFORM_CONFIG:        ${SHIRABE_PLATFORM_CONFIG}"
        "SHIRABE_MODULE_NAME:            ${SHIRABE_MODULE_NAME}"
        "SHIRABE_MODULE_OUTPUT_NAME:     ${SHIRABE_MODULE_TARGET_OUTPUT_NAME}"
        "SHIRABE_PLATFORM_CONFIG_SUFFIX: ${SHIRABE_PLATFORM_CONFIG_SUFFIX}"
        " "
)

#---------------------------------------------------------------------------------------------------------------------------------------------------
# FILESYSTEM ENVIRONMENT
#---------------------------------------------------------------------------------------------------------------------------------------------------
LogStatus(
        MESSAGES
        " "
        "#### FILESYSTEM ENVIRONMENT ####"
        " ")

set(SHIRABE_PROJECT_CONF_DIR         ${SHIRABE_PROJECT_DIR}/config)
set(SHIRABE_PROJECT_CODE_DIR         ${SHIRABE_PROJECT_DIR}/code)
set(SHIRABE_PROJECT_INC_DIR          ${SHIRABE_PROJECT_DIR}/code/include)
set(SHIRABE_PROJECT_SRC_DIR          ${SHIRABE_PROJECT_DIR}/code/source)
set(SHIRABE_PROJECT_UI_DIR           ${SHIRABE_PROJECT_DIR}/ui)
set(SHIRABE_PROJECT_RES_DIR          ${SHIRABE_PROJECT_DIR}/resources)
set(SHIRABE_PROJECT_DOC_DIR          ${SHIRABE_PROJECT_DIR}/documentation)

set(SHIRABE_PROJECT_BUILD_DIR        ${SHIRABE_PUBLIC_BUILD_ROOT}/${SHIRABE_PLATFORM_CONFIG_SUFFIX})
set(SHIRABE_PROJECT_INTERMEDIATE_DIR ${SHIRABE_PUBLIC_BUILD_ROOT}/${SHIRABE_PLATFORM_CONFIG_SUFFIX}/intermediate)
set(SHIRABE_PROJECT_OBJECTS_DIR      ${SHIRABE_PROJECT_INTERMEDIATE_DIR}/obj)
set(SHIRABE_PROJECT_GEN_DIR          ${SHIRABE_PROJECT_INTERMEDIATE_DIR}/gen)

# Only used for public installation! (YES I know I should be consistent with the above "private" deploy dir...)
set(SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR
        ${SHIRABE_PUBLIC_DEPLOY_ROOT}/${SHIRABE_PLATFORM_CONFIG_SUFFIX})

if(CMAKE_INSTALL_PREFIX)
        set(SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR ${CMAKE_INSTALL_PREFIX})
endif(CMAKE_INSTALL_PREFIX)

set(SHIRABE_PROJECT_DEPLOY_DIR
        ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}) # ${SHIRABE_PROJECT_DIR}/_deploy/${SHIRABE_PLATFORM_CONFIG_SUFFIX})

set(EXECUTABLE_OUTPUT_PATH ${SHIRABE_PROJECT_DEPLOY_DIR})
set(LIBRARY_OUTPUT_PATH    ${SHIRABE_PROJECT_DEPLOY_DIR})
set(CMAKE_INTDIR           ${SHIRABE_PROJECT_INTERMEDIATE_DIR})

LogStatus(
        MESSAGES
        "-----------------------------------------------------------------------------------"
        "Project-Directory-Setup:"
        "-----------------------------------------------------------------------------------"
        "${NAME} - Base Directory:            ${SHIRABE_PROJECT_DIR}"
        "${NAME} - Config Directory:          ${SHIRABE_PROJECT_CONF_DIR}"
        "${NAME} - Code Directory:            ${SHIRABE_PROJECT_CODE_DIR}"
        "${NAME} - Include Directory:         ${SHIRABE_PROJECT_INC_DIR}"
        "${NAME} - Source Directory:          ${SHIRABE_PROJECT_SRC_DIR}"
        "${NAME} - Ui Files Directory:        ${SHIRABE_PROJECT_UI_DIR}"
        "${NAME} - Resources Directory:       ${SHIRABE_PROJECT_RES_DIR}"
        "${NAME} - Documentation Directory:   ${SHIRABE_PROJECT_DOC_DIR}"
        ""
        "${NAME} - Build Directory:           ${SHIRABE_PROJECT_BUILD_DIR}"
        "${NAME} - Intermediate Directory:    ${SHIRABE_PROJECT_INTERMEDIATE_DIR}"
        "${NAME} - Objects Directory:         ${SHIRABE_PROJECT_OBJECTS_DIR}"
        "${NAME} - Generated Files Directory: ${SHIRABE_PROJECT_GEN_DIR}"
        ""
        "${NAME} - Deploy Directory:          ${SHIRABE_PROJECT_DEPLOY_DIR}"
        "${NAME} - Public Deploy Directory:   ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}"
        ""
        "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}"
        "-----------------------------------------------------------------------------------"
        " "
)

# Uncomment for your project
# include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/<your workspace>.environment.cmake)
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
set(SHIRABE_PCH_ENABLED  NO)
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
