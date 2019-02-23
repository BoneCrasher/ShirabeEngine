#ifndef   _SHIRABE_SHADERPRECOMP_EXTRACTION_H_
#define   _SHIRABE_SHADERPRECOMP_EXTRACTION_H_

#include <vector>
#include <string>
#include <stdint.h>

#include "shadercompilationunit.h"

namespace shader_precompiler
{
    /**
     * Read a file into a string.
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
    EResult spirvCrossExtract(SShaderCompilationUnit const &aUnit);
}

#endif // _SHIRABE_SHADERPRECOMP_EXTRACTION_H_
