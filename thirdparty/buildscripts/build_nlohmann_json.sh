#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    mkdir -p ${deploy_directory}/include/nlohmann_json

    cp -rv ${source_directory}/include/nlohmann ${deploy_directory}/include
}

buildOne linux ${addressmode} ${configuration}
