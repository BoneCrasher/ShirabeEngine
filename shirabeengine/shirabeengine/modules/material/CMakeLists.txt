cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEASSET
        SHIRABEMODULEUTILITY
        SHIRABEMODULERESOURCES
        SHIRABEMODULEGRAPHICSAPI
        SHIRABEMODULECORE
        SHIRABEMODULEMATH
        SHIRABEMODULELOG
        SHIRABEMODULEPLATFORM
        SHIRABEMODULEBASE
        VULKANHEADERS
        JSON)

include(project_setup)

add_dependencies(material INSTALL_platform)
add_dependencies(material INSTALL_base)
add_dependencies(material INSTALL_log)
add_dependencies(material INSTALL_core)
add_dependencies(material INSTALL_utility)
add_dependencies(material INSTALL_graphicsapi)
add_dependencies(material INSTALL_resources)
add_dependencies(material INSTALL_math)
add_dependencies(material INSTALL_asset)
