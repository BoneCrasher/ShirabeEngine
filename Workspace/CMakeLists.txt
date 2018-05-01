cmake_minimum_required(VERSION 3.9)

add_subdirectory(ShirabeEngine)
add_subdirectory(Test)

add_dependencies(Test INSTALL_ShirabeEngine)
