#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    # mkdir -p ${deploy_directory}/include/nlohmann_json

    # cp -rv ${source_directory}/include/nlohmann ${deploy_directory}/include

    export CFLAGS="-m${addressmode}"
    export CXXFLAGS="-m${addressmode}"

    cd ${build_directory}

    echo "${CFLAGS} ${CXXFLAGS}"

    cmake                                             \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}"  \
        -DCMAKE_BUILD_TYPE=${configuration}           \
        -DJSON_BuildTests=OFF                         \
        ${source_directory}

    cmake --build . -- -j12
    cmake --build . --target install

    # Make sure to reset exported variables
    export CFLAGS=
    export CXXFLAGS=

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
