cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "Application")

set(SHIRABE_COMPILER__SUBSYSTEM CONSOLE)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEENGINE
        VULKANSDK
        STBI
        FXGLTF
        JSON
        XML2)

include(project_setup)

file(COPY ${SHIRABE_WORKSPACE_ROOT_DIR}/tools DESTINATION ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin            USE_SOURCE_PERMISSIONS)
file(COPY ${SHIRABE_PROJECT_RES_DIR}          DESTINATION ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin/data/input USE_SOURCE_PERMISSIONS)

# Before adding the game project, hook in preprocessing tools

set(SHIRABE_SHADER_PRECOMPILER_BINARY_INPUT_NAME shirabe_shader_precompiler)
set(SHIRABE_SHADER_PRECOMPILER_BINARY_NAME       shirabe_shader_precompiler)
formatPlatformConfigName(
    ${SHIRABE_SHADER_PRECOMPILER_BINARY_INPUT_NAME}
    SHIRABE_ADDRESSMODEL_64BIT
    SHIRABE_PLATFORM_CONFIG
    OFF
    SHIRABE_SHADER_PRECOMPILER_BINARY_NAME
    )

LogStatus(MESSAGES "precompiler binary name: ${SHIRABE_SHADER_PRECOMPILER_BINARY_NAME}")

set(SHIRABE_SHADER_PRECOMPILE_COMMAND
            "${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin/${SHIRABE_SHADER_PRECOMPILER_BINARY_NAME}")

set(SHIRABE_SHADER_PRECOMPILE_COMMAND_ARGS
    --verbose --debug --optimize -I=./data/input/resources/shaders/include -o=./data/resources/shaders/standard -i=./data/input/resources/shaders/standard/standard.mat.index)

add_custom_target(shirabe_shader_precompiler_execute
                  VERBATIM
                  COMMAND           ${SHIRABE_SHADER_PRECOMPILE_COMMAND} ${SHIRABE_SHADER_PRECOMPILE_COMMAND_ARGS}
                  WORKING_DIRECTORY ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin
                  COMMENT "Precompiling shaders")
add_dependencies(shirabe_shader_precompiler_execute INSTALL_shirabe_shader_precompiler)

add_dependencies(game shirabe_shader_precompiler_execute)
add_dependencies(game shirabe_shader_precompiler)
add_dependencies(game INSTALL_shirabeengine)
add_dependencies(game INSTALL_platform)
add_dependencies(game INSTALL_base)
add_dependencies(game INSTALL_os)
add_dependencies(game INSTALL_log)
add_dependencies(game INSTALL_math)
add_dependencies(game INSTALL_core)
add_dependencies(game INSTALL_utility)
add_dependencies(game INSTALL_wsi)
add_dependencies(game INSTALL_resources)
add_dependencies(game INSTALL_graphicsapi)
add_dependencies(game INSTALL_rendering)
add_dependencies(game INSTALL_material)
add_dependencies(game INSTALL_vulkan_integration)
add_dependencies(game INSTALL_asset)
