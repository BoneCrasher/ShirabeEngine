cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "Application")

set(SHIRABE_COMPILER__SUBSYSTEM CONSOLE)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEMATERIAL
        SHIRABEMODULEASSET
        SHIRABEMODULEGRAPHICSAPI
        SHIRABEMODULERESOURCES
        SHIRABEMODULEMATH
        SHIRABEMODULEUTILITY
        SHIRABEMODULECORE
        SHIRABEMODULELOG
        SHIRABEMODULEBASE
        SHIRABEMODULEPLATFORM
        SPIRVCROSS
        SPIRVTOOLS
        CRYPTOPP
        GLSLANG
        JSON
        XML2
        ZLIB)

include(project_setup)

add_dependencies(shirabe_shader_precompiler INSTALL_platform)
add_dependencies(shirabe_shader_precompiler INSTALL_base)
add_dependencies(shirabe_shader_precompiler INSTALL_log)
add_dependencies(shirabe_shader_precompiler INSTALL_core)
add_dependencies(shirabe_shader_precompiler INSTALL_utility)
add_dependencies(shirabe_shader_precompiler INSTALL_asset)
add_dependencies(shirabe_shader_precompiler INSTALL_math)
add_dependencies(shirabe_shader_precompiler INSTALL_resources)
add_dependencies(shirabe_shader_precompiler INSTALL_graphicsapi)
add_dependencies(shirabe_shader_precompiler INSTALL_material)

install(DIRECTORY ${SHIRABE_WORKSPACE_ROOT_DIR}/tools DESTINATION ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin USE_SOURCE_PERMISSIONS)
