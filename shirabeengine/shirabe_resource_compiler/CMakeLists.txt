cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "Application")

set(SHIRABE_COMPILER__SUBSYSTEM CONSOLE)

set(SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS
        VK_NO_PROTOTYPES # Required to not access the function pointers to vulkan functionality to be dynamically loaded from the DLL.
    )

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
        VULKANHEADERS
        CRYPTOPP
        GLSLANG
        FXGLTF
        FMT
        JSON
        XML2
        ZLIB)

include(project_setup)

add_dependencies(shirabe_resource_compiler INSTALL_platform)
add_dependencies(shirabe_resource_compiler INSTALL_base)
add_dependencies(shirabe_resource_compiler INSTALL_log)
add_dependencies(shirabe_resource_compiler INSTALL_core)
add_dependencies(shirabe_resource_compiler INSTALL_utility)
add_dependencies(shirabe_resource_compiler INSTALL_asset)
add_dependencies(shirabe_resource_compiler INSTALL_math)
add_dependencies(shirabe_resource_compiler INSTALL_resources)
add_dependencies(shirabe_resource_compiler INSTALL_graphicsapi)
add_dependencies(shirabe_resource_compiler INSTALL_material)

install(DIRECTORY ${SHIRABE_WORKSPACE_ROOT_DIR}/tools DESTINATION ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin USE_SOURCE_PERMISSIONS)
