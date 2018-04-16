cmake_minimum_required(VERSION 3.9)

include(../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "SharedLib")

set(
	SHIRABE_EXCLUDED_SOURCES 
		ApplicationMain.cpp
		Serialization/JSONObjectSerializer.cpp
		Serialization/XMLObjectSerializer.cpp
)

set(LINK_JSON TRUE)
set(LINK_XML2 TRUE)

include(GenericProject)
