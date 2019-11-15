#!/bin/bash

THIS=`pwd -P`

buildOne ()
{

    cd ${build_directory}

    echo "${CFLAGS} ${CXXFLAGS}"

    cmake                                            \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}" \
        -DCMAKE_BUILD_TYPE=${configuration}          \
        -DASSIMP_ANDROID_JNIIOSYSTEM=OFF             \
        ${source_directory}

    cmake --build . -- -j12
    cmake --build . --target install

    # Make sure to reset exported variables

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
