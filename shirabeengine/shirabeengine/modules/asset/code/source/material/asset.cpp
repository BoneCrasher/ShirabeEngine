﻿#include "asset/material/asset.h"
#include <util/documents/json.h>

namespace engine::material
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    std::unordered_map<VkPipelineStageFlagBits, SMaterialStage> const SMaterialAsset::sEmptyMetaMap =
    {
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                  , {} },
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT    , {} },
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT , {} },
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                , {} },
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                , {} },
            { VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                 , {} }
    };
    //<-----------------------------------------------------------------------------
}
