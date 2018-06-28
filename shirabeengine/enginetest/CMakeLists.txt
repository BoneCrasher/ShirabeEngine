cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "Application")

set(SHIRABE_COMPILER__SUBSYSTEM CONSOLE)

set(
    SR_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEENGINE
        STBI
        FXGLTF
        JSON
        XML2)

include(project_setup)
