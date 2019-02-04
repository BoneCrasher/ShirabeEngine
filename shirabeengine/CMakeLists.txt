cmake_minimum_required(VERSION 3.9)

# Set global property (all targets are impacted)
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

# set(CMAKE_C_COMPILER               clang)
# set(CMAKE_CXX_COMPILER             clang++)
# set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "/usr/bin/iwyu;-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;args")

project(shirabedevelopment)

add_subdirectory(shirabeengine)
add_subdirectory(shirabe_shader_precompiler)
add_subdirectory(enginetest)
add_subdirectory(game)

add_dependencies(shirabe_shader_precompiler INSTALL_shirabeengine)
add_dependencies(enginetest                 INSTALL_shirabeengine)
add_dependencies(game                       INSTALL_shirabeengine)
add_dependencies(game                       INSTALL_shirabe_shader_precompiler)
        
