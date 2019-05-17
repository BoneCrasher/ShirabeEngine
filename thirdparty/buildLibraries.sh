# Based on the answer of Robert Riemer @ Stackoverflow: https://stackoverflow.com/a/29754866/3754223

# getopt:           https://linux.die.net/man/1/getopt
# PIPESTATUS, etc.: https://www.tldp.org/LDP/abs/html/internalvariables.html

#!/bin/bash

THIS=$(pwd -P)
BUILD_SCRIPTS_DIR=${THIS}/buildscripts
LIBRARY_SOURCE_DIR=${THIS}/sources

BUILD_BASE_DIR=${THIS}/_build
DEPLOY_BASE_DIR=${THIS}/_deploy

mkdir -p ${BUILD_BASE_DIR}
mkdir -p ${DEPLOY_BASE_DIR}

DEBUG=0
VERBOSE=0
LIBRARIES=()
ADDRESS_MODES=( 32 64 )
CONFIGURATIONS=( debug release )

ALREADY_BUILT_LIBS=()

#---------------------------------------------------------------------
# Print usage
#---------------------------------------------------------------------
function usage 
{
    printf "Usage:                                                          \n"
    printf "    ./buildLibraries.sh <options>                               \n"
    printf "                                                                \n"
    printf "Options:                                                        \n"
    printf "    -d|--debug                                                  \n"
    printf "    -v|--verbose                                                \n"
    printf "    -l|--libraries <list>                                       \n"
    printf "    -a|--address_modes <list>                                   \n"
    printf "    -c|--configurations <list>                                  \n"
    printf "                                                                \n"
    printf "Example:                                                        \n"
    printf "    ./buildLibraries.sh -d -v -libraries asio -a 32 64 -c debug \n"
    printf "                                                                \n"
    
}  
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# Test environment
#---------------------------------------------------------------------
function test_env
{
    # Saner programming env: these switches turn some bugs into errors
    set -o errexit -o pipefail -o noclobber -o nounset

    # Check if we can read command line options or not. 
    ! getopt --test > /dev/null 
    if [[ ${PIPESTATUS[0]} -ne 4 ]]; then
        printf '`getopt --test` failed in this environment.\n'
        exit -2
    fi
    
    # Check, if there's a source directory.
    if [ ! -d ${1} ]; then 
        printf "Directory 'source' containing the third party library sources does not exist.\n"
        exit -3
    fi
}
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# Check, if a specific element is contained in the provided array.
#    Arg 1: Array
#    Arg 2: Test value
#---------------------------------------------------------------------
function has_element
{
    local search=${1}; shift
    local array=( "${@}" )

    local element_test
    for element_test in "${array[@]}";
    do
        printf "Testing against ${element_test} \n"

        if [ "${element_test}" = "${search}" ]; then
            printf "${search} contained in ${array[*]}.\n"
            return 0
        fi
    done
    
    return 1
}
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# Read all arguments
#---------------------------------------------------------------------
function read_arguments
{
    printf "\nProcessing script arguments\n"

    # <option>   No argument
    # <option>:  Required
    # <option>:: Optional
    SHRT_OPTIONS=d,v,l:a:c:
    LONG_OPTIONS=debug,verbose,libraries:,address_mode:,config:

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
            -d|--debug)
                printf "Enabling debug mode.\n"
                DEBUG=1
                
                shift
                ;;
            -v|--verbose)
                printf "Enabling verbose mode.\n"
                VERBOSE=1
                
                shift
                ;;
            -l|--libraries)
                # Verify, that the library names provided are available, before committing.
                local libraries_requested=${2}
                                
                local library
                for library in "${libraries_requested[@]}";
                do
                    if ! has_element ${library} ${LIBRARIES[@]}; then
                        printf "Error: Libary ${library} not available in sources directory.\n"
                    fi
                done
                
                LIBRARIES=(${libraries_requested[@]})
                
                shift 2
                ;;
            -a|--address_modes)
                local modes_requested=${2}
                
                local mode
                for mode in "${modes_requested[@]}";
                do
                    if ! has_element ${mode} ${ADDRESS_MODES[@]}; then
                            printf "Error: Address mode ${mode} not supported.\n"
                    fi
                done
                
                ADDRESS_MODES=(${modes_requested[@]})
                
                shift 2
                ;;
            -c|--configurations)
                local configs_requested=${2}
                
                local config
                for config in "${configs_requested[@]}";
                do
                    if ! has_element ${config} ${CONFIGURATIONS[@]}; then
                            printf "Error: Configuration ${config} not supported.\n"
                    fi
                done
                
                CONFIGURATIONS=(${configs_requested[@]})
                
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

