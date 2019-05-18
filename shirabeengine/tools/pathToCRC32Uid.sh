#!/bin/bash

function convertOne
{
    crc32 <(echo $1) | tr a-z A-Z | xargs -t echo "ibase=16; " | bc
}

FILES=(
            "./data/output/resources/shaders/standard/standard.mat.index"
            "./data/output/resources/shaders/standard/standard.mat.signature"
            "./data/output/resources/shaders/standard/standard.mat.config"
            "./data/output/resources/shaders/standard/standard.vert.glsl.spv"
            "./data/output/resources/shaders/standard/standard.frag.glsl.spv"
      )   
      
for lFile in ${FILES[@]}; do
    convertOne ${lFile}
done;
