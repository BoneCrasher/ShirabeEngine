#!/bin/bash

THIS=$(pwd -P)

SOURCES_DIR=${THIS}/sources
DEPLOY_DIR=${THIS}/_deploy

declare -A REPOSITORIES
REPOSITORIES[zlib]="git@github.com:madler/zlib.git"
REPOSITORIES[libxml2]="git@github.com:GNOME/libxml2.git"
REPOSITORIES[libxslt]="git@github.com:GNOME/libxslt.git"
REPOSITORIES[nlohmann_json]="git@github.com:nlohmann/json.git"
REPOSITORIES[googletest]="git@github.com:google/googletest.git"
REPOSITORIES[asio]="git@github.com:chriskohlhoff/asio.git"
REPOSITORIES[stb]="git@github.com:nothings/stb.git"
REPOSITORIES[assimp]="git@github.com:assimp/assimp.git"
REPOSITORIES[fxgltf]="git@github.com:jessey-git/fx-gltf.git"
REPOSITORIES[glslang]="git@github.com:KhronosGroup/glslang.git"
REPOSITORIES[spirv_headers]="git@github.com:KhronosGroup/SPIRV-Headers.git"
REPOSITORIES[spirv_cross]="git@github.com:KhronosGroup/SPIRV-Cross.git"
REPOSITORIES[spirv_tools]="git@github.com:KhronosGroup/SPIRV-Tools.git"
REPOSITORIES[spirv_tools_headers]="git@github.com:KhronosGroup/SPIRV-Headers.git"
REPOSITORIES[spirv_tools_effcee]="git@github.com:google/effcee.git"
REPOSITORIES[spirv_tools_re2]="git@github.com:google/re2.git"
REPOSITORIES[spirv_tools_gtest]="git@github.com:google/googletest.git"
REPOSITORIES[cryptopp]="git@github.com:weidai11/cryptopp.git"
REPOSITORIES[vulkan_headers]="git@github.com:KhronosGroup/Vulkan-Headers.git"
REPOSITORIES[vulkan_sdk]="git@github.com:KhronosGroup/Vulkan-Loader.git"
REPOSITORIES[vulkan_sdk_validation_layers]="git@github.com:KhronosGroup/Vulkan-ValidationLayers.git"
REPOSITORIES[fmt]="git@github.com:fmtlib/fmt.git"

declare -A TARGET_DIRECTORIES
TARGET_DIRECTORIES[zlib]="${SOURCES_DIR}/zlib"
TARGET_DIRECTORIES[libxml2]="${SOURCES_DIR}/libxml2"
TARGET_DIRECTORIES[libxslt]="${SOURCES_DIR}/libxslt"
TARGET_DIRECTORIES[googletest]="${SOURCES_DIR}/googletest"
TARGET_DIRECTORIES[nlohmann_json]="${SOURCES_DIR}/nlohmann_json"
TARGET_DIRECTORIES[asio]="${SOURCES_DIR}/asio"
TARGET_DIRECTORIES[stb]="${SOURCES_DIR}/stb"
TARGET_DIRECTORIES[assimp]="${SOURCES_DIR}/assimp"
TARGET_DIRECTORIES[fxgltf]="${SOURCES_DIR}/fxgltf"
TARGET_DIRECTORIES[glslang]="${SOURCES_DIR}/glslang"
TARGET_DIRECTORIES[spirv_headers]="${SOURCES_DIR}/spirv_headers"
TARGET_DIRECTORIES[spirv_cross]="${SOURCES_DIR}/spirv_cross"
TARGET_DIRECTORIES[spirv_tools]="${SOURCES_DIR}/spirv_tools"
TARGET_DIRECTORIES[spirv_tools_headers]="${SOURCES_DIR}/spirv_tools/external/spirv-headers"
TARGET_DIRECTORIES[spirv_tools_effcee]="${SOURCES_DIR}/spirv_tools/external/effcee"
TARGET_DIRECTORIES[spirv_tools_re2]="${SOURCES_DIR}/spirv_tools/external/re2"
TARGET_DIRECTORIES[spirv_tools_gtest]="${SOURCES_DIR}/spirv_tools/external/googletest"
TARGET_DIRECTORIES[cryptopp]="${SOURCES_DIR}/cryptopp"
TARGET_DIRECTORIES[vulkan_headers]="${SOURCES_DIR}/vulkan_headers"
TARGET_DIRECTORIES[vulkan_sdk]="${SOURCES_DIR}/vulkan_sdk"
TARGET_DIRECTORIES[vulkan_sdk_validation_layers]="${SOURCES_DIR}/vulkan_sdk_validation_layers"
TARGET_DIRECTORIES[fmt]="${SOURCES_DIR}/fmt"


