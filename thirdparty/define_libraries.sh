#!/bin/bash

LIBRARIES=(              \
           zlib          \
           libxml2       \
           libxslt       \
           nlohmann_json \
           googletest    \
           asio          \
           stb           \
           assimp        \
           glslang       \
           SPIRV-Tools   \
           SPIRV-Cross   \
          )

DEPENDENCIES_libxml2=( zlib )
DEPENDENCIES_libxslt=( zlib libxml2 )
