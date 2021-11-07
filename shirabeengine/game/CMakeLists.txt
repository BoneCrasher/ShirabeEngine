cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)

set(SHIRABE_TEMPLATE "Application")
include(project_default_template)

set(SHIRABE_COMPILER__SUBSYSTEM CONSOLE)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEENGINE)

include(project_setup)

add_custom_target(shirabe_game_precopy
                  VERBATIM
                  COMMAND           cp -rf ${SHIRABE_WORKSPACE_ROOT_DIR}/tools ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin
                  WORKING_DIRECTORY ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin
                  COMMENT           "Precompiling shaders")

SET(SHIRABE_PROJECT_RESOURCE_CP_DEST_DIR ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin/data/input/resources)
add_custom_target(shirabe_game_copy_resources
                  VERBATIM
                  COMMAND           rm -rf ${SHIRABE_PROJECT_RESOURCE_CP_DEST_DIR} && mkdir -p ${SHIRABE_PROJECT_RESOURCE_CP_DEST_DIR} && cp -rf . ${SHIRABE_PROJECT_RESOURCE_CP_DEST_DIR}
                  WORKING_DIRECTORY ${SHIRABE_PROJECT_RES_DIR}
                  COMMENT           "Copying resources")

add_custom_target(shirabe_game_process_resources
                  VERBATIM
                  COMMAND           ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin/tools/shirabe_process_resources.sh -r ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin/data/input/resources
                  WORKING_DIRECTORY ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin
                  COMMENT           "Processing resources")

# Before adding the game project, hook in preprocessing tools

set(SHIRABE_SHADER_PRECOMPILER_BINARY_INPUT_NAME shirabe_resource_compiler)
set(SHIRABE_SHADER_PRECOMPILER_BINARY_NAME       shirabe_resource_compiler)
formatPlatformConfigName(
    ${SHIRABE_SHADER_PRECOMPILER_BINARY_INPUT_NAME}
    SHIRABE_ADDRESSMODEL_64BIT
    SHIRABE_PLATFORM_CONFIG
    OFF
    SHIRABE_SHADER_PRECOMPILER_BINARY_NAME
    )

LogStatus(MESSAGES "precompiler binary name: ${SHIRABE_SHADER_PRECOMPILER_BINARY_NAME}")

# Make sure that all shaders are compiled first!
# set(SHIRABE_SHADER_PRECOMPILE_COMMAND
#             "${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin/${SHIRABE_SHADER_PRECOMPILER_BINARY_NAME}")
#
# set(SHIRABE_SHADER_PRECOMPILE_COMMAND_ARGS
#     --verbose
#     --debug
#     --optimize
#     -I=./data/input/resources/shaders/include
#     -om=./data/output/resources/shaders/standard
#     -oi=./data/output/resources/shaders/standard/standard.mat.index
#     -os=./data/output/resources/shaders/standard/standard.mat.signature
#     -oc=./data/output/resources/shaders/standard/standard.mat.config
#     -i=./data/input/resources/shaders/standard/standard.mat.index
#     )
#
# add_custom_target(shirabe_shader_precompiler_execute
#                   VERBATIM
#                   COMMAND           ${SHIRABE_SHADER_PRECOMPILE_COMMAND} ${SHIRABE_SHADER_PRECOMPILE_COMMAND_ARGS}
#                   WORKING_DIRECTORY ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin
#                   COMMENT           "Precompiling shaders")

set(USE_RESOURCE_PROCESSING ON)

add_dependencies(shirabe_game_precopy           INSTALL_shirabe_resource_compiler)
add_dependencies(shirabe_game_copy_resources    shirabe_game_precopy)

if(USE_RESOURCE_PROCESSING)
    add_dependencies(shirabe_game_process_resources shirabe_game_copy_resources)
    add_dependencies(game                           shirabe_game_process_resources)
else()
    add_dependencies(game                           shirabe_game_copy_resources)
endif()

add_dependencies(game INSTALL_crc32generator)
add_dependencies(game INSTALL_shirabeengine)
add_dependencies(game INSTALL_platform)
add_dependencies(game INSTALL_base)
add_dependencies(game INSTALL_os)
add_dependencies(game INSTALL_log)
add_dependencies(game INSTALL_math)
add_dependencies(game INSTALL_core)
add_dependencies(game INSTALL_utility)
add_dependencies(game INSTALL_wsi)
add_dependencies(game INSTALL_graphicsapi)
add_dependencies(game INSTALL_asset)
