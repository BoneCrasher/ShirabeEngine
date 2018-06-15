cmake_minimum_required(VERSION 3.9)

add_subdirectory(Modules/Platform)
add_subdirectory(Modules/OS)
add_subdirectory(Modules/Log)
add_subdirectory(Modules/Math)
add_subdirectory(Modules/Core)
add_subdirectory(Modules/Utility)
add_subdirectory(Modules/WSI)
add_subdirectory(Modules/Resources)
add_subdirectory(Modules/Rendering)
add_subdirectory(Modules/GraphicsAPI)
add_subdirectory(Modules/Vulkan)
add_subdirectory(Modules/Asset)

include(../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "SharedLib")

set(
	SHIRABE_EXCLUDED_SOURCES 
		ApplicationMain.cpp
		Serialization/JSONObjectSerializer.cpp
		Serialization/XMLObjectSerializer.cpp
)

set(
	SHIRABE_EMBED_MODULE_NAMES
		OS
		Core
		Log
		Math 
		Utility
		WSI
		Resources
		Rendering
		GraphicsAPI
		Vulkan
		Asset
)

set(LINK_SHIRABEMODULEVULKAN      TRUE)
set(LINK_SHIRABEMODULERENDERING   TRUE)
set(LINK_SHIRABEMODULEGRAPHICSAPI TRUE)
set(LINK_SHIRABEMODULERESOURCES   TRUE)
set(LINK_SHIRABEMODULEASSET       TRUE)
set(LINK_SHIRABEMODULEWSI         TRUE)
set(LINK_SHIRABEMODULECORE        TRUE)
set(LINK_SHIRABEMODULEUTILITY     TRUE)
set(LINK_SHIRABEMODULELOG         TRUE)
set(LINK_SHIRABEMODULEMATH        TRUE)
set(LINK_SHIRABEMODULEOS          TRUE)
set(LINK_SHIRABEMODULEPLATFORM    TRUE)
set(LINK_VULKANSDK                TRUE)
set(LINK_STBI                     TRUE)
set(LINK_FXGLTF                   TRUE)
set(LINK_JSON                     TRUE)
set(LINK_XML2                     TRUE)

include(GenericProject)

add_dependencies(ShirabeEngine INSTALL_Platform)
add_dependencies(ShirabeEngine INSTALL_OS)
add_dependencies(ShirabeEngine INSTALL_Log)
add_dependencies(ShirabeEngine INSTALL_Math)
add_dependencies(ShirabeEngine INSTALL_Core)
add_dependencies(ShirabeEngine INSTALL_Utility)
add_dependencies(ShirabeEngine INSTALL_WSI)
add_dependencies(ShirabeEngine INSTALL_Resources)
add_dependencies(ShirabeEngine INSTALL_GraphicsAPI)
add_dependencies(ShirabeEngine INSTALL_Rendering)
add_dependencies(ShirabeEngine INSTALL_Vulkan)
add_dependencies(ShirabeEngine INSTALL_Asset)