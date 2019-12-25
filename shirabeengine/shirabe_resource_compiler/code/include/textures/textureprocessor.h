//
// Created by dotti on 08.12.19.
//

#ifndef __SHIRABEDEVELOPMENT_TEXTUREPROCESSOR_H__
#define __SHIRABEDEVELOPMENT_TEXTUREPROCESSOR_H__

#include <filesystem>
#include <core/result.h>

#include "common/config.h"
#include "textures/definition.h"

namespace texture
{
    using engine::CResult;
    using resource_compiler::EResult;

    CResult<EResult> processTexture(std::filesystem::path const &aTextureFile, SConfiguration const &aConfig);
}

#endif //__SHIRABEDEVELOPMENT_MATERIALPROCESSOR_H__
