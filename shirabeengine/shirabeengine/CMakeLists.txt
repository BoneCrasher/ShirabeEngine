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
add_subdirectory(modules/asset)
add_subdirectory(modules/graphicsapi)
add_subdirectory(modules/rhi)
add_subdirectory(modules/data)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)

set(SHIRABE_TEMPLATE "SharedLib")
include(project_default_template)

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
        resources
		rhi
		data)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
		SHIRABEMODULEDATA)

include(project_setup)

add_dependencies(shirabeengine INSTALL_data)
add_dependencies(shirabeengine INSTALL_rhi)
add_dependencies(shirabeengine INSTALL_utility)
add_dependencies(shirabeengine INSTALL_core)
add_dependencies(shirabeengine INSTALL_os)
add_dependencies(shirabeengine INSTALL_log)
add_dependencies(shirabeengine INSTALL_math)
add_dependencies(shirabeengine INSTALL_base)
add_dependencies(shirabeengine INSTALL_platform)

