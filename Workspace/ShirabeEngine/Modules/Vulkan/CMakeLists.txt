cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "Module")

set(LINK_SHIRABEMODULERESOURCES   TRUE)
set(LINK_SHIRABEMODULEGRAPHICSAPI TRUE)
set(LINK_SHIRABEMODULEOS          TRUE)
set(LINK_SHIRABEMODULEMATH        TRUE)
set(LINK_SHIRABEMODULEPLATFORM    TRUE)
set(LINK_SHIRABEMODULECORE        TRUE)
set(LINK_SHIRABEMODULELOG         TRUE)
set(LINK_VULKANSDK                TRUE)

include(GenericProject)

add_dependencies(Vulkan INSTALL_Log)
add_dependencies(Vulkan INSTALL_Platform)
add_dependencies(Vulkan INSTALL_Core)
add_dependencies(Vulkan INSTALL_OS)
add_dependencies(Vulkan INSTALL_Math)
add_dependencies(Vulkan INSTALL_Resources)
add_dependencies(Vulkan INSTALL_GraphicsAPI)