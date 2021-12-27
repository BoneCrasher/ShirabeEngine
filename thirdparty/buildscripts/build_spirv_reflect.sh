#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    cd ${build_directory}

    # cmake                                            \
    #     -DCMAKE_INSTALL_PREFIX="${deploy_directory}" \
    #     -DCMAKE_BUILD_TYPE=${configuration}          \
    #     ${source_directory}

    # cmake --build . -- -j12
    # cmake --build . --target install

    if [ ! -d ${deploy_directory}/source ]; then
        mkdir ${deploy_directory}/source
    fi
    
    cp ${source_directory}/spirv_reflect.h ${deploy_directory}/source/
    cp ${source_directory}/spirv_reflect.c ${deploy_directory}/source/
    cp -r ${source_directory}/include ${deploy_directory}

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
