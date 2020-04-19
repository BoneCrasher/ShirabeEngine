cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULECORE
        VULKANHEADERS)

set(
    SHIRABE_EXCLUDED_HEADERS
        graphicsapi/resources/types_old/all.h
        graphicsapi/resources/types_old/buffer.h
        graphicsapi/resources/types_old/bufferview.h
        graphicsapi/resources/types_old/constantbuffer.h
        graphicsapi/resources/types_old/definition.h
        graphicsapi/resources/types_old/depthstencilstate.h
        graphicsapi/resources/types_old/device.h
        graphicsapi/resources/types_old/framebuffer.h
        graphicsapi/resources/types_old/instancebuffer.h
        graphicsapi/resources/types_old/mesh.h
        graphicsapi/resources/types_old/objectbuffer.h
        graphicsapi/resources/types_old/pipeline.h
        graphicsapi/resources/types_old/rasterizerstate.h
        graphicsapi/resources/types_old/renderpass.h
        graphicsapi/resources/types_old/shader.h
        graphicsapi/resources/types_old/structuredbuffer.h
        graphicsapi/resources/types_old/swapchain.h
        graphicsapi/resources/types_old/texture.h
        graphicsapi/resources/types_old/textureview.h
        graphicsapi/resources/gfxapi.h
        graphicsapi/resources/gfxapiresourcebackend.h
        graphicsapi/resources/gfxapiresourceproxy.h
        graphicsapi/resources/gfxapiresourcetaskbackend.h
)
set(
        SHIRABE_EXCLUDED_SOURCES
        types/buffer.cpp
        types/bufferview.cpp
        types/depthstencilstate.cpp
        types/framebuffer.cpp
        types/mesh.cpp
        types/pipeline.cpp
        types/rasterizerstate.cpp
        types/renderpass.cpp
        types/swapchain.cpp
        types/texture.cpp
        types/textureview.cpp
        gfxapiresourcebackend.cpp
)

include(project_setup)

add_dependencies(graphicsapi INSTALL_base)
add_dependencies(graphicsapi INSTALL_platform)
add_dependencies(graphicsapi INSTALL_math)
add_dependencies(graphicsapi INSTALL_log)
add_dependencies(graphicsapi INSTALL_core)
