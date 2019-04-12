#!/bin/bash

 WORKINGDIR=/home/dotti/workspaces/shirabeengine/shirabeengine/_deploy/linux64/debug/bin
   TOOLSDIR=${WORKINGDIR}/tools
    GLSLANG=${TOOLSDIR}/glslang/bin/glslangValidator
  SPIRV_OPT=${TOOLSDIR}/spirv-tools/bin/spirv-opt
  SPIRV_DIS=${TOOLSDIR}/spirv-tools/bin/spirv-dis
  SPIRV_LNK=${TOOLSDIR}/spirv-tools/bin/spirv-link  
  SPIRV_VAL=${TOOLSDIR}/spirv-tools/bin/spirv-val
SHADER_ROOT=${WORKINGDIR}/data/input/resources/shaders
RESULT_ROOT=${WORKINGDIR}/data/output/khronosmeetup

#
# ./glslangValidator [options] [-o <outputFn>] <inputFilename> [<inputFilename>...]
#
# The glslangValidator tool will compile GLSL to SPIR-V using a target-environment flag
# set to the Vk environment to be supported. 
# This will derive the SPIR-V version as well.
# Since GLSL natively only supports a single "main" entry point per file, 
# we rename it for later use of the SPIR-V module in the spirv-link-call.
#
# Options used for demo:
#   -v                  Verbose Output
#
#   -Od                 Disables optimization; may cause illegal SPIR-V for HLSL 
#
#   --target-env        Set execution environment that emitted code will execute in (versus source language.
#                         vulkan1.1 -> SPIR-V 1.3
#                         (!) Implies -V
# 
#   -V (implicit)       Create SPIR-V binary, under Vulkan semantics;
#                         (!) Implies -l                
#
#   -l (implicit)       Link all input files together to form a single module
#
#   --entry-point       Specify <name> as the entry-point function name
# 
#   --source-entrypoint The given shader source function is renamed to be the <name> given in -e
#
function run_glslangValidator
{
    COMMAND="${GLSLANG} -v                                        \
                        -Od                                       \
                        --target-env vulkan1.1                    \
                        -I${SHADER_ROOT}/include/                 \
                        --entry-point "main_$3"                   \
                        --source-entrypoint main                  \
                        -o $2                                     \
                        $1"
                        
    ${COMMAND}
}

#
# ./spirv-val [options] [<filename>]
#
# Validate a single SPIR-V binary against the desired vulkan environment.
#
# Options used for demo:
#   --target-env Specifies the target environment to validate against.
#                  vulkan1.1 -> SPIR-V 1.3
#
function run_spirv-validate
{
    COMMAND="${SPIRV_VAL} --target-env vulkan1.1 $1"
    
    ${COMMAND} 2&>1 tee $2 # Write it all to file AND print it in console, if there's anything...
}

# 
# ./spirv-dis [options] [<filename>]
#
# Disassemble a SPIR-V module to human readable SPIR-V.
#
# Options used for demo:
#   -o Specifies the output filename to write the disassembled SPIR-V module to. 
#        Writes to stdout, if omitted.
#
function run_spirv-dis 
{
    COMMAND="${SPIRV_DIS} -o $1.dis $1"   
                                    
    ${COMMAND}
}

# 
# ./spirv-opt [options] [<input>] -o <output>
#
# Optimize a SPIR-V binary w/ regard to specific requirements.
#
# Options used for demo:
#   -O            Optimize for performance (-Os would optimize for size).
#
#   -o            Specifies the output filename of the optimized module.
#
#   --strip-debug Remove all debug symbols
#
function run_spirv-opt
{    
    COMMAND="${SPIRV_OPT} -O $1 -o $2"
                          
    ${COMMAND}
}