CAN_FETCH_BUILD_TOOLS=0

function clean
{
    if [ -d ${1} ]; then
        rm -rf ${1}
    fi
}

function test_env
{
    if [[ $EUID -ne 0 ]]; then
       echo "This script must be run as root if all software required to build should be installed."
    else
        CAN_FETCH_BUILD_TOOLS=1
    fi
}

function generate_dir
{
    if [ -d ${1} ]; then
        rm -rf ${1}
    fi

    mkdir -p ${1}
}

function setup_env
{
    generate_dir ${SOURCES_DIR}
    generate_dir ${DEPLOY_DIR}
}

function setup_one
{
    printf "Setting up repository ${1}\n"

    local key=${1}

    clean ${key}

    local repo="${REPOSITORIES[${key}]}"
    local directory="${TARGET_DIRECTORIES[${key}]}"

    printf "Command: git clone ${repo} ${directory}\n"

    git clone ${repo} ${directory}
}

function update_one
{
    printf "Updating repository ${1}\n"

    local key=${1}
    local dir=${TARGET_DIRECTORIES[${key}]}

    if [ ! -d ${dir} ]; then
        printf "Directory ${key} does not exist.\n"
    else
        cd ${dir}
        git pull
        cd ..
    fi
}

function setup_base
{
    setup_one googletest
}

function setup_spirv_tools
{
    setup_one spirv_tools

    mkdir -p ${SOURCES_DIR}/SPIRV-Tools/external

    setup_one spirv_tools_headers
    setup_one spirv_tools_effcee
    setup_one spirv_tools_re2
    setup_one spirv_tools_gtest
}

function setup_spirv_cross
{
    setup_one spirv_headers
    setup_one spirv_cross
}

function setup_glslang
{
    setup_one glslang
}

function setup_tools
{
    setup_one zlib
    setup_one libxml2
    setup_one libxslt
    setup_one nlohmann_json
    setup_one asio
    setup_one stb
    setup_one assimp
    setup_one fxgltf
    setup_one cryptopp
    setup_one fmt
}

function setup_vulkan
{
    # local vulkan_sdk_tar_name="vulkan-sdk.tar.gz"
    #
    # # Make sure 'tar' can extract
    # mkdir vulkan_sdk
    # mkdir _deploy
    #
    # # Download latest VK-SDK
    # wget https://sdk.lunarg.com/sdk/download/latest/linux/${vulkan_sdk_tar_name}
    # # Extract
    # tar -C vulkan_sdk -xvzf ${vulkan_sdk_tar_name}
    # # Rename to vulkan_sdk/version
    # find vulkan_sdk/ -mindepth 1 -maxdepth 1 -type d | xargs -I {} mv {} vulkan_sdk/version
    # # Move to deploy, as it will be expected to be there linking
    # mv vulkan_sdk _deploy

    setup_one vulkan_headers
    setup_one vulkan_sdk
    setup_one vulkan_sdk_validation_layers
}

function setup_vmem
{
    # copy all packaged dependencies to the sources directory during setup.
    cp -rf packaged/* sources 
}

# Check, whether we should update.
if [ ! -z ${1} ]; then

    if [ "${1}" = "--update" ]; then
        source ${THIS}/define_libraries.sh

        for library in "${LIBRARIES[@]}";
        do

            update_one ${library}

        done

        exit 1
    fi

fi

test_env
clean_env
setup_base
setup_spirv_tools
setup_spirv_cross
setup_glslang
setup_tools
setup_vulkan
setup_vmem
