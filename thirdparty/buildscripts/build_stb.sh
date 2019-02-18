#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    mkdir ${deploy_directory}/include

    cp -v ${source_directory}/*.h ${deploy_directory}/include/
}

buildOne linux ${addressmode} ${configuration}
