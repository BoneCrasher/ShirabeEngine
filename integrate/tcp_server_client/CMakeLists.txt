# include($ENV{SR_BUILDSYSTEM_ENV_FILEPATH}/integration/onecode.environment.cmake)
include(project_default_template)

set(SR_TEMPLATE "StaticLib")

if(WIN32)
    append(
        SR_PROJECT_REQUESTED_LINK_TARGETS
            LINK_CORE_WINDOWS)
else()
    append(
        SR_PROJECT_REQUESTED_LINK_TARGETS
            LINK_CORE_LINUX)
endif()

append(
    SR_PROJECT_REQUESTED_LINK_TARGETS
        LINK_CORE_BASE
        LINK_ASIO)

include(project_setup)

add_dependencies(tcp_server_client_GeneratePCH INSTALL_property_system)
add_dependencies(tcp_server_client             tcp_server_client_GeneratePCH)
