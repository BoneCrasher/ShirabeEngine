#!/bin/bash

THIS=`pwd -P`

buildOne () {
	IN_ZLIB_DIR=${THIS}/../zlib/deploy/${1}${2}/${3}

	make clean
	make distclean	
	export CFLAGS="-m${2}"
	# export CPPFLAGS="-I${THIS}/../zlib-1.2.11/deploy/${1}${2}/${2}/include"
	# export LDFLAGS="-L${THIS}/../zlib-1.2.11/deploy/${1}${2}/${2}/lib"
	# export LIBS="-lz"

	Compiler=i386
	if [ ${2} -eq 64 ]; then
		Compiler=x86_64
	fi

	./autogen.sh  --prefix=${THIS}/deploy/${1}${2}/${3} --with-python=no --with-zlib=${IN_ZLIB_DIR} --enable-static=yes --enable-shared=no --with-pic --build=${Compiler}-pc-linux-gnu 
	make -j12
	make install

}

cd libxml2-2.7.8

buildOne $1 $2 $3

cd ..
