//
// Created by dotti on 08.12.19.
//

#ifndef __SHIRABEDEVELOPMENT_MATERIALPROCESSOR_H__
#define __SHIRABEDEVELOPMENT_MATERIALPROCESSOR_H__

#include <filesystem>
#include <core/result.h>

#include "common/config.h"
#include "materials/definition.h"
#include "materials/shadercompilationunit.h"

namespace materials
{
    CResult<EResult> processMaterial(std::filesystem::path const &aMaterialFile, SConfiguration const &aConfig);
}

#endif //__SHIRABEDEVELOPMENT_MATERIALPROCESSOR_H__
