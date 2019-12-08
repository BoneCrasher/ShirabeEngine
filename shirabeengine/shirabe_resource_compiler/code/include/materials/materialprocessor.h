//
// Created by dotti on 08.12.19.
//

#ifndef __SHIRABEDEVELOPMENT_MATERIALPROCESSOR_H__
#define __SHIRABEDEVELOPMENT_MATERIALPROCESSOR_H__

#include <filesystem>
#include <core/result.h>
#include "definition.h"
#include "shadercompilationunit.h"

namespace materials
{
    CResult<EResult> processMaterial(std::filesystem::path const &aMaterialFile);
}

#endif //__SHIRABEDEVELOPMENT_MATERIALPROCESSOR_H__
