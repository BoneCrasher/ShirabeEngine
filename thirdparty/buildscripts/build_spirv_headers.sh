#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    export CFLAGS="-m${addressmode}"
    export CXXFLAGS="-m${addressmode}"

    export ASFLAGS=--${addressmode}

    cd ${build_directory}

    cmake                                                  \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}"       \
        -DCMAKE_BUILD_TYPE=${configuration}                \
        ${source_directory}

    # cmake -LA | awk '{if(f)print} /-- Cache values/{f=1}'

    cmake --build . -- -j12
    cmake --build . --target install

    # Make sure to reset exported variables
    export CFLAGS=
    export CXXFLAGS=

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
