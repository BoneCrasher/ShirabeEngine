#!/bin/bash

buildOne ()
{

    cd ${source_directory}

    # make clean
    # make distclean

    local zlib_dir=${DEPLOY_BASE_DIR}/zlib/linux${addressmode}/${configuration}

    local Compiler=i386
    if [ ${addressmode} -eq 64 ]; then
            Compiler=x86_64
    fi

    ./autogen.sh                         \
        --prefix=${deploy_directory}     \
        --with-python=no                 \
        --with-zlib=${zlib_dir}          \
        --enable-static=yes              \
        --enable-shared=no               \
        --with-pic                       \
        --build=${Compiler}-pc-linux-gnu

    make -j12
    make install


    cd ${THIS}
}

buildOne
