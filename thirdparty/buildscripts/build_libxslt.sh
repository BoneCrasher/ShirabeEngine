#!/bin/bash

buildOne ()
{

    cd ${source_directory}

    # make clean
    # make distclean

    local libxml2_dir=${DEPLOY_BASE_DIR}/libxml2/linux${addressmode}/${configuration}
    local zlib_dir=${DEPLOY_BASE_DIR}/zlib/linux${addressmode}/${configuration}

    export LDFLAGS="-L${libxml2_dir}/lib -lxml2"

    local Compiler=i386
    if [ ${addressmode} -eq 64 ]; then
            Compiler=x86_64
    fi

    ./autogen.sh                                                    \
        --prefix=${deploy_directory}                                \
        --with-python=no                                            \
        --with-libxml-prefix=${libxml2_dir}                         \
        --with-libxml-include-prefix=${libxml2_dir}/include/libxml2 \
        --with-libxml-libs-prefix=${libxml2_dir}/lib                \
        --enable-static=yes                                         \
        --enable-shared=no                                          \
        --with-pic                                                  \
        --build=${Compiler}-pc-linux-gnu

    make -j12
    make install


    cd ${THIS}
}

buildOne
