#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    mkdir -p ${deploy_directory}/include/stb

    cp -v ${source_directory}/*.h ${deploy_directory}/include/stb
}

buildOne linux ${addressmode} ${configuration}
