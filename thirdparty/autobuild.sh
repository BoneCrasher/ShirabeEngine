#!/bin/bash

THIS=$(pwd -P)

./setup.sh
./buildLibraries.sh -a 64 -c debug
./buildLibraries.sh -a 64 -c release

# rm -rf ${THIS}/_build
# rm -rf ${THIS}/sources

