#!/bin/bash

THIS=`pwd -P`

buildOne ()
{
    export CFLAGS="-m${addressmode}"
    export CXXFLAGS="-m${addressmode}"

    export ASFLAGS=--${addressmode}

    cd ${build_directory}

    # Prepare
    # ${source_directory}/update_external_sources.sh --${addressmode} --${configuration}

    local target_config=linux${addressmode}/${configuration}

    local glslang_dir=${DEPLOY_BASE_DIR}/glslang/${target_config}
    local spirv_tools_dir=${DEPLOY_BASE_DIR}/spirv_tools
    local vulkan_headers_dir=${DEPLOY_BASE_DIR}/vulkan_headers/linux${addressmode}/${configuration}

    local spirv_tools_bin=${spirv_tools_dir}/${target_config}/bin
    local spirv_tools_inc=${spirv_tools_dir}/${target_config}/include
    local spirv_tools_lib=${spirv_tools_dir}/${target_config}/lib/libSPIRV-Tools.a
    local spirv_tools_opt=${spirv_tools_dir}/${target_config}/lib/libSPIRV-Tools-opt.a

    cmake                                                 \
        -DBUILD_LAYERS=ON                                 \
        -DBUILD_LAYER_SUPPORT_FILES=ON                    \
        -DBUILD_TESTS=OFF                                 \
        -DBUILD_WSI_XLIB_SUPPORT=ON                       \
        -DBUILD_WSI_XCB_SUPPORT=ON                        \
        -DBUILD_WSI_WAYLAND_SUPPORT=OFF                   \
        -VULKAN_HEADERS_INSTALL_DIR=${vulkan_headers_dir} \
        -DGLSLANG_INSTALL_DIR=${glslang_dir}              \
        -DSPIRV_TOOLS_BINARY_ROOT=${spirv_tools_bin}      \
        -DSPIRV_TOOLS_INCLUDE_DIR=${spirv_tools_inc}      \
        -DSPIRV_TOOLS_LIB=${spirv_tools_lib}              \
        -DSPIRV_TOOLS_OPT_BINARY_ROOT=${spirv_tools_bin}  \
        -DSPIRV_TOOLS_OPT_LIB=${spirv_tools_opt}          \
        -DCMAKE_INSTALL_PREFIX="${deploy_directory}"      \
        -DCMAKE_BUILD_TYPE=${configuration}               \
        ${source_directory}

    # cmake -LA | awk '{if(f)print} /-- Cache values/{f=1}'
    cmake --build . -- -j12
    cmake --build . --target install

    # Make sure to reset exported variables
    export CFLAGS=
    export CXXFLAGS=
    export ASFLAGS=

    cd ${THIS}
}

buildOne linux ${addressmode} ${configuration}
