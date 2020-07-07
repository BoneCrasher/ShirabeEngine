//
// Created by dotti on 10.12.19.
//
#include "common/functions.h"
#include "common/definition.h"
#include <log/log.h>

namespace resource_compiler
{
    auto checkPathExists(std::filesystem::path const &aPath) -> void
    {
        std::filesystem::path path = aPath;

        CLog::Error(logTag(), "Making sure that '{}' exists...", path.string());

        bool const pathExists = std::filesystem::exists(path);
        bool const pathIsFile = not std::filesystem::is_directory(path);
        if(pathExists and pathIsFile)
        {
            path = aPath.parent_path();
        }

        if(not pathExists)
        {
            try
            {
                bool const created = std::filesystem::create_directories(path);
                if(not created)
                {
                    CLog::Error(logTag(), "Can't create directory '{}'", path.string());
                }
            }
            catch(std::filesystem::filesystem_error const &fserr)
            {
                CLog::Error(logTag(), "Cant create directory '{}'. Error: {}", path.string(), fserr.what());
            }
        }
    }
}
