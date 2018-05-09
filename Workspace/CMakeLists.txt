cmake_minimum_required(VERSION 3.9)

project(ShirabeDevelopment)

add_subdirectory(ShirabeEngine)
add_subdirectory(EngineTest)

add_dependencies(EngineTest INSTALL_ShirabeEngine)
        