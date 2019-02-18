#!/bin/bash

THIS=`pwd -P`

buildOne () {
	BUILD_DIR=${THIS}/../_build/${1}${2}/${3}
	INSTALL_DIR=${THIS}/../SPIRV-Cross/${1}${2}/${3}

	export CFLAGS="-m${2}"

	mkdir -p ${BUILD_DIR}
	cd ${BUILD_DIR}
	cmake -DSPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS=ON -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DCMAKE_BUILD_TYPE=${3} ${THIS}/SPIRV-Cross
	cmake --build .
	cmake --build . --target install
    cd ${THIS}
}

buildOne linux 32 debug
buildOne linux 32 release
buildOne linux 64 debug
buildOne linux 64 release
