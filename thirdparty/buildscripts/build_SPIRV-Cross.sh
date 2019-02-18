#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    export CFLAGS="-m${addressmode}"

    cd ${build_directory}

    cmake                                              \
        -DSPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS=ON      \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}"   \
        -DCMAKE_BUILD_TYPE=${configuration}            \
        ${source_directory}

    cmake --build . -- -j12
    cmake --build . --target install

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
