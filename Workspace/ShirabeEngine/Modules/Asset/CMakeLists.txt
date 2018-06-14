cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "StaticLib")

set(LINK_SHIRABEMODULEPLATFORM    TRUE)
set(LINK_SHIRABEMODULECORE        TRUE)
set(LINK_SHIRABEMODULEUTILITY     TRUE)
set(LINK_SHIRABEMODULEGRAPHICSAPI TRUE)
set(LINK_STBI                     TRUE)
set(LINK_FXGLTF                   TRUE)
set(LINK_JSON                     TRUE)
set(LINK_XML2                     TRUE)

include(GenericProject)

add_dependencies(Asset INSTALL_Platform)
add_dependencies(Asset INSTALL_Core)
add_dependencies(Asset INSTALL_Utility)
add_dependencies(Asset INSTALL_GraphicsAPI)