#
# 
# ./spirv-link [options] <filename> [<filename> ...]
#
# Link together several SPIR-V binaries into a single module.
#
# Options used for demo:
#   --target-env Specifies the target environment to validate against.
#                  vulkan1.1 -> SPIR-V1.3
#
#   -o           Specifies the output file name of the linked-together 
#                  SPIRV-binary.
#
function run_spirv-link
{
    COMMAND="${SPIRV_LNK} --target-env vulkan1.1                         \
                          -o $1 \
                          $2"
    
    ${COMMAND}
}

# Files to process
FILENAMES=( standard/standard.vert standard/standard.frag )

# Filelists used as input to spirv-link
    FILENAME_LIST=""
OPT_FILENAME_LIST=""

#
# For each shader file:
#
#   1. Compile glsl -> spirv1.3 using the glsl reference compiler glslangValidator
#   2. Validate the module using spirv-val.
#   3. Disassemble the module for debug using spirv-dis
#   4. Optimize the module using spirv-opt
#   5. Disassemble the optimized module using spirv-dis
#
# Finally:
#   1. Link together the unoptimized modules using spirv-link
#   2. Link together the   optimized modules using spirv-link
#

for lShaderfile in ${FILENAMES[@]}
do
    # Make sure that our output dir exists.
    lTargetDirectory=$(dirname "${RESULT_ROOT}/${lShaderfile}")
    mkdir -p ${lTargetDirectory}
    
     lFilename=$(basename -- "${lShaderfile}")
    lExtension="${lFilename##*.}"             # vert/frag
    # lFilename="${lFilename%.*}"    
    
              lInputFilename=${SHADER_ROOT}/${lShaderfile}.glsl
             lModuleFileBase=${RESULT_ROOT}/${lShaderfile}.glsl 
        lUnoptimizedModuleFn=${lModuleFileBase}.spv
    lUnoptimizedValidationFn=${lModuleFileBase}.validation.md
          lOptimizedModuleFn=${lModuleFileBase}.OPT.spv
      lOptimizedValidationFn=${lModuleFileBase}.OPT.validation.md
        
    # Compile
    run_glslangValidator ${lInputFilename}       \
                         ${lUnoptimizedModuleFn} \
                         ${lExtension}             # For renaming main to main_${lExtension} 
    run_spirv-validate   ${lUnoptimizedModuleFn} \
                         ${lUnoptimizedValidationFn}
    run_spirv-dis        ${lUnoptimizedModuleFn}   # Will emit ${lUnoptimizedModuleFn}.dis
    
    #
    # Optimize the module.
    #
    run_spirv-opt        ${lUnoptimizedModuleFn} \
                         ${lOptimizedModuleFn}
    run_spirv-validate   ${lOptimizedModuleFn} \
                         ${lOptimizedValidationFn}
    run_spirv-dis        ${lOptimizedModuleFn}     # Will emit ${lOptimizedModuleFn}.dis 
    
        FILENAME_LIST="${FILENAME_LIST}     ${RESULT_ROOT}/${lShaderfile}.glsl.spv"
    OPT_FILENAME_LIST="${OPT_FILENAME_LIST} ${RESULT_ROOT}/${lShaderfile}.glsl.OPT.spv"    
    
done

#
# Link both modules together into one!
#

run_spirv-link       ${RESULT_ROOT}/standard/standard.linked.spv ${FILENAME_LIST}
run_spirv-dis        ${RESULT_ROOT}/standard/standard.linked.spv
run_spirv-validate   ${RESULT_ROOT}/standard/standard.linked.spv \
                     ${RESULT_ROOT}/standard/standard.linked.validation.md
run_spirv-link       ${RESULT_ROOT}/standard/standard.OPT.linked.spv ${OPT_FILENAME_LIST}
run_spirv-dis        ${RESULT_ROOT}/standard/standard.OPT.linked.spv
run_spirv-validate   ${RESULT_ROOT}/standard/standard.OPT.linked.spv \
                     ${RESULT_ROOT}/standard/standard.OPT.linked.validation.md
