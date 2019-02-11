//
// Copyright (C) 2016 Google, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//    Neither the name of Google Inc. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <cctype>

#include "resourcelimits.h"

namespace glslang_wrapper
{

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CBuiltInResource_t const &DefaultBuiltInResource()
    {
        static CBuiltInResource_t sResource =
        {
        /* .MaxLights                                 = */ 32,
        /* .MaxClipPlanes                             = */ 6,
        /* .MaxTextureUnits                           = */ 32,
        /* .MaxTextureCoords                          = */ 32,
        /* .MaxVertexAttribs                          = */ 64,
        /* .MaxVertexUniformComponents                = */ 4096,
        /* .MaxVaryingFloats                          = */ 64,
        /* .MaxVertexTextureImageUnits                = */ 32,
        /* .MaxCombinedTextureImageUnits              = */ 80,
        /* .MaxTextureImageUnits                      = */ 32,
        /* .MaxFragmentUniformComponents              = */ 4096,
        /* .MaxDrawBuffers                            = */ 32,
        /* .MaxVertexUniformVectors                   = */ 128,
        /* .MaxVaryingVectors                         = */ 8,
        /* .MaxFragmentUniformVectors                 = */ 16,
        /* .MaxVertexOutputVectors                    = */ 16,
        /* .MaxFragmentInputVectors                   = */ 15,
        /* .MinProgramTexelOffset                     = */ -8,
        /* .MaxProgramTexelOffset                     = */ 7,
        /* .MaxClipDistances                          = */ 8,
        /* .MaxComputeWorkGroupCountX                 = */ 65535,
        /* .MaxComputeWorkGroupCountY                 = */ 65535,
        /* .MaxComputeWorkGroupCountZ                 = */ 65535,
        /* .MaxComputeWorkGroupSizeX                  = */ 1024,
        /* .MaxComputeWorkGroupSizeY                  = */ 1024,
        /* .MaxComputeWorkGroupSizeZ                  = */ 64,
        /* .MaxComputeUniformComponents               = */ 1024,
        /* .MaxComputeTextureImageUnits               = */ 16,
        /* .MaxComputeImageUniforms                   = */ 8,
        /* .MaxComputeAtomicCounters                  = */ 8,
        /* .MaxComputeAtomicCounterBuffers            = */ 1,
        /* .MaxVaryingComponents                      = */ 60,
        /* .MaxVertexOutputComponents                 = */ 64,
        /* .MaxGeometryInputComponents                = */ 64,
        /* .MaxGeometryOutputComponents               = */ 128,
        /* .MaxFragmentInputComponents                = */ 128,
        /* .MaxImageUnits                             = */ 8,
        /* .MaxCombinedImageUnitsAndFragmentOutputs   = */ 8,
        /* .MaxCombinedShaderOutputResources          = */ 8,
        /* .MaxImageSamples                           = */ 0,
        /* .MaxVertexImageUniforms                    = */ 0,
        /* .MaxTessControlImageUniforms               = */ 0,
        /* .MaxTessEvaluationImageUniforms            = */ 0,
        /* .MaxGeometryImageUniforms                  = */ 0,
        /* .MaxFragmentImageUniforms                  = */ 8,
        /* .MaxCombinedImageUniforms                  = */ 8,
        /* .MaxGeometryTextureImageUnits              = */ 16,
        /* .MaxGeometryOutputVertices                 = */ 256,
        /* .MaxGeometryTotalOutputComponents          = */ 1024,
        /* .MaxGeometryUniformComponents              = */ 1024,
        /* .MaxGeometryVaryingComponents              = */ 64,
        /* .MaxTessControlInputComponents             = */ 128,
        /* .MaxTessControlOutputComponents            = */ 128,
        /* .MaxTessControlTextureImageUnits           = */ 16,
        /* .MaxTessControlUniformComponents           = */ 1024,
        /* .MaxTessControlTotalOutputComponents       = */ 4096,
        /* .MaxTessEvaluationInputComponents          = */ 128,
        /* .MaxTessEvaluationOutputComponents         = */ 128,
        /* .MaxTessEvaluationTextureImageUnits        = */ 16,
        /* .MaxTessEvaluationUniformComponents        = */ 1024,
        /* .MaxTessPatchComponents                    = */ 120,
        /* .MaxPatchVertices                          = */ 32,
        /* .MaxTessGenLevel                           = */ 64,
        /* .MaxViewports                              = */ 16,
        /* .MaxVertexAtomicCounters                   = */ 0,
        /* .MaxTessControlAtomicCounters              = */ 0,
        /* .MaxTessEvaluationAtomicCounters           = */ 0,
        /* .MaxGeometryAtomicCounters                 = */ 0,
        /* .MaxFragmentAtomicCounters                 = */ 8,
        /* .MaxCombinedAtomicCounters                 = */ 8,
        /* .MaxAtomicCounterBindings                  = */ 1,
        /* .MaxVertexAtomicCounterBuffers             = */ 0,
        /* .MaxTessControlAtomicCounterBuffers        = */ 0,
        /* .MaxTessEvaluationAtomicCounterBuffers     = */ 0,
        /* .MaxGeometryAtomicCounterBuffers           = */ 0,
        /* .MaxFragmentAtomicCounterBuffers           = */ 1,
        /* .MaxCombinedAtomicCounterBuffers           = */ 1,
        /* .MaxAtomicCounterBufferSize                = */ 16384,
        /* .MaxTransformFeedbackBuffers               = */ 4,
        /* .MaxTransformFeedbackInterleavedComponents = */ 64,
        /* .MaxCullDistances                          = */ 8,
        /* .MaxCombinedClipAndCullDistances           = */ 8,
        /* .MaxSamples                                = */ 4,
        /* .maxMeshOutputVerticesNV                   = */ 256,
        /* .maxMeshOutputPrimitivesNV                 = */ 512,
        /* .maxMeshWorkGroupSizeX_NV                  = */ 32,
        /* .maxMeshWorkGroupSizeY_NV                  = */ 1,
        /* .maxMeshWorkGroupSizeZ_NV                  = */ 1,
        /* .maxTaskWorkGroupSizeX_NV                  = */ 32,
        /* .maxTaskWorkGroupSizeY_NV                  = */ 1,
        /* .maxTaskWorkGroupSizeZ_NV                  = */ 1,
        /* .maxMeshViewCountNV                        = */ 4,

        /* .limits = */
        {
            /* .nonInductiveForLoops                  = */ 1,
            /* .whileLoops                            = */ 1,
            /* .doWhileLoops                          = */ 1,
            /* .generalUniformIndexing                = */ 1,
            /* .generalAttributeMatrixVectorIndexing  = */ 1,
            /* .generalVaryingIndexing                = */ 1,
            /* .generalSamplerIndexing                = */ 1,
            /* .generalVariableIndexing               = */ 1,
            /* .generalConstantMatrixVectorIndexing   = */ 1,
        }
        };

        return sResource;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    std::string GetDefaultBuiltInResourceString()
    {
        CBuiltInResource_t const &builtInResource = DefaultBuiltInResource();

        std::ostringstream ostream;

        ostream << "MaxLights "                                 << builtInResource.maxLights                                   << "\n"
                << "MaxClipPlanes "                             << builtInResource.maxClipPlanes                               << "\n"
                << "MaxTextureUnits "                           << builtInResource.maxTextureUnits                             << "\n"
                << "MaxTextureCoords "                          << builtInResource.maxTextureCoords                            << "\n"
                << "MaxVertexAttribs "                          << builtInResource.maxVertexAttribs                            << "\n"
                << "MaxVertexUniformComponents "                << builtInResource.maxVertexUniformComponents                  << "\n"
                << "MaxVaryingFloats "                          << builtInResource.maxVaryingFloats                            << "\n"
                << "MaxVertexTextureImageUnits "                << builtInResource.maxVertexTextureImageUnits                  << "\n"
                << "MaxCombinedTextureImageUnits "              << builtInResource.maxCombinedTextureImageUnits                << "\n"
                << "MaxTextureImageUnits "                      << builtInResource.maxTextureImageUnits                        << "\n"
                << "MaxFragmentUniformComponents "              << builtInResource.maxFragmentUniformComponents                << "\n"
                << "MaxDrawBuffers "                            << builtInResource.maxDrawBuffers                              << "\n"
                << "MaxVertexUniformVectors "                   << builtInResource.maxVertexUniformVectors                     << "\n"
                << "MaxVaryingVectors "                         << builtInResource.maxVaryingVectors                           << "\n"
                << "MaxFragmentUniformVectors "                 << builtInResource.maxFragmentUniformVectors                   << "\n"
                << "MaxVertexOutputVectors "                    << builtInResource.maxVertexOutputVectors                      << "\n"
                << "MaxFragmentInputVectors "                   << builtInResource.maxFragmentInputVectors                     << "\n"
                << "MinProgramTexelOffset "                     << builtInResource.minProgramTexelOffset                       << "\n"
                << "MaxProgramTexelOffset "                     << builtInResource.maxProgramTexelOffset                       << "\n"
                << "MaxClipDistances "                          << builtInResource.maxClipDistances                            << "\n"
                << "MaxComputeWorkGroupCountX "                 << builtInResource.maxComputeWorkGroupCountX                   << "\n"
                << "MaxComputeWorkGroupCountY "                 << builtInResource.maxComputeWorkGroupCountY                   << "\n"
                << "MaxComputeWorkGroupCountZ "                 << builtInResource.maxComputeWorkGroupCountZ                   << "\n"
                << "MaxComputeWorkGroupSizeX "                  << builtInResource.maxComputeWorkGroupSizeX                    << "\n"
                << "MaxComputeWorkGroupSizeY "                  << builtInResource.maxComputeWorkGroupSizeY                    << "\n"
                << "MaxComputeWorkGroupSizeZ "                  << builtInResource.maxComputeWorkGroupSizeZ                    << "\n"
                << "MaxComputeUniformComponents "               << builtInResource.maxComputeUniformComponents                 << "\n"
                << "MaxComputeTextureImageUnits "               << builtInResource.maxComputeTextureImageUnits                 << "\n"
                << "MaxComputeImageUniforms "                   << builtInResource.maxComputeImageUniforms                     << "\n"
                << "MaxComputeAtomicCounters "                  << builtInResource.maxComputeAtomicCounters                    << "\n"
                << "MaxComputeAtomicCounterBuffers "            << builtInResource.maxComputeAtomicCounterBuffers              << "\n"
                << "MaxVaryingComponents "                      << builtInResource.maxVaryingComponents                        << "\n"
                << "MaxVertexOutputComponents "                 << builtInResource.maxVertexOutputComponents                   << "\n"
                << "MaxGeometryInputComponents "                << builtInResource.maxGeometryInputComponents                  << "\n"
                << "MaxGeometryOutputComponents "               << builtInResource.maxGeometryOutputComponents                 << "\n"
                << "MaxFragmentInputComponents "                << builtInResource.maxFragmentInputComponents                  << "\n"
                << "MaxImageUnits "                             << builtInResource.maxImageUnits                               << "\n"
                << "MaxCombinedImageUnitsAndFragmentOutputs "   << builtInResource.maxCombinedImageUnitsAndFragmentOutputs     << "\n"
                << "MaxCombinedShaderOutputResources "          << builtInResource.maxCombinedShaderOutputResources            << "\n"
                << "MaxImageSamples "                           << builtInResource.maxImageSamples                             << "\n"
                << "MaxVertexImageUniforms "                    << builtInResource.maxVertexImageUniforms                      << "\n"
                << "MaxTessControlImageUniforms "               << builtInResource.maxTessControlImageUniforms                 << "\n"
                << "MaxTessEvaluationImageUniforms "            << builtInResource.maxTessEvaluationImageUniforms              << "\n"
                << "MaxGeometryImageUniforms "                  << builtInResource.maxGeometryImageUniforms                    << "\n"
                << "MaxFragmentImageUniforms "                  << builtInResource.maxFragmentImageUniforms                    << "\n"
                << "MaxCombinedImageUniforms "                  << builtInResource.maxCombinedImageUniforms                    << "\n"
                << "MaxGeometryTextureImageUnits "              << builtInResource.maxGeometryTextureImageUnits                << "\n"
                << "MaxGeometryOutputVertices "                 << builtInResource.maxGeometryOutputVertices                   << "\n"
                << "MaxGeometryTotalOutputComponents "          << builtInResource.maxGeometryTotalOutputComponents            << "\n"
                << "MaxGeometryUniformComponents "              << builtInResource.maxGeometryUniformComponents                << "\n"
                << "MaxGeometryVaryingComponents "              << builtInResource.maxGeometryVaryingComponents                << "\n"
                << "MaxTessControlInputComponents "             << builtInResource.maxTessControlInputComponents               << "\n"
                << "MaxTessControlOutputComponents "            << builtInResource.maxTessControlOutputComponents              << "\n"
                << "MaxTessControlTextureImageUnits "           << builtInResource.maxTessControlTextureImageUnits             << "\n"
                << "MaxTessControlUniformComponents "           << builtInResource.maxTessControlUniformComponents             << "\n"
                << "MaxTessControlTotalOutputComponents "       << builtInResource.maxTessControlTotalOutputComponents         << "\n"
                << "MaxTessEvaluationInputComponents "          << builtInResource.maxTessEvaluationInputComponents            << "\n"
                << "MaxTessEvaluationOutputComponents "         << builtInResource.maxTessEvaluationOutputComponents           << "\n"
                << "MaxTessEvaluationTextureImageUnits "        << builtInResource.maxTessEvaluationTextureImageUnits          << "\n"
                << "MaxTessEvaluationUniformComponents "        << builtInResource.maxTessEvaluationUniformComponents          << "\n"
                << "MaxTessPatchComponents "                    << builtInResource.maxTessPatchComponents                      << "\n"
                << "MaxPatchVertices "                          << builtInResource.maxPatchVertices                            << "\n"
                << "MaxTessGenLevel "                           << builtInResource.maxTessGenLevel                             << "\n"
                << "MaxViewports "                              << builtInResource.maxViewports                                << "\n"
                << "MaxVertexAtomicCounters "                   << builtInResource.maxVertexAtomicCounters                     << "\n"
                << "MaxTessControlAtomicCounters "              << builtInResource.maxTessControlAtomicCounters                << "\n"
                << "MaxTessEvaluationAtomicCounters "           << builtInResource.maxTessEvaluationAtomicCounters             << "\n"
                << "MaxGeometryAtomicCounters "                 << builtInResource.maxGeometryAtomicCounters                   << "\n"
                << "MaxFragmentAtomicCounters "                 << builtInResource.maxFragmentAtomicCounters                   << "\n"
                << "MaxCombinedAtomicCounters "                 << builtInResource.maxCombinedAtomicCounters                   << "\n"
                << "MaxAtomicCounterBindings "                  << builtInResource.maxAtomicCounterBindings                    << "\n"
                << "MaxVertexAtomicCounterBuffers "             << builtInResource.maxVertexAtomicCounterBuffers               << "\n"
                << "MaxTessControlAtomicCounterBuffers "        << builtInResource.maxTessControlAtomicCounterBuffers          << "\n"
                << "MaxTessEvaluationAtomicCounterBuffers "     << builtInResource.maxTessEvaluationAtomicCounterBuffers       << "\n"
                << "MaxGeometryAtomicCounterBuffers "           << builtInResource.maxGeometryAtomicCounterBuffers             << "\n"
                << "MaxFragmentAtomicCounterBuffers "           << builtInResource.maxFragmentAtomicCounterBuffers             << "\n"
                << "MaxCombinedAtomicCounterBuffers "           << builtInResource.maxCombinedAtomicCounterBuffers             << "\n"
                << "MaxAtomicCounterBufferSize "                << builtInResource.maxAtomicCounterBufferSize                  << "\n"
                << "MaxTransformFeedbackBuffers "               << builtInResource.maxTransformFeedbackBuffers                 << "\n"
                << "MaxTransformFeedbackInterleavedComponents " << builtInResource.maxTransformFeedbackInterleavedComponents   << "\n"
                << "MaxCullDistances "                          << builtInResource.maxCullDistances                            << "\n"
                << "MaxCombinedClipAndCullDistances "           << builtInResource.maxCombinedClipAndCullDistances             << "\n"
                << "MaxSamples "                                << builtInResource.maxSamples                                  << "\n"
    #ifdef NV_EXTENSIONS
                << "MaxMeshOutputVerticesNV "                   << builtInResource.maxMeshOutputVerticesNV                     << "\n"
                << "MaxMeshOutputPrimitivesNV "                 << builtInResource.maxMeshOutputPrimitivesNV                   << "\n"
                << "MaxMeshWorkGroupSizeX_NV "                  << builtInResource.maxMeshWorkGroupSizeX_NV                    << "\n"
                << "MaxMeshWorkGroupSizeY_NV "                  << builtInResource.maxMeshWorkGroupSizeY_NV                    << "\n"
                << "MaxMeshWorkGroupSizeZ_NV "                  << builtInResource.maxMeshWorkGroupSizeZ_NV                    << "\n"
                << "MaxTaskWorkGroupSizeX_NV "                  << builtInResource.maxTaskWorkGroupSizeX_NV                    << "\n"
                << "MaxTaskWorkGroupSizeY_NV "                  << builtInResource.maxTaskWorkGroupSizeY_NV                    << "\n"
                << "MaxTaskWorkGroupSizeZ_NV "                  << builtInResource.maxTaskWorkGroupSizeZ_NV                    << "\n"
                << "MaxMeshViewCountNV "                        << builtInResource.maxMeshViewCountNV                          << "\n"
    #endif
                << "nonInductiveForLoops "                      << builtInResource.limits.nonInductiveForLoops                 << "\n"
                << "whileLoops "                                << builtInResource.limits.whileLoops                           << "\n"
                << "doWhileLoops "                              << builtInResource.limits.doWhileLoops                         << "\n"
                << "generalUniformIndexing "                    << builtInResource.limits.generalUniformIndexing               << "\n"
                << "generalAttributeMatrixVectorIndexing "      << builtInResource.limits.generalAttributeMatrixVectorIndexing << "\n"
                << "generalVaryingIndexing "                    << builtInResource.limits.generalVaryingIndexing               << "\n"
                << "generalSamplerIndexing "                    << builtInResource.limits.generalSamplerIndexing               << "\n"
                << "generalVariableIndexing "                   << builtInResource.limits.generalVariableIndexing              << "\n"
                << "generalConstantMatrixVectorIndexing "       << builtInResource.limits.generalConstantMatrixVectorIndexing  << "\n"
          ;

        return ostream.str();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------f
    void DecodeResourceLimits(CBuiltInResource_t *aResources, char *aConfig)
    {
        static char const *delimiters = " \t\n\r";

        size_t pos = 0;

        std::string configStr(config);

        while ((pos = configStr.find_first_not_of(delims, pos)) != std::string::npos) {
            const size_t token_s = pos;
            const size_t token_e = configStr.find_first_of(delims, token_s);
            const size_t value_s = configStr.find_first_not_of(delims, token_e);
            const size_t value_e = configStr.find_first_of(delims, value_s);
            pos = value_e;

            // Faster to use compare(), but prefering readability.
            const std::string tokenStr = configStr.substr(token_s, token_e-token_s);
            const std::string valueStr = configStr.substr(value_s, value_e-value_s);

            if (value_s == std::string::npos || ! (valueStr[0] == '-' || isdigit(valueStr[0]))) {
                printf("Error: '%s' bad .conf file.  Each name must be followed by one number.\n",
                       valueStr.c_str());
                return;
            }

            const int value = std::atoi(valueStr.c_str());

            if (tokenStr == "MaxLights")
                resources->maxLights = value;
            else if (tokenStr == "MaxClipPlanes")
                resources->maxClipPlanes = value;
            else if (tokenStr == "MaxTextureUnits")
                resources->maxTextureUnits = value;
            else if (tokenStr == "MaxTextureCoords")
                resources->maxTextureCoords = value;
            else if (tokenStr == "MaxVertexAttribs")
                resources->maxVertexAttribs = value;
            else if (tokenStr == "MaxVertexUniformComponents")
                resources->maxVertexUniformComponents = value;
            else if (tokenStr == "MaxVaryingFloats")
                resources->maxVaryingFloats = value;
            else if (tokenStr == "MaxVertexTextureImageUnits")
                resources->maxVertexTextureImageUnits = value;
            else if (tokenStr == "MaxCombinedTextureImageUnits")
                resources->maxCombinedTextureImageUnits = value;
            else if (tokenStr == "MaxTextureImageUnits")
                resources->maxTextureImageUnits = value;
            else if (tokenStr == "MaxFragmentUniformComponents")
                resources->maxFragmentUniformComponents = value;
            else if (tokenStr == "MaxDrawBuffers")
                resources->maxDrawBuffers = value;
            else if (tokenStr == "MaxVertexUniformVectors")
                resources->maxVertexUniformVectors = value;
            else if (tokenStr == "MaxVaryingVectors")
                resources->maxVaryingVectors = value;
            else if (tokenStr == "MaxFragmentUniformVectors")
                resources->maxFragmentUniformVectors = value;
            else if (tokenStr == "MaxVertexOutputVectors")
                resources->maxVertexOutputVectors = value;
            else if (tokenStr == "MaxFragmentInputVectors")
                resources->maxFragmentInputVectors = value;
            else if (tokenStr == "MinProgramTexelOffset")
                resources->minProgramTexelOffset = value;
            else if (tokenStr == "MaxProgramTexelOffset")
                resources->maxProgramTexelOffset = value;
            else if (tokenStr == "MaxClipDistances")
                resources->maxClipDistances = value;
            else if (tokenStr == "MaxComputeWorkGroupCountX")
                resources->maxComputeWorkGroupCountX = value;
            else if (tokenStr == "MaxComputeWorkGroupCountY")
                resources->maxComputeWorkGroupCountY = value;
            else if (tokenStr == "MaxComputeWorkGroupCountZ")
                resources->maxComputeWorkGroupCountZ = value;
            else if (tokenStr == "MaxComputeWorkGroupSizeX")
                resources->maxComputeWorkGroupSizeX = value;
            else if (tokenStr == "MaxComputeWorkGroupSizeY")
                resources->maxComputeWorkGroupSizeY = value;
            else if (tokenStr == "MaxComputeWorkGroupSizeZ")
                resources->maxComputeWorkGroupSizeZ = value;
            else if (tokenStr == "MaxComputeUniformComponents")
                resources->maxComputeUniformComponents = value;
            else if (tokenStr == "MaxComputeTextureImageUnits")
                resources->maxComputeTextureImageUnits = value;
            else if (tokenStr == "MaxComputeImageUniforms")
                resources->maxComputeImageUniforms = value;
            else if (tokenStr == "MaxComputeAtomicCounters")
                resources->maxComputeAtomicCounters = value;
            else if (tokenStr == "MaxComputeAtomicCounterBuffers")
                resources->maxComputeAtomicCounterBuffers = value;
            else if (tokenStr == "MaxVaryingComponents")
                resources->maxVaryingComponents = value;
            else if (tokenStr == "MaxVertexOutputComponents")
                resources->maxVertexOutputComponents = value;
            else if (tokenStr == "MaxGeometryInputComponents")
                resources->maxGeometryInputComponents = value;
            else if (tokenStr == "MaxGeometryOutputComponents")
                resources->maxGeometryOutputComponents = value;
            else if (tokenStr == "MaxFragmentInputComponents")
                resources->maxFragmentInputComponents = value;
            else if (tokenStr == "MaxImageUnits")
                resources->maxImageUnits = value;
            else if (tokenStr == "MaxCombinedImageUnitsAndFragmentOutputs")
                resources->maxCombinedImageUnitsAndFragmentOutputs = value;
            else if (tokenStr == "MaxCombinedShaderOutputResources")
                resources->maxCombinedShaderOutputResources = value;
            else if (tokenStr == "MaxImageSamples")
                resources->maxImageSamples = value;
            else if (tokenStr == "MaxVertexImageUniforms")
                resources->maxVertexImageUniforms = value;
            else if (tokenStr == "MaxTessControlImageUniforms")
                resources->maxTessControlImageUniforms = value;
            else if (tokenStr == "MaxTessEvaluationImageUniforms")
                resources->maxTessEvaluationImageUniforms = value;
            else if (tokenStr == "MaxGeometryImageUniforms")
                resources->maxGeometryImageUniforms = value;
            else if (tokenStr == "MaxFragmentImageUniforms")
                resources->maxFragmentImageUniforms = value;
            else if (tokenStr == "MaxCombinedImageUniforms")
                resources->maxCombinedImageUniforms = value;
            else if (tokenStr == "MaxGeometryTextureImageUnits")
                resources->maxGeometryTextureImageUnits = value;
            else if (tokenStr == "MaxGeometryOutputVertices")
                resources->maxGeometryOutputVertices = value;
            else if (tokenStr == "MaxGeometryTotalOutputComponents")
                resources->maxGeometryTotalOutputComponents = value;
            else if (tokenStr == "MaxGeometryUniformComponents")
                resources->maxGeometryUniformComponents = value;
            else if (tokenStr == "MaxGeometryVaryingComponents")
                resources->maxGeometryVaryingComponents = value;
            else if (tokenStr == "MaxTessControlInputComponents")
                resources->maxTessControlInputComponents = value;
            else if (tokenStr == "MaxTessControlOutputComponents")
                resources->maxTessControlOutputComponents = value;
            else if (tokenStr == "MaxTessControlTextureImageUnits")
                resources->maxTessControlTextureImageUnits = value;
            else if (tokenStr == "MaxTessControlUniformComponents")
                resources->maxTessControlUniformComponents = value;
            else if (tokenStr == "MaxTessControlTotalOutputComponents")
                resources->maxTessControlTotalOutputComponents = value;
            else if (tokenStr == "MaxTessEvaluationInputComponents")
                resources->maxTessEvaluationInputComponents = value;
            else if (tokenStr == "MaxTessEvaluationOutputComponents")
                resources->maxTessEvaluationOutputComponents = value;
            else if (tokenStr == "MaxTessEvaluationTextureImageUnits")
                resources->maxTessEvaluationTextureImageUnits = value;
            else if (tokenStr == "MaxTessEvaluationUniformComponents")
                resources->maxTessEvaluationUniformComponents = value;
            else if (tokenStr == "MaxTessPatchComponents")
                resources->maxTessPatchComponents = value;
            else if (tokenStr == "MaxPatchVertices")
                resources->maxPatchVertices = value;
            else if (tokenStr == "MaxTessGenLevel")
                resources->maxTessGenLevel = value;
            else if (tokenStr == "MaxViewports")
                resources->maxViewports = value;
            else if (tokenStr == "MaxVertexAtomicCounters")
                resources->maxVertexAtomicCounters = value;
            else if (tokenStr == "MaxTessControlAtomicCounters")
                resources->maxTessControlAtomicCounters = value;
            else if (tokenStr == "MaxTessEvaluationAtomicCounters")
                resources->maxTessEvaluationAtomicCounters = value;
            else if (tokenStr == "MaxGeometryAtomicCounters")
                resources->maxGeometryAtomicCounters = value;
            else if (tokenStr == "MaxFragmentAtomicCounters")
                resources->maxFragmentAtomicCounters = value;
            else if (tokenStr == "MaxCombinedAtomicCounters")
                resources->maxCombinedAtomicCounters = value;
            else if (tokenStr == "MaxAtomicCounterBindings")
                resources->maxAtomicCounterBindings = value;
            else if (tokenStr == "MaxVertexAtomicCounterBuffers")
                resources->maxVertexAtomicCounterBuffers = value;
            else if (tokenStr == "MaxTessControlAtomicCounterBuffers")
                resources->maxTessControlAtomicCounterBuffers = value;
            else if (tokenStr == "MaxTessEvaluationAtomicCounterBuffers")
                resources->maxTessEvaluationAtomicCounterBuffers = value;
            else if (tokenStr == "MaxGeometryAtomicCounterBuffers")
                resources->maxGeometryAtomicCounterBuffers = value;
            else if (tokenStr == "MaxFragmentAtomicCounterBuffers")
                resources->maxFragmentAtomicCounterBuffers = value;
            else if (tokenStr == "MaxCombinedAtomicCounterBuffers")
                resources->maxCombinedAtomicCounterBuffers = value;
            else if (tokenStr == "MaxAtomicCounterBufferSize")
                resources->maxAtomicCounterBufferSize = value;
            else if (tokenStr == "MaxTransformFeedbackBuffers")
                resources->maxTransformFeedbackBuffers = value;
            else if (tokenStr == "MaxTransformFeedbackInterleavedComponents")
                resources->maxTransformFeedbackInterleavedComponents = value;
            else if (tokenStr == "MaxCullDistances")
                resources->maxCullDistances = value;
            else if (tokenStr == "MaxCombinedClipAndCullDistances")
                resources->maxCombinedClipAndCullDistances = value;
            else if (tokenStr == "MaxSamples")
                resources->maxSamples = value;
    #ifdef NV_EXTENSIONS
            else if (tokenStr == "MaxMeshOutputVerticesNV")
                resources->maxMeshOutputVerticesNV = value;
            else if (tokenStr == "MaxMeshOutputPrimitivesNV")
                resources->maxMeshOutputPrimitivesNV = value;
            else if (tokenStr == "MaxMeshWorkGroupSizeX_NV")
                resources->maxMeshWorkGroupSizeX_NV = value;
            else if (tokenStr == "MaxMeshWorkGroupSizeY_NV")
                resources->maxMeshWorkGroupSizeY_NV = value;
            else if (tokenStr == "MaxMeshWorkGroupSizeZ_NV")
                resources->maxMeshWorkGroupSizeZ_NV = value;
            else if (tokenStr == "MaxTaskWorkGroupSizeX_NV")
                resources->maxTaskWorkGroupSizeX_NV = value;
            else if (tokenStr == "MaxTaskWorkGroupSizeY_NV")
                resources->maxTaskWorkGroupSizeY_NV = value;
            else if (tokenStr == "MaxTaskWorkGroupSizeZ_NV")
                resources->maxTaskWorkGroupSizeZ_NV = value;
            else if (tokenStr == "MaxMeshViewCountNV")
                resources->maxMeshViewCountNV = value;
    #endif
            else if (tokenStr == "nonInductiveForLoops")
                resources->limits.nonInductiveForLoops = (value != 0);
            else if (tokenStr == "whileLoops")
                resources->limits.whileLoops = (value != 0);
            else if (tokenStr == "doWhileLoops")
                resources->limits.doWhileLoops = (value != 0);
            else if (tokenStr == "generalUniformIndexing")
                resources->limits.generalUniformIndexing = (value != 0);
            else if (tokenStr == "generalAttributeMatrixVectorIndexing")
                resources->limits.generalAttributeMatrixVectorIndexing = (value != 0);
            else if (tokenStr == "generalVaryingIndexing")
                resources->limits.generalVaryingIndexing = (value != 0);
            else if (tokenStr == "generalSamplerIndexing")
                resources->limits.generalSamplerIndexing = (value != 0);
            else if (tokenStr == "generalVariableIndexing")
                resources->limits.generalVariableIndexing = (value != 0);
            else if (tokenStr == "generalConstantMatrixVectorIndexing")
                resources->limits.generalConstantMatrixVectorIndexing = (value != 0);
            else
                printf("Warning: unrecognized limit (%s) in configuration file.\n", tokenStr.c_str());

        }
    }

}  // end namespace glslang
