# include($ENV{SR_BUILDSYSTEM_ENV_FILEPATH}/integration/onecode.environment.cmake)
include(project_default_template)

set(SR_TEMPLATE "Application")

set(
    SR_PROJECT_REQUESTED_LINK_TARGETS
        LINK_PROPERTY_SYSTEM
        LINK_COMPONENTS_TCP_SERVER_CLIENT)

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
        LINK_NLOHMANNJSON
        LINK_ASIO)

include(project_setup)

add_dependencies(property_server_GeneratePCH INSTALL_tcp_server_client)
add_dependencies(property_server             property_server_GeneratePCH)
