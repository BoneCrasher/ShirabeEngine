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

        std::string const configStr(aConfig);

        while (std::string::npos != (pos = configStr.find_first_not_of(delimiters, pos)))
        {
            size_t const token_s = pos;
            size_t const token_e = configStr.find_first_of    (delimiters, token_s);
            size_t const value_s = configStr.find_first_not_of(delimiters, token_e);
            size_t const value_e = configStr.find_first_of    (delimiters, value_s);

            pos = value_e;

            // Faster to use compare(), but prefering readability.
            std::string const tokenStr = configStr.substr(token_s, token_e-token_s);
            std::string const valueStr = configStr.substr(value_s, value_e-value_s);

            if ( std::string::npos == value_s || not ('-' == valueStr[0] || std::isdigit(valueStr[0])) )
            {
                printf("Error: '%s' bad .conf file.  Each name must be followed by one number.\n", valueStr.c_str());
                return;
            }

            int32_t const value = std::atoi(valueStr.c_str());

            if ("MaxLights" == tokenStr)
                aResources->maxLights = value;
            else if ("MaxClipPlanes" == tokenStr)
                aResources->maxClipPlanes = value;
            else if ("MaxTextureUnits" == tokenStr)
                aResources->maxTextureUnits = value;
            else if ("MaxTextureCoords" == tokenStr)
                aResources->maxTextureCoords = value;
            else if ("MaxVertexAttribs" == tokenStr)
                aResources->maxVertexAttribs = value;
            else if ("MaxVertexUniformComponents" == tokenStr)
                aResources->maxVertexUniformComponents = value;
            else if ("MaxVaryingFloats" == tokenStr)
                aResources->maxVaryingFloats = value;
            else if ("MaxVertexTextureImageUnits" == tokenStr)
                aResources->maxVertexTextureImageUnits = value;
            else if ("MaxCombinedTextureImageUnits" == tokenStr)
                aResources->maxCombinedTextureImageUnits = value;
            else if ("MaxTextureImageUnits" == tokenStr)
                aResources->maxTextureImageUnits = value;
            else if ("MaxFragmentUniformComponents" == tokenStr)
                aResources->maxFragmentUniformComponents = value;
            else if ("MaxDrawBuffers" == tokenStr)
                aResources->maxDrawBuffers = value;
            else if ("MaxVertexUniformVectors" == tokenStr)
                aResources->maxVertexUniformVectors = value;
            else if ("MaxVaryingVectors" == tokenStr)
                aResources->maxVaryingVectors = value;
            else if ("MaxFragmentUniformVectors" == tokenStr)
                aResources->maxFragmentUniformVectors = value;
            else if ("MaxVertexOutputVectors" == tokenStr)
                aResources->maxVertexOutputVectors = value;
            else if ("MaxFragmentInputVectors" == tokenStr)
                aResources->maxFragmentInputVectors = value;
            else if ("MinProgramTexelOffset" == tokenStr)
                aResources->minProgramTexelOffset = value;
            else if ("MaxProgramTexelOffset" == tokenStr)
                aResources->maxProgramTexelOffset = value;
            else if ("MaxClipDistances" == tokenStr)
                aResources->maxClipDistances = value;
            else if ("MaxComputeWorkGroupCountX" == tokenStr)
                aResources->maxComputeWorkGroupCountX = value;
            else if ("MaxComputeWorkGroupCountY" == tokenStr)
                aResources->maxComputeWorkGroupCountY = value;
            else if ("MaxComputeWorkGroupCountZ" == tokenStr)
                aResources->maxComputeWorkGroupCountZ = value;
            else if ("MaxComputeWorkGroupSizeX" == tokenStr)
                aResources->maxComputeWorkGroupSizeX = value;
            else if ("MaxComputeWorkGroupSizeY" == tokenStr)
                aResources->maxComputeWorkGroupSizeY = value;
            else if ("MaxComputeWorkGroupSizeZ" == tokenStr)
                aResources->maxComputeWorkGroupSizeZ = value;
            else if ("MaxComputeUniformComponents" == tokenStr)
                aResources->maxComputeUniformComponents = value;
            else if ("MaxComputeTextureImageUnits" == tokenStr)
                aResources->maxComputeTextureImageUnits = value;
            else if ("MaxComputeImageUniforms" == tokenStr)
                aResources->maxComputeImageUniforms = value;
            else if ("MaxComputeAtomicCounters" == tokenStr)
                aResources->maxComputeAtomicCounters = value;
            else if ("MaxComputeAtomicCounterBuffers" == tokenStr)
                aResources->maxComputeAtomicCounterBuffers = value;
            else if ("MaxVaryingComponents" == tokenStr)
                aResources->maxVaryingComponents = value;
            else if ("MaxVertexOutputComponents" == tokenStr)
                aResources->maxVertexOutputComponents = value;
            else if ("MaxGeometryInputComponents" == tokenStr)
                aResources->maxGeometryInputComponents = value;
            else if ("MaxGeometryOutputComponents" == tokenStr)
                aResources->maxGeometryOutputComponents = value;
            else if ("MaxFragmentInputComponents" == tokenStr)
                aResources->maxFragmentInputComponents = value;
            else if ("MaxImageUnits" == tokenStr)
                aResources->maxImageUnits = value;
            else if ("MaxCombinedImageUnitsAndFragmentOutputs" == tokenStr)
                aResources->maxCombinedImageUnitsAndFragmentOutputs = value;
            else if ("MaxCombinedShaderOutputResources" == tokenStr)
                aResources->maxCombinedShaderOutputResources = value;
            else if ("MaxImageSamples" == tokenStr)
                aResources->maxImageSamples = value;
            else if ("MaxVertexImageUniforms" == tokenStr)
                aResources->maxVertexImageUniforms = value;
            else if ("MaxTessControlImageUniforms" == tokenStr)
                aResources->maxTessControlImageUniforms = value;
            else if ("MaxTessEvaluationImageUniforms" == tokenStr)
                aResources->maxTessEvaluationImageUniforms = value;
            else if ("MaxGeometryImageUniforms" == tokenStr)
                aResources->maxGeometryImageUniforms = value;
            else if ("MaxFragmentImageUniforms" == tokenStr)
                aResources->maxFragmentImageUniforms = value;
            else if ("MaxCombinedImageUniforms" == tokenStr)
                aResources->maxCombinedImageUniforms = value;
            else if ("MaxGeometryTextureImageUnits" == tokenStr)
                aResources->maxGeometryTextureImageUnits = value;
            else if ("MaxGeometryOutputVertices" == tokenStr)
                aResources->maxGeometryOutputVertices = value;
            else if ("MaxGeometryTotalOutputComponents" == tokenStr)
                aResources->maxGeometryTotalOutputComponents = value;
            else if ("MaxGeometryUniformComponents" == tokenStr)
                aResources->maxGeometryUniformComponents = value;
            else if ("MaxGeometryVaryingComponents" == tokenStr)
                aResources->maxGeometryVaryingComponents = value;
            else if ("MaxTessControlInputComponents" == tokenStr)
                aResources->maxTessControlInputComponents = value;
            else if ("MaxTessControlOutputComponents" == tokenStr)
                aResources->maxTessControlOutputComponents = value;
            else if ("MaxTessControlTextureImageUnits" == tokenStr)
                aResources->maxTessControlTextureImageUnits = value;
            else if ("MaxTessControlUniformComponents" == tokenStr)
                aResources->maxTessControlUniformComponents = value;
            else if ("MaxTessControlTotalOutputComponents" == tokenStr)
                aResources->maxTessControlTotalOutputComponents = value;
            else if ("MaxTessEvaluationInputComponents" == tokenStr)
                aResources->maxTessEvaluationInputComponents = value;
            else if ("MaxTessEvaluationOutputComponents" == tokenStr)
                aResources->maxTessEvaluationOutputComponents = value;
            else if ("MaxTessEvaluationTextureImageUnits" == tokenStr)
                aResources->maxTessEvaluationTextureImageUnits = value;
            else if ("MaxTessEvaluationUniformComponents" == tokenStr)
                aResources->maxTessEvaluationUniformComponents = value;
            else if ("MaxTessPatchComponents" == tokenStr)
                aResources->maxTessPatchComponents = value;
            else if ("MaxPatchVertices" == tokenStr)
                aResources->maxPatchVertices = value;
            else if ("MaxTessGenLevel" == tokenStr)
                aResources->maxTessGenLevel = value;
            else if ("MaxViewports" == tokenStr)
                aResources->maxViewports = value;
            else if ("MaxVertexAtomicCounters" == tokenStr)
                aResources->maxVertexAtomicCounters = value;
            else if ("MaxTessControlAtomicCounters" == tokenStr)
                aResources->maxTessControlAtomicCounters = value;
            else if ("MaxTessEvaluationAtomicCounters" == tokenStr)
                aResources->maxTessEvaluationAtomicCounters = value;
            else if ("MaxGeometryAtomicCounters" == tokenStr)
                aResources->maxGeometryAtomicCounters = value;
            else if ("MaxFragmentAtomicCounters" == tokenStr)
                aResources->maxFragmentAtomicCounters = value;
            else if ("MaxCombinedAtomicCounters" == tokenStr)
                aResources->maxCombinedAtomicCounters = value;
            else if ("MaxAtomicCounterBindings" == tokenStr)
                aResources->maxAtomicCounterBindings = value;
            else if ("MaxVertexAtomicCounterBuffers" == tokenStr)
                aResources->maxVertexAtomicCounterBuffers = value;
            else if ("MaxTessControlAtomicCounterBuffers" == tokenStr)
                aResources->maxTessControlAtomicCounterBuffers = value;
            else if ("MaxTessEvaluationAtomicCounterBuffers" == tokenStr)
                aResources->maxTessEvaluationAtomicCounterBuffers = value;
            else if ("MaxGeometryAtomicCounterBuffers" == tokenStr)
                aResources->maxGeometryAtomicCounterBuffers = value;
            else if ("MaxFragmentAtomicCounterBuffers" == tokenStr)
                aResources->maxFragmentAtomicCounterBuffers = value;
            else if ("MaxCombinedAtomicCounterBuffers" == tokenStr)
                aResources->maxCombinedAtomicCounterBuffers = value;
            else if ("MaxAtomicCounterBufferSize" == tokenStr)
                aResources->maxAtomicCounterBufferSize = value;
            else if ("MaxTransformFeedbackBuffers" == tokenStr)
                aResources->maxTransformFeedbackBuffers = value;
            else if ("MaxTransformFeedbackInterleavedComponents" == tokenStr)
                aResources->maxTransformFeedbackInterleavedComponents = value;
            else if ("MaxCullDistances" == tokenStr)
                aResources->maxCullDistances = value;
            else if ("MaxCombinedClipAndCullDistances" == tokenStr)
                aResources->maxCombinedClipAndCullDistances = value;
            else if ("MaxSamples" == tokenStr)
                aResources->maxSamples = value;
    #ifdef NV_EXTENSIONS
            else if ("MaxMeshOutputVerticesNV" == tokenStr)
                aResources->maxMeshOutputVerticesNV = value;
            else if ("MaxMeshOutputPrimitivesNV" == tokenStr)
                aResources->maxMeshOutputPrimitivesNV = value;
            else if (tokenStr == "MaxMeshWorkGroupSizeX_NV")
                aResources->maxMeshWorkGroupSizeX_NV = value;
            else if (tokenStr == "MaxMeshWorkGroupSizeY_NV")
                aResources->maxMeshWorkGroupSizeY_NV = value;
            else if (tokenStr == "MaxMeshWorkGroupSizeZ_NV")
                aResources->maxMeshWorkGroupSizeZ_NV = value;
            else if (tokenStr == "MaxTaskWorkGroupSizeX_NV")
                aResources->maxTaskWorkGroupSizeX_NV = value;
            else if (tokenStr == "MaxTaskWorkGroupSizeY_NV")
                aResources->maxTaskWorkGroupSizeY_NV = value;
            else if (tokenStr == "MaxTaskWorkGroupSizeZ_NV")
                aResources->maxTaskWorkGroupSizeZ_NV = value;
            else if ("MaxMeshViewCountNV" == tokenStr)
                aResources->maxMeshViewCountNV = value;
    #endif
            else if ("nonInductiveForLoops" == tokenStr)
                aResources->limits.nonInductiveForLoops = (value != 0);
            else if ("whileLoops" == tokenStr)
                aResources->limits.whileLoops = (value != 0);
            else if ("doWhileLoops" == tokenStr)
                aResources->limits.doWhileLoops = (value != 0);
            else if ("generalUniformIndexing" == tokenStr)
                aResources->limits.generalUniformIndexing = (value != 0);
            else if ("generalAttributeMatrixVectorIndexing" == tokenStr)
                aResources->limits.generalAttributeMatrixVectorIndexing = (value != 0);
            else if ("generalVaryingIndexing" == tokenStr)
                aResources->limits.generalVaryingIndexing = (value != 0);
            else if ("generalSamplerIndexing" == tokenStr)
                aResources->limits.generalSamplerIndexing = (value != 0);
            else if ("generalVariableIndexing" == tokenStr)
                aResources->limits.generalVariableIndexing = (value != 0);
            else if ("generalConstantMatrixVectorIndexing" == tokenStr)
                aResources->limits.generalConstantMatrixVectorIndexing = (value != 0);
            else
                printf("Warning: unrecognized limit (%s) in configuration file.\n", tokenStr.c_str());

        }
    }

}  // end namespace glslang
