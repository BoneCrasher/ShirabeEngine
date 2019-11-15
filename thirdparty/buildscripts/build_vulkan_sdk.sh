#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    export ASFLAGS=--${addressmode}

    cd ${build_directory}

    # Prepare
    # ${source_directory}/update_external_sources.sh --${addressmode} --${configuration}

    local vulkan_headers=${DEPLOY_BASE_DIR}/vulkan_headers/linux${addressmode}/${configuration}

    cmake                                              \
        -DBUILD_LOADER=ON                              \
        -DBUILD_DEMOS=OFF                              \
        -DBUILD_TESTS=OFF                              \
        -DBUILD_WSI_XLIB_SUPPORT=ON                    \
        -DBUILD_WSI_XCB_SUPPORT=ON                     \
        -DBUILD_WSI_WAYLAND_SUPPORT=OFF                \
        -DVULKAN_HEADERS_INSTALL_DIR=${vulkan_headers} \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}"   \
        -DCMAKE_BUILD_TYPE=${configuration}            \
        ${source_directory}

    # cmake -LA | awk '{if(f)print} /-- Cache values/{f=1}'

    cmake --build . -- -j12
    cmake --build . --target install

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
