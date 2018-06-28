cmake_minimum_required(VERSION 3.9)

project(shirabedevelopment)

add_subdirectory(shirabeengine)
add_subdirectory(enginetest)

add_dependencies(enginetest INSTALL_shirabeengine)
        
