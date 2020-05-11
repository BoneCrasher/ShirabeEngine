#!/bin/bash

THIS=$(pwd -P)
cd /usr/bin
dot -v -Tpng -o ${THIS}/Test.png ${THIS}/RenderGraphTest.gv
cd ${THIS}
