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
           fxgltf                       \
           glslang                      \
           spirv_headers                \
           spirv_cross                  \
           spirv_tools                  \
           spirv_tools_headers          \
           spirv_tools_effcee           \
           spirv_tools_re2              \
           spirv_tools_gtest            \
           spirv_reflect                \
           cryptopp                     \
           vulkan_headers               \
           vulkan_sdk                   \
           vulkan_sdk_validation_layers \
           fmt                          \
          )

DEPENDENCIES_fxgltf=(nlohmann_json)
DEPENDENCIES_libxml2=( zlib )
DEPENDENCIES_libxslt=( zlib libxml2 )
DEPENDENCIES_vulkan_sdk=( googletest vulkan_headers )
DEPENDENCIES_vulkan_sdk_validation_layers=( googletest glslang spirv_tools vulkan_headers vulkan_sdk )
