cmake_minimum_required(VERSION 3.10)

# Module name for export declaration
set(SHIRABE_MODULE_TARGET_NAME SHIRABEENGINE)

# Add modules
add_subdirectory(modules/platform)
add_subdirectory(modules/base)
add_subdirectory(modules/log)
add_subdirectory(modules/math)
add_subdirectory(modules/core)
add_subdirectory(modules/os)
add_subdirectory(modules/utility)
add_subdirectory(modules/wsi)
add_subdirectory(modules/graphicsapi)
add_subdirectory(modules/resources)
add_subdirectory(modules/resources/submodules/buffers)
add_subdirectory(modules/resources/submodules/textures)
add_subdirectory(modules/resources/submodules/material)
add_subdirectory(modules/resources/submodules/mesh)
add_subdirectory(modules/rendering)
add_subdirectory(modules/vulkan_integration)
add_subdirectory(modules/asset)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "SharedLib")

set(
	SHIRABE_EXCLUDED_SOURCES 
		ApplicationMain.cpp
		Serialization/JSONObjectSerializer.cpp
        Serialization/XMLObjectSerializer.cpp)

set(
    SHIRABE_EMBED_MODULE_NAMES
        base
        os
        core
        log
        math
        utility
        wsi
        resources
        rendering
        graphicsapi
		buffers
		textures
        material
		mesh
        vulkan_integration
        asset)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEVULKAN)

include(project_setup)

add_dependencies(shirabeengine INSTALL_wsi)
add_dependencies(shirabeengine INSTALL_resources)
add_dependencies(shirabeengine INSTALL_graphicsapi)
add_dependencies(shirabeengine INSTALL_rendering)
add_dependencies(shirabeengine INSTALL_vulkan_integration)
add_dependencies(shirabeengine INSTALL_asset)
add_dependencies(shirabeengine INSTALL_material)
add_dependencies(shirabeengine INSTALL_mesh)
add_dependencies(shirabeengine INSTALL_buffers)
add_dependencies(shirabeengine INSTALL_textures)
add_dependencies(shirabeengine INSTALL_utility)
add_dependencies(shirabeengine INSTALL_core)
add_dependencies(shirabeengine INSTALL_os)
add_dependencies(shirabeengine INSTALL_log)
add_dependencies(shirabeengine INSTALL_math)
add_dependencies(shirabeengine INSTALL_base)
add_dependencies(shirabeengine INSTALL_platform)

