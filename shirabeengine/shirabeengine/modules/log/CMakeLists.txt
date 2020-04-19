cmake_minimum_required(VERSION 3.12)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEBASE)
        
include(project_setup)

add_dependencies(log INSTALL_base)
add_dependencies(log INSTALL_platform)
