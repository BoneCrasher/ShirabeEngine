cmake_minimum_required(VERSION 3.9)

# 
# Automatically detect the workspace root path 'SHIRABE_sdk_component_workspace',
# by traversing up the path towards the drive-letter.
# If the path was not found, consider the subproject being located incorrectly.
# 
set(SHIRABE_WORKSPACE_ROOT_DIR ${CMAKE_SOURCE_DIR})

set(TMP_PATH)
get_filename_component(TMP_PATH "${SHIRABE_WORKSPACE_ROOT_DIR}" NAME)
message(STATUS "Workspace root: ${TMP_PATH}")
while(NOT ("${TMP_PATH}" STREQUAL "" OR "${TMP_PATH}" STREQUAL "ShirabeDevelopment"))
	get_filename_component(SHIRABE_WORKSPACE_ROOT_DIR "${SHIRABE_WORKSPACE_ROOT_DIR}" DIRECTORY)
	get_filename_component(TMP_PATH "${SHIRABE_WORKSPACE_ROOT_DIR}" NAME)
	
	message(STATUS "Workspace root: ${TMP_PATH}")
endwhile()

if("${TMP_PATH}" STREQUAL "")
	message(FATAL_ERROR "Trying to open project outside of valid context.")
endif()
message(STATUS "Workspace directory: ${SHIRABE_WORKSPACE_ROOT_DIR}")
                                                   
#
# Store current location from which CMake is run.
# In case of Visual Studio 2017 RC2, this is the buildRoot
# defined in CMakeSettings.json for the respective build configuration.
#
# It is defined to be ${ProjectDir}\\build\\${Target}\\${Platform}
# for each configuration and consequently, to get to the "PWD"
# of this project, we simply go back by a few folders...
#
set(SHIRABE_WS_BASE_DIR         ${SHIRABE_WORKSPACE_ROOT_DIR})
set(SHIRABE_CMAKE_FRAMEWORK_DIR ${SHIRABE_WORKSPACE_ROOT_DIR}/ShirabeCMakeFramework)                         
set(SHIRABE_THIRD_PARTY_DIR     ${SHIRABE_WORKSPACE_ROOT_DIR}/ThirdParty)     
set(SHIRABE_PUBLIC_BUILD_ROOT   ${SHIRABE_WORKSPACE_ROOT_DIR}/_build)                                                       
set(SHIRABE_PUBLIC_DEPLOY_ROOT  ${SHIRABE_WORKSPACE_ROOT_DIR}/_deploy)                                                 
set(CMAKE_MODULE_PATH           ${CMAKE_MODULE_PATH} ${SHIRABE_WORKSPACE_ROOT_DIR}/ShirabeCMakeFramework) 
                                                                             
include(CommonHelper)                                                      
include(ProjectHelper)                                                                              
include(3rdPartyLibraryHelper)                                                                         