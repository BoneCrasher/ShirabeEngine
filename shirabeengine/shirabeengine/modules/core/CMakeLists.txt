cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)

set(SHIRABE_TEMPLATE "StaticLib")
include(project_default_template)

set(SHIRABE_EXCLUDED_SOURCES 
        serialization/jsonobjectserializer.cpp
        serialization/xmlobjectserializer.cpp)
	
set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEMATH)

include(project_setup)

add_dependencies(core INSTALL_math)
add_dependencies(core INSTALL_log)
add_dependencies(core INSTALL_base)
add_dependencies(core INSTALL_platform)
