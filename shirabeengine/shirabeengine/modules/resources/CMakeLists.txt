cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(SHIRABE_EXCLUDED_HEADERS
        resources/code/include/resources/old/eresourcetype.h
        resources/code/include/resources/old/iresource.h
        resources/code/include/resources/old/iresourceproxy.h
        resources/code/include/resources/old/requestdefaultimplementation.h
        resources/code/include/resources/old/resourcebackendproxy.h
        resources/code/include/resources/old/resourcedomaintransfer.h
        resources/code/include/resources/old/resourcemanagerbase.h
        resources/code/include/resources/old/resourcepool.h
        resources/code/include/resources/old/resourceproxyfactory.h
        resources/code/include/resources/old/resourcetraits.h
)

set(SHIRABE_EXCLUDED_SOURCES
        old/resourcedatasource.cpp
        old/resourcemanagerbase.cpp)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEASSET
        SHIRABEMODULEGRAPHICSAPI
        SHIRABEMODULECORE     
        SHIRABEMODULEMATH     
        SHIRABEMODULELOG      
        SHIRABEMODULEBASE
        SHIRABEMODULEPLATFORM
        VULKANHEADERS
        JSON                  
        XML2
        ZLIB)

include(project_setup)

add_dependencies(resources INSTALL_asset)
add_dependencies(resources INSTALL_graphicsapi)
add_dependencies(resources INSTALL_math)
add_dependencies(resources INSTALL_log)
add_dependencies(resources INSTALL_core)
add_dependencies(resources INSTALL_base)
add_dependencies(resources INSTALL_platform)
