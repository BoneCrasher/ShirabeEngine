#!/bin/bash

THIS=`pwd -P`

buildOne ()
{

    cd ${build_directory}

    echo "${CFLAGS} ${CXXFLAGS}"

    NLOHMANN_DIR=${DEPLOY_BASE_DIR}/nlohmann_json/linux${addressmode}/${configuration}/lib/cmake/nlohmann_json/

    cmake                                                                                          \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}"                                               \
        -DCMAKE_BUILD_TYPE=${configuration}                                                        \
        -Dnlohmann_json_DIR=${NLOHMANN_DIR}                                                        \
        -DBUILD_TESTING=OFF                                                                        \
        ${source_directory}

    cmake --build . -- -j12
    cmake --build . --target install

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
