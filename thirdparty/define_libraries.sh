#!/bin/bash

LIBRARIES=(                             \
           zlib                         \
           libxml2                      \
           libxslt                      \
           nlohmann_json                \
           googletest                   \
           asio                         \
           stb                          \
           assimp                       \
           glslang                      \
           spirv_tools                  \
           spirv_cross                  \
           vulkan_headers               \
           vulkan_sdk                   \
           vulkan_sdk_validation_layers \
          )

DEPENDENCIES_libxml2=( zlib )
DEPENDENCIES_libxslt=( zlib libxml2 )
DEPENDENCIES_vulkan_sdk=( googletest vulkan_headers )
DEPENDENCIES_vulkan_sdk_validation_layers=( googletest glslang spirv_tools vulkan_headers vulkan_sdk )
