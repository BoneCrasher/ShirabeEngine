#ifndef   _SHIRABE_SHADERPRECOMP_EXTRACTION_H_
#define   _SHIRABE_SHADERPRECOMP_EXTRACTION_H_

#include <vector>
#include <string>
#include <stdint.h>

#include "shadercompilationunit.h"

namespace shader_precompiler
{
    /**
     * Read a uint32_t word SPIR-V file into a vector<uint32_t>.
     *
     * @param aFileName Filename of the file to read.
     * @return          See brief.
     */
    std::vector<uint32_t> const readSpirVFile(std::string const &aFileName);

    /**
     * @brief spirvCrossExtract
     * @param aUnit
     * @return
     */
    CResult<SMaterial> spirvCrossExtract(SShaderCompilationUnit const &aUnit);
}

#endif // _SHIRABE_SHADERPRECOMP_EXTRACTION_H_
