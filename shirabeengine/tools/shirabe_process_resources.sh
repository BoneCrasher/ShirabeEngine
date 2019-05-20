#!/bin/bash

THIS=$(pwd -P)

RESOURCES_ROOT_DIR="."
PRECOMPILER_PATH="shirabe_shader_precompiler_x64d"

function usage
{
    echo "Usage: \n"
    echo "    ./shirabe_process_resources.sh -r <path>"
}

function read_arguments
{
    printf "\nProcessing script arguments\n"

    # <option>   No argument
    # <option>:  Required
    # <option>:: Optional
    SHRT_OPTIONS=r:
    LONG_OPTIONS=resources_root:

    # -use ! and PIPESTATUS to get exit code with errexit set
    # -temporarily store output to be able to check for errors
    # -activate quoting/enhanced mode (e.g. by writing out “--options”)
    # -pass arguments only via   -- "$@"   to separate them correctly

    ! PARSED=$(getopt --options=$SHRT_OPTIONS --longoptions=$LONG_OPTIONS --name "$0" -- "$@")

    if [[ ${PIPESTATUS[0]} -ne 0 ]]; then
        # e.g. return value is 1
        #  then getopt has complained about wrong arguments to stdout
        exit 2
    fi

    # read getopt’s output this way to handle the quoting right:
    eval set -- "$PARSED"

    # now enjoy the options in order and nicely split until we see --
    while true; do
        case "$1" in
            -r|--resources_root)
                printf "Processing resources in ${2}\n"
                
                shift 2
                ;;                
            --)
                break
                ;;
            *)
                usage
                exit -1
                ;;
        esac
    done
}

read_arguments $@

#
# Process all materials
#
${THIS}/${PRECOMPILER_PATH}                                               \
    --verbose                                                             \
    --debug                                                               \
    --optimize                                                            \
    -I=./data/input/resources/shaders/include                             \
    -om=./data/output/resources/shaders/standard                          \
    -oi=./data/output/resources/shaders/standard/standard.mat.index       \
    -os=./data/output/resources/shaders/standard/standard.mat.signature   \
    -oc=./data/output/resources/shaders/standard/standard.mat.config      \
    -i=./data/input/resources/shaders/standard/standard.mat.index         
    
# 
# Copy all non-generated files
#
cp -f ${THIS}/data/input/resources/game.assetindex.xml                                ${THIS}/data/output/resources
cp -f ${THIS}/data/input/resources/shaders/standard/standard.mat.instance_0000.index  ${THIS}/data/output/resources/shaders/standard
cp -f ${THIS}/data/input/resources/shaders/standard/standard.mat.instance_0000.config ${THIS}/data/output/resources/shaders/standard
