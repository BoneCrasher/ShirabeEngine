cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)

set(SHIRABE_TEMPLATE "Application")
include(project_default_template)

set(SHIRABE_COMPILER__SUBSYSTEM CONSOLE)

set(SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS
        VK_NO_PROTOTYPES # Required to not access the function pointers to vulkan functionality to be dynamically loaded from the DLL.
    )

set(LOCAL_SPIRV_REFLECT_DIR
        ${THIRD_PARTY_DIR}/_deploy/spirv_reflect/${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})

set(SHIRABE_ADDITIONAL_INCLUDEPATHS
        ${LOCAL_SPIRV_REFLECT_DIR})

set(SHIRABE_PROJECT_ADDITIONAL_HEADERS
        ${LOCAL_SPIRV_REFLECT_DIR}/source/spirv_reflect.h)
set(SHIRABE_PROJECT_ADDITIONAL_SOURCES
        ${LOCAL_SPIRV_REFLECT_DIR}/source/spirv_reflect.c)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEASSET
        SPIRVCROSS
        SPIRVTOOLS
        GLSLANG
        STBI)

include(project_setup)

add_dependencies(shirabe_resource_compiler INSTALL_platform)
add_dependencies(shirabe_resource_compiler INSTALL_base)
add_dependencies(shirabe_resource_compiler INSTALL_log)
add_dependencies(shirabe_resource_compiler INSTALL_core)
add_dependencies(shirabe_resource_compiler INSTALL_utility)
add_dependencies(shirabe_resource_compiler INSTALL_asset)
add_dependencies(shirabe_resource_compiler INSTALL_math)
add_dependencies(shirabe_resource_compiler INSTALL_graphicsapi)

install(DIRECTORY ${SHIRABE_WORKSPACE_ROOT_DIR}/tools DESTINATION ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin USE_SOURCE_PERMISSIONS)
