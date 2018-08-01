cmake_minimum_required(VERSION 3.10)

set(SHIRABE_CONFIG__DEBUG_LOG 1)

macro(INIT)
    #
    # Store current location from which CMake is run.
    # In case of Visual Studio 2017 RC2, this is the buildRoot
    # defined in CMakeSettings.json for the respective build configuration.
    #
    # It is defined to be ${ProjectDir}\\build\\${Target}\\${Platform}
    # for each configuration and consequently, to get to the "PWD"
    # of this project, we simply go back by a few folders...
    #

    # DON'T CHANGE THIS LINE
    file(TO_CMAKE_PATH $ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH} SHIRABE_BUILD_SYSTEM_DIR)
	
    file(TO_CMAKE_PATH $ENV{SHIRABE_WORKSPACE_PATH_SHIRABEENGINE}   SHIRABE_WORKSPACE_ROOT_DIR)

    set(SHIRABE_BUILD_SYSTEM_EXT_DIR ${SHIRABE_WORKSPACE_ROOT_DIR}/build_extension)
    set(APPS_DIR                     ${SHIRABE_WORKSPACE_ROOT_DIR}/apps)
    set(COMPONENT_DIR                ${SHIRABE_WORKSPACE_ROOT_DIR}/components)
    set(ENGINE_DIR                   ${SHIRABE_WORKSPACE_ROOT_DIR}/engine)
    set(THIRD_PARTY_DIR              ${SHIRABE_WORKSPACE_ROOT_DIR}/thirdparty)

    set(SHIRABE_PUBLIC_BUILD_ROOT    ${SHIRABE_WORKSPACE_ROOT_DIR}/)
    set(SHIRABE_PUBLIC_DEPLOY_ROOT   ${SHIRABE_WORKSPACE_ROOT_DIR}/)

    # DO NOT EDIT anything below this comment.
    set(CMAKE_MODULE_PATH       ${CMAKE_MODULE_PATH}
                                  ${SHIRABE_BUILD_SYSTEM_DIR}/core
                                  ${SHIRABE_BUILD_SYSTEM_DIR}/templates
                                  ${SHIRABE_BUILD_SYSTEM_DIR}/propertysheets
                                  ${SHIRABE_BUILD_SYSTEM_DIR}/thirdparty
                                  ${SHIRABE_BUILD_SYSTEM_EXT_DIR}/templates
                                  ${SHIRABE_BUILD_SYSTEM_EXT_DIR}/propertysheets
                                  ${SHIRABE_BUILD_SYSTEM_EXT_DIR}/thirdparty)

    include(common_helper)
    include(thirdparty_helper)

    set(BUILDSYSTEM_ENVIRONMENT_INITIALIZED TRUE)
endmacro()

if(NOT BUILDSYSTEM_ENVIRONMENT_INITIALIZED)
    INIT()
    LogStatus(MESSAGES "Initialized Buildsystem environment.")
else()
    LogStatus(MESSAGES "Buildsystem environment is already initialized.")
endif()