#---------------------------------------------------------------------
# build()
#---------------------------------------------------------------------
function build 
{
    local library 
    for library in "${LIBRARIES[@]}";
    do    
        local dependency_name="DEPENDENCIES_${library}[@]"
        local librariesToBuild=

        if [ ! -z ${!dependency_name-} ]; then
            local dependencies=${!dependency_name} #Indirect expansion
            librariesToBuild=( ${dependencies[@]} ${library} )
        else
            librariesToBuild=( ${library} )
        fi

        for libraryToBuild in "${librariesToBuild[@]}";
        do

            local buildscript="${BUILD_SCRIPTS_DIR}/build_${libraryToBuild}.sh"

            # Test for build script
            if [ ! -f ${buildscript} ]; then
                printf "Buildscript ${buildscript} for library '${libraryToBuild}' does not exist. Skipping.\n"

                continue
            fi

            # Run build for each address mode and configuration
            local addressmode
            for addressmode in "${ADDRESS_MODES[@]}"
            do

                local configuration
                for configuration in "${CONFIGURATIONS[@]}";
                do
                    if [ ${#ALREADY_BUILT_LIBS[@]} -ne 0 ]; then

                        printf "Checking whether library ${libraryToBuild}_${addressmode}_${configuration} was compiled already.\n"

                        if has_element ${libraryToBuild}_${addressmode}_${configuration} ${ALREADY_BUILT_LIBS[@]}; then
                            printf "Library ${libraryToBuild}_${addressmode}_${configuration} already compiled. Skipping\n"
                            continue
                        fi
                    fi

                    printf "/*--------------------------------------------------------------------*/\n"
                    printf "/* Building ${libraryToBuild} as ${addressmode}/${configuration}      */\n"
                    printf "/*--------------------------------------------------------------------*/\n"
                    printf "                                                                        \n"

                    local source_directory=${LIBRARY_SOURCE_DIR}/${libraryToBuild}
                    local build_directory=${BUILD_BASE_DIR}/${libraryToBuild}
                    local deploy_directory=${DEPLOY_BASE_DIR}/${libraryToBuild}/linux${addressmode}/${configuration}

                    rm -rf ${build_directory}
                    mkdir -p ${build_directory}

                    rm -rf ${deploy_directory}
                    mkdir -p ${deploy_directory}

                    # Will inherit the current enclosing scopes and variables
                    source ${buildscript}

                    ALREADY_BUILT_LIBS=( ${ALREADY_BUILT_LIBS[@]} ${libraryToBuild}_${addressmode}_${configuration} )

                    printf "/*--------------------------------------------------------------------*/\n"
                    printf "/* Done building ${libraryToBuild} as ${addressmode}/${configuration} */\n"
                    printf "/*--------------------------------------------------------------------*/\n"
                    printf "                                                                        \n"

                done # foreach configuration

            done # foreach addressmode

        done # foreach libraryToBuild
        
    done # foreach library
}

#---------------------------------------------------------------------
# main()
#---------------------------------------------------------------------
# usage

test_env ${LIBRARY_SOURCE_DIR}

if [ -f define_libraries.sh ]; then
    source define_libraries.sh
else
    printf "Autodetecting available libraries in ${LIBRARY_SOURCE_DIR}\n"
    # LIBRARIES=( $( cd ${LIBRARY_SOURCE_DIR} && ls -d */ | sed 's/\/$//' ) )
fi

read_arguments $@

printf "Debug: ${DEBUG}\n"

if [ "${DEBUG}" = "1" ]; then
    printf "\nSelected libraries:"
    for library in " ${LIBRARIES[@]}"
    do 
    printf " ${library}"
    done

    printf "\nSelected address modes: "
    for addressmode in "${ADDRESS_MODES[@]}"
    do 
    printf " ${addressmode}"
    done

    printf "\nSelected configurations: "
    for configuration in "${CONFIGURATIONS[@]}"
    do 
    printf " ${configuration}"
    done
    
    printf "\n"
fi

build
