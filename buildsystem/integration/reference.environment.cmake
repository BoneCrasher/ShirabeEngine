cmake_minimum_required(VERSION 3.10)

set(SR_CONFIG__DEBUG_LOG 1) # Set to 1 to enable logging. 0 Otherwise.

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
    file(TO_CMAKE_PATH $ENV{SR_BUILDSYSTEM_ENV_FILEPATH} SR_BUILD_SYSTEM_DIR)
	
    file(TO_CMAKE_PATH $ENV{SR_WORKSPACE_PATH_<your workspace id in uppercase letters>} SR_WORKSPACE_ROOT_DIR)

    set(SR_BUILD_SYSTEM_EXT_DIR ${SR_WORKSPACE_ROOT_DIR}/build_extension)
    set(APPS_DIR                ${SR_WORKSPACE_ROOT_DIR}/apps)
    set(COMPONENT_DIR           ${SR_WORKSPACE_ROOT_DIR}/components)
    set(ENGINE_DIR              ${SR_WORKSPACE_ROOT_DIR}/engine)
    set(THIRD_PARTY_DIR         ${SR_WORKSPACE_ROOT_DIR}/thirdparty)

    set(SR_PUBLIC_BUILD_ROOT    ${SR_WORKSPACE_ROOT_DIR}/_build)
    set(SR_PUBLIC_DEPLOY_ROOT   ${SR_WORKSPACE_ROOT_DIR}/_deploy)

    # DO NOT EDIT anything below this comment.
    set(CMAKE_MODULE_PATH       ${CMAKE_MODULE_PATH}
                                  ${SR_BUILD_SYSTEM_DIR}/core
                                  ${SR_BUILD_SYSTEM_DIR}/templates
                                  ${SR_BUILD_SYSTEM_DIR}/propertysheets
                                  ${SR_BUILD_SYSTEM_DIR}/thirdparty
                                  ${SR_BUILD_SYSTEM_EXT_DIR}/templates
                                  ${SR_BUILD_SYSTEM_EXT_DIR}/propertysheets
                                  ${SR_BUILD_SYSTEM_EXT_DIR}/thirdparty)

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
