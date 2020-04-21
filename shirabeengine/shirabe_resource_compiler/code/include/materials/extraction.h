#ifndef   _SHIRABE_RESOURCECOMP_EXTRACTION_H_
#define   _SHIRABE_RESOURCECOMP_EXTRACTION_H_

#include <vector>
#include <string>
#include <cstdint>
#include <core/result.h>

#include "shadercompilationunit.h"

namespace engine::material
{
    struct SMaterialAsset;
}

namespace materials
{
    using engine::CResult;
    using engine::material::SMaterialAsset;
    
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
    CResult<bool> spirvCrossExtract(SShaderCompilationUnit const &aUnit, SMaterialAsset &aInOutAsset);
}

#endif // _SHIRABE_SHADERPRECOMP_EXTRACTION_H_
