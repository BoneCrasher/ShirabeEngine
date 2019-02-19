#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    export CFLAGS="-m${addressmode}"
    export CXXFLAGS="-m${addressmode}"

    export ASFLAGS=--${addressmode}

    cd ${build_directory}

    # Prepare
    ${source_directory}/update_external_sources.sh --${addressmode} --${configuration}

    cmake                                              \
        -DBUILD_WSI_XLIB_SUPPORT=ON                    \
        -DBUILD_TESTS=OFF                              \
        -DDBUILD_WSI_XCB_SUPPORT=OFF                   \
        -DBUILD_WSI_WAYLAND_SUPPORT=OFF                \
        -DBUILD_DEMOS=OFF                              \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}"   \
        -DCMAKE_BUILD_TYPE=${configuration}            \
        ${source_directory}

    cmake --build . -- -j12
    cmake --build . --target install

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
