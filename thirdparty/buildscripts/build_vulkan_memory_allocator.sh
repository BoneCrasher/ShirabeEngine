#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    export ASFLAGS=--${addressmode}

    cd ${build_directory}

    local vulkan_headers=${DEPLOY_BASE_DIR}/vulkan_headers/linux${addressmode}/${configuration}
    local vulkan_sdk=${DEPLOY_BASE_DIR}/vulkan_sdk/linux${addressmode}/${configuration}

    cmake                                                  \
        -DVulkan_LIBRARY="${vulkan_sdk}/lib"               \
        -DVulkan_INCLUDE_DIR="${vulkan_headers}/include"   \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}"       \
        -DCMAKE_BUILD_TYPE=${configuration}                \
        ${source_directory}

    # cmake -LA | awk '{if(f)print} /-- Cache values/{f=1}'

    cmake --build . -- -j12
    cmake --build . --target install
    
    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
