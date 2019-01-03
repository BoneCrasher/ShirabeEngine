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

install(DIRECTORY ${SHIRABE_WORKSPACE_ROOT_DIR}/tools DESTINATION ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin USE_SOURCE_PERMISSIONS)

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
add_dependencies(game INSTALL_shirabeengine)