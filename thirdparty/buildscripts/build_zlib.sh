#!/bin/bash

buildOne ()
{
    cd ${source_directory}

    # make clean
    # make distclean

    local x64flag=""
    local arch="i386"
    if [ ${addressmode} -eq 64 ]; then
        x64flag="--64"
        arch="x86_64"
    fi

    ./configure                      \
        --prefix=${deploy_directory} \
        ${x64flag}

    make -j12
    make check
    make install

    cd ${THIS}
}

buildOne
