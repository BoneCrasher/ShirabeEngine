#!/bin/bash

THIS=$(pwd -P)

SOURCES_DIR=${THIS}/sources

declare -A REPOSITORIES
REPOSITORIES[gtest]="git@github.com:google/googletest.git"
REPOSITORIES[glslang]="git@github.com:KhronosGroup/glslang.git"
REPOSITORIES[spirv_cross]="git@github.com:KhronosGroup/SPIRV-Cross.git"
REPOSITORIES[spirv_tools]="git@github.com:KhronosGroup/SPIRV-Tools.git"
REPOSITORIES[spirv_headers]="git@github.com:KhronosGroup/SPIRV-Headers.git"
REPOSITORIES[spirv_effcee]="git@github.com:google/effcee.git"
REPOSITORIES[spirv_re2]="git@github.com:google/re2.git"
REPOSITORIES[spirv_gtest]="git@github.com:google/googletest.git"

REPOSITORIES[xml2]="git@github.com:GNOME/libxml2.git"
REPOSITORIES[xslt]="git@github.com:GNOME/libxslt.git"
REPOSITORIES[json]="git@github.com:nlohmann/json.git"
REPOSITORIES[asio]="git@github.com:chriskohlhoff/asio.git"
REPOSITORIES[stb]="git@github.com:nothings/stb.git"
REPOSITORIES[assimp]="git@github.com:assimp/assimp.git"

declare -A TARGET_DIRECTORIES
TARGET_DIRECTORIES[gtest]="${SOURCES_DIR}/googletest"
TARGET_DIRECTORIES[glslang]="${SOURCES_DIR}/glslang"
TARGET_DIRECTORIES[spirv_cross]="${SOURCES_DIR}/SPIRV-Cross"
TARGET_DIRECTORIES[spirv_tools]="${SOURCES_DIR}/SPIRV-Tools"
TARGET_DIRECTORIES[spirv_headers]="${SOURCES_DIR}/SPIRV-Tools/external/spirv-headers"
TARGET_DIRECTORIES[spirv_effcee]="${SOURCES_DIR}/SPIRV-Tools/external/effcee"
TARGET_DIRECTORIES[spirv_r2]="${SOURCES_DIR}/SPIRV-Tools/external/r2"
TARGET_DIRECTORIES[spirv_gtest]="${SOURCES_DIR}/SPIRV-Tools/external/googletest"

TARGET_DIRECTORIES[xml2]="${SOURCES_DIR}/libxml2"
TARGET_DIRECTORIES[xslt]="${SOURCES_DIR}/libxslt"
TARGET_DIRECTORIES[json]="${SOURCES_DIR}/nlohmann_json"
TARGET_DIRECTORIES[asio]="${SOURCES_DIR}/asio"
TARGET_DIRECTORIES[stb]="${SOURCES_DIR}/stb"
TARGET_DIRECTORIES[assimp]="${SOURCES_DIR}/assimp"

function test_env
{
    if [[ $EUID -ne 0 ]]; then
       echo "This script must be run as root"
       exit 1
    fi
}

function setup_env
{
    if [ -d ${SOURCES_DIR} ]; then
        rm -rf ${SOURCES_DIR}
    fi

    mkdir -p ${SOURCES_DIR}
}

function setup_one
{
    printf "Setting up repository ${1}\n"

    local key=${1}
    local repo="${REPOSITORIES[${key}]}"
    local directory="${TARGET_DIRECTORIES[${key}]}"

    printf "Command: git clone ${repo} ${directory}\n"

    git clone ${repo} ${directory}
}

function setup_base
{
    setup_one gtest
}

function setup_spirv_tools
{
    setup_one spirv_tools

    mkdir -p ${SOURCES_DIR}/SPIRV-Tools/external

    setup_one spirv_headers
    setup_one spirv_effcee
    setup_one spirv_re2
    setup_one spirv_gtest
}

function setup_spirv_cross
{
    setup_one spirv_cross
}

function setup_glslang
{
    setup_one glslang
}

function setup_tools
{
    setup_one xml2
    setup_one xslt
    setup_one json
    setup_one asio
    setup_one stb
    setup_one assimp
}

function setup_vulkan
{
    mkdir vulkan_sdk

    local vulkan_sdk_tar_name="vulkan-sdk.tar.gz"

    wget https://sdk.lunarg.com/sdk/download/latest/linux/${vulkan_sdk_tar_name}
    tar -C vulkan_sdk -xvzf ${vulkan_sdk_tar_name}
    find vulkan_sdk/ -mindepth 1 -maxdepth 1 -type d | xargs -I {} mv {} vulkan_sdk/version
}

#test_env
#setup_base
#setup_spirv_tools
#setup_spirv_cross
#setup_glslang
#setup_tools
setup_vulkan
