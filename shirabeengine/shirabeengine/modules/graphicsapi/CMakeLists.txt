cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "HeaderOnly")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULECORE
        VULKANHEADERS)

include(project_setup)

add_dependencies(graphicsapi INSTALL_base)
add_dependencies(graphicsapi INSTALL_platform)
add_dependencies(graphicsapi INSTALL_math)
add_dependencies(graphicsapi INSTALL_log)
add_dependencies(graphicsapi INSTALL_core)
