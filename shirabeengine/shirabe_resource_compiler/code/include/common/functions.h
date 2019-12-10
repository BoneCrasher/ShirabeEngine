//
// Created by dotti on 10.12.19.
//

#ifndef __SHIRABEDEVELOPMENT_FUNCTIONS_H__
#define __SHIRABEDEVELOPMENT_FUNCTIONS_H__

#include <filesystem>

namespace resource_compiler
{
    auto checkPathExists(std::filesystem::path const &aPath) -> void;
}

#endif //__SHIRABEDEVELOPMENT_FUNCTIONS_H__
