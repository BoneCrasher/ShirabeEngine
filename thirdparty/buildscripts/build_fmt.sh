#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    export CFLAGS="-m${addressmode}"
    export CXXFLAGS="-m${addressmode}"

    cd ${build_directory}

    echo "${CFLAGS} ${CXXFLAGS}"

    cmake                                                                                          \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}"                                               \
        -DCMAKE_BUILD_TYPE=${configuration}                                                        \
        ${source_directory}

    cmake --build . -- -j12
    cmake --build . --target install

    # Make sure to reset exported variables
    export CFLAGS=
    export CXXFLAGS=

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
