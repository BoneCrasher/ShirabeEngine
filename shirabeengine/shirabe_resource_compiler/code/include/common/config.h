//
// Created by dotti on 08.12.19.
//

#ifndef __SHIRABEDEVELOPMENT_CONFIG_H__
#define __SHIRABEDEVELOPMENT_CONFIG_H__

#include <vector>
#include <core/bitfield.h>

/**
 * Enumeration describing various tool options to be respected while processing.
 */
enum class EOptions
        : uint64_t
{
    None                 = 0,
    VerboseOutput        = (1lu << 1lu),
    DebugMode            = (1lu << 2lu),
    OptimizationEnabled  = (1lu << 3lu),
    MultiThreaded        = (1lu << 4lu),
    RecursiveScan        = (1lu << 5lu),
    DumpConfig           = (1lu << 6lu),
    DumpReflection       = (1lu << 7lu),
    DumpBareVersion      = (1lu << 8lu),
};


/**
 * @brief The SConfiguration struct
 */
struct SConfiguration
{
    std::filesystem::path                inputPath;
    std::filesystem::path                outputPath;
    std::vector<std::filesystem::path>   includePaths;
    std::vector<std::filesystem::path>   filesToProcess;
    engine::core::CBitField<EOptions>    options;
};

#endif //__SHIRABEDEVELOPMENT_CONFIG_H__
