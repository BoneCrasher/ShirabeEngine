cmake_minimum_required(VERSION 3.9)

# Set global property (all targets are impacted)
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

# set(CMAKE_C_COMPILER               clang)
# set(CMAKE_CXX_COMPILER             clang++)
# set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "/usr/bin/iwyu;-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;args")

project(shirabedevelopment)

SET(SHIRABE_BUILDSYSTEM_ENV_FILEPATH     ${CMAKE_SOURCE_DIR}/buildsystem)
SET(SHIRABE_WORKSPACE_PATH_SHIRABEENGINE ${CMAKE_SOURCE_DIR})

add_subdirectory(shirabeengine)
        
