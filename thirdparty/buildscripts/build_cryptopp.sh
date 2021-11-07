#!/bin/bash

THIS=`pwd -P`

buildOne ()
{    
    export CXXFLAGS="${CXXFLAGS} -ffast-math"
    
    export PREFIX=${deploy_directory}
    
    if [!-d ${deploy_directory}/source]; then
        mkdir ${deploy_directory}/source
    fi

    if [!-d ${deploy_directory}/lib]; then
        mkdir ${deploy_directory}/lib
    fi
    
    # cd ${build_directory}
    cd ${source_directory}
    
    make -f GNUmakefile -j12
    make -f GNUmakefile install
    make -f GNUmakefile distclean
    make -f GNUmakefile clean
    
    # Make sure to reset exported variables

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
