#!/bin/bash

THIS=$(pwd -P)
cd /usr/bin
dot -v -Tpng -o ${THIS}/Test.png ${THIS}/FrameGraphTest.gv
cd ${THIS}
