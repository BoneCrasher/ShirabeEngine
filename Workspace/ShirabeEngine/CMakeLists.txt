cmake_minimum_required(VERSION 3.9)

add_subdirectory(Modules/Log)
add_subdirectory(Modules/Math)
add_subdirectory(Modules/Platform)
add_subdirectory(Modules/Core)
add_subdirectory(Modules/Asset)
add_subdirectory(Modules/Utility)
add_subdirectory(Modules/Resources)
add_subdirectory(Modules/GraphicsAPI)
add_subdirectory(Modules/Vulkan)

include(../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "SharedLib")

set(
	SHIRABE_EXCLUDED_SOURCES 
		ApplicationMain.cpp
		Serialization/JSONObjectSerializer.cpp
		Serialization/XMLObjectSerializer.cpp
)

set(LINK_SHIRABEMODULEASSET)
set(LINK_SHIRABEMODULECORE)
set(LINK_SHIRABEMODULEUTILITY)
set(LINK_SHIRABEMODULEPLATFORM)
set(LINK_SHIRABEMODULELOG)
set(LINK_SHIRABEMODULEMATH)
set(LINK_STBI   TRUE)
set(LINK_FXGLTF TRUE)
set(LINK_JSON   TRUE)
set(LINK_XML2   TRUE)
set(LINK_VULKANSDK TRUE)

include(GenericProject)

add_dependencies(ShirabeEngine INSTALL_Log)
add_dependencies(ShirabeEngine INSTALL_Math)
add_dependencies(ShirabeEngine INSTALL_Platform)
add_dependencies(ShirabeEngine INSTALL_Core)
add_dependencies(ShirabeEngine INSTALL_Asset)
add_dependencies(ShirabeEngine INSTALL_Utility)
add_dependencies(ShirabeEngine INSTALL_Resources)
add_dependencies(ShirabeEngine INSTALL_GraphicsAPI)
add_dependencies(ShirabeEngine INSTALL_Vulkan)