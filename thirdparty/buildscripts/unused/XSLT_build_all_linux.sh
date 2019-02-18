#!/bin/bash

THIS=`pwd -P`

buildOne () {
	IN_ZLIB_DIR=${THIS}/../zlib/deploy/${1}${2}/${3}
	IN_XML2_DIR=${THIS}/../libXML2/deploy/${1}${2}/${3}

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

	./autogen.sh  --prefix=${THIS}/deploy/${1}${2}/${3} --with-python=no --with-crypto=no --with-zlib=${IN_ZLIB_DIR} --enable-static=yes --enable-shared=no --with-libxml-prefix=${IN_XML2_DIR} --with-libxml-include-prefix=${IN_XML2_DIR}/include --with-libxml-libs-prefix=${IN_XML2_DIR}/lib --with-pic --build=${Compiler}-pc-linux-gnu 
	make -j12
	make install

}

cd libxslt-1.1.26

buildOne $1 $2 $3

cd ..
