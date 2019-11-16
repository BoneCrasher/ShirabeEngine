#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    export ASFLAGS=--${addressmode}

    cd ${build_directory}

    cmake                                                  \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}"       \
        -DCMAKE_BUILD_TYPE=${configuration}                \
        ${source_directory}

    # cmake -LA | awk '{if(f)print} /-- Cache values/{f=1}'

    cmake --build . -- -j12
    cmake --build . --target install
    
    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
