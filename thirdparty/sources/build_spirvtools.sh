#!/bin/bash

THIS=`pwd -P`

buildOne () {
	BUILD_DIR=${THIS}/../_build/${1}${2}/${3}
	INSTALL_DIR=${THIS}/../SPIRV-Tools/${1}${2}/${3}

	export CFLAGS="-m${2}"

	mkdir -p ${BUILD_DIR}
	cd ${BUILD_DIR}
	cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DCMAKE_BUILD_TYPE=${3} ${THIS}/SPIRV-Tools
	cmake --build . -j12
	cmake --build . --target install 
    cd ${THIS}
}

#buildOne linux 32 debug
buildOne linux 32 release
#buildOne linux 64 debug
#buildOne linux 64 release
