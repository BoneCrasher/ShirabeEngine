//
// Created by dotti on 08.12.19.
//

#ifndef __SHIRABEDEVELOPMENT_MESHPROCESSOR_H__
#define __SHIRABEDEVELOPMENT_MESHPROCESSOR_H__

#include <filesystem>
#include <core/result.h>

#include "common/config.h"
#include "meshes/definition.h"

namespace meshes
{
    using engine::CResult;
    using resource_compiler::EResult;

    CResult<EResult> processMesh(std::filesystem::path const &aMeshFile, SConfiguration const &aConfig);
}

#endif //__SHIRABEDEVELOPMENT_MATERIALPROCESSOR_H__
