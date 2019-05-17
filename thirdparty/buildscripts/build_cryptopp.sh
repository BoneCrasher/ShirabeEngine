#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    export CC=gcc-8
    export CPP=g++-8 
    export CXX=g++-8 
    export LD=g++-8
    
    export CFLAGS="-m${addressmode} -std=c++17"
    export CXXFLAGS="-m${addressmode} -std=c++17 -ffast-math"
    
    export PREFIX=${deploy_directory}
    
    mkdir ${deploy_directory}/include
    mkdir ${deploy_directory}/lib
    
    # cd ${build_directory}
    cd ${source_directory}
    
    make -f GNUmakefile -j12
    make -f GNUmakefile install
    make -f GNUmakefile distclean
    make -f GNUmakefile clean
    
    # Make sure to reset exported variables
    export CFLAGS=
    export CXXFLAGS=

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
