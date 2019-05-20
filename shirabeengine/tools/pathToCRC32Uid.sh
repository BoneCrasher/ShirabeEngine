#!/bin/bash

THIS=$(pwd -P)

function convertOne
{
    crc32 <(echo $1) | tr a-z A-Z | xargs echo "ibase=16;" | bc
}

FILES=(
            "./shaders/standard/standard.mat.index"               
            "./shaders/standard/standard.mat.signature"           
            "./shaders/standard/standard.mat.config"              
            "./shaders/standard/standard.vert.glsl.spv"           
            "./shaders/standard/standard.frag.glsl.spv"           
            "./shaders/standard/standard.mat.instance_0000.index" 
            "./shaders/standard/standard.mat.instance_0000.config"
      )   
      
for lFile in ${FILES[@]}; do
    # convertOne ${lFile}
    /home/dotti/workspaces/shirabeengine/shirabeengine/_deploy/linux64/debug/bin/crc32generator_x64d -i=${lFile}
done;
