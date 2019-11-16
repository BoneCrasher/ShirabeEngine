#!/bin/bash

# Fetch input parameters, if any
while getopts p:s option
do
    case "${option}"
    in
        p) PROJECTS=( ${OPTARG} );;
        s) SINGLE_DOC=1;;
    esac
done

echo "Single doc: ${SINGLE_DOC}"
echo "Project:    ${PROJECTS}"

if [ -z "${SINGLE_DOC}" ]; then
    SINGLE_DOC=0
fi

if [ -z "${PROJECTS}" ]; then    
    PROJECTS=(                                   \
        shirabeengine/modules/asset              \
        shirabeengine/modules/base               \
        shirabeengine/modules/core               \
        shirabeengine/modules/graphicsapi        \
        shirabeengine/modules/log                \
        shirabeengine/modules/material           \
        shirabeengine/modules/math               \
        shirabeengine/modules/os                 \
        shirabeengine/modules/platform           \
        shirabeengine/modules/rendering          \
        shirabeengine/modules/resources          \
        shirabeengine/modules/utility            \
        shirabeengine/modules/vulkan_integration \
        shirabeengine/modules/wsi                \
        shirabeengine
    )
fi

PWDDIR=$(pwd -P)
DOCBASEDIR=${PWDDIR}/../_documentation
SRCBASEDIR=${PWDDIR}/..

function verify_paths() 
{
    if [ -d $1 ]; then
        rm -r $1
    fi
    
    mkdir -p $1
}

# Parameter1: Project-Id; Implicitly used as source dir if parameter 2 is unspecified
# Parameter2: Source-Dir
function generate_one()
{
    lproject=$1
    lproject_id=${lproject//'/'/'_'}
    lproject_doc_dir=${DOCBASEDIR}/${lproject}
    lproject_src_dir=${SRCBASEDIR}/${lproject}
    if ! [ -z "$2" ]; then 
        lproject_src_dir=$2
    fi
    
    verify_paths ${lproject_doc_dir} # DON'T DO THIS FOR THE SOURCE PATH OR YOU DELETE YOUR SOURCE!
    
    echo "Generating doxygen for project ${lproject} in ${lproject_doc_dir} from ${lproject_src_dir}."
    
    # Take the base doxyconf and append the directory settings.
    lproject_doxyconf_tmp_fn=shirabeengine.${lproject_id}.doxyconf
    lproject_doxyconf_mcss_tmp_fn=shirabeengine.${lproject_id}.mcss.doxyconf
    
    lproject_excl_dirs="${SRCBASEDIR}/_build ${SRCBASEDIR}/_deploy"
    
    lproject_config_data=`cat 'shirabeengine.doxyconf.template'` # Read file to variable
    lproject_config_data=${lproject_config_data/'${SHIRABEENGINE_PLATFORM_CONFIG}'/'linux64/debug'}
    lproject_config_data=${lproject_config_data/'${SHIRABEENGINE_SOURCE_DIR}'/${lproject_src_dir}}
    lproject_config_data=${lproject_config_data/'${SHIRABEENGINE_OUTPUT_DIR}'/${lproject_doc_dir}}
    lproject_config_data=${lproject_config_data/'${SHIRABEENGINE_EXCLUDED_DIRS}'/${lproject_excl_dirs}}
    
    echo "${lproject_config_data}" >> "${lproject_doxyconf_tmp_fn}" # Write M.CSS results
    
    # Setup M.CSS config file
    lproject_config_data_mcss=`cat 'shirabeengine.doxyfile-mcss.template'`
    lproject_config_data_mcss=${lproject_config_data_mcss/'${SHIRABEENGINE_DOXYFILE_NAME}'/${lproject_doxyconf_tmp_fn}}
    
    echo "${lproject_config_data_mcss}" >> "${lproject_doxyconf_mcss_tmp_fn}" # Write M.CSS results
    
    
    # DOXY IT!
    python3.6 ./m.css/documentation/doxygen.py ${PWDDIR}/${lproject_doxyconf_mcss_tmp_fn}
    
    # Cleanup
    rm ${lproject_doxyconf_tmp_fn}
    rm ${lproject_doxyconf_mcss_tmp_fn}
}


if [ ${SINGLE_DOC} -eq 1 ]; then
    generate_one shirabeengine ${SRCBASEDIR}
    echo "Built single documentation tree."
else
    for lproject in ${PROJECTS[*]}
    do
        generate_one ${lproject}
    done
    echo "Built documentation tree for each subproject individually."
fi

