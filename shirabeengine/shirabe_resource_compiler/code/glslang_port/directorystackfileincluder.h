//
// Copyright (C) 2002-2005  3Dlabs Inc. Ltd.
// Copyright (C) 2017 Google, Inc.
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
//    Neither the name of 3Dlabs Inc. Ltd. nor the names of its
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
//

#ifndef __SHIRABE_THIRDPARTY_STAND_ALONE_DIRSTACK_FILEINCLUDER_INCLUDED__
#define __SHIRABE_THIRDPARTY_STAND_ALONE_DIRSTACK_FILEINCLUDER_INCLUDED__

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

#include <glslang/Public/ShaderLang.h>

#include <base/declaration.h>
#include <base/stl_container_helpers.h>

/**
 * Default include class for normal include convention of search backward
 * through the stack of active include paths (for nested includes).
 * Can be overridden to customize.
 */
class CDirectoryStackFileIncluder
        : public glslang::TShader::Includer
{
public_constructors:
    CDirectoryStackFileIncluder()
        : mExternalLocalDirectoryCount(0)
    { }

public_destructors:
    virtual ~CDirectoryStackFileIncluder() override = default;

private_typedefs:
    typedef char UserDataElement_t;

public_methods:
    /**
     * @brief includeLocal
     * @param aHeaderName
     * @param aIncluderName
     * @param aInclusionDepth
     * @return
     */
    virtual IncludeResult* includeLocal(char const *aHeaderName,
                                        char const *aIncluderName,
                                        size_t      aInclusionDepth) override
    {
        return readLocalPath(aHeaderName, aIncluderName, aInclusionDepth);
    }

    /**
     * @brief includeSystem
     * @param headerName
     * @return
     */
    virtual IncludeResult* includeSystem(char const *aHeaderName,
                                         char const *aIncluderName,
                                         size_t      aInclusionDepth) override
    {
        SHIRABE_UNUSED(aIncluderName);
        SHIRABE_UNUSED(aInclusionDepth);

        return readSystemPath(aHeaderName);
    }

    /**
     * Externally set directories. E.g., from a command-line -I<dir>.
     * - Most-recently pushed are checked first.
     * - All these are checked after the parse-time stack of local directories
     *   is checked.
     * - This only applies to the "local" form of #include.
     * - Makes its own copy of the path.
     *
     * @param aDirectory
     */
    virtual void pushExternalLocalDirectory(std::string const &aDirectory)
    {
        mDirectoryStack.push_back(aDirectory);

        mExternalLocalDirectoryCount = mDirectoryStack.size();
    }

    /**
     * @brief releaseInclude
     * @param result
     */
    virtual void releaseInclude(IncludeResult *aResult) override
    {
        if (nullptr != aResult)
        {
            UserDataElement_t *userData = static_cast<UserDataElement_t*>(aResult->userData);
            delete [] userData;
            delete aResult;
        }
    }


protected_methods:

    /**
     * Search for a valid "local" path based on combining the stack of include
     * directories and the nominal name of the header.
     *
     * @param headerName
     * @param includerName
     * @param depth
     * @return
     */
    virtual IncludeResult* readLocalPath(char const *aHeaderName,
                                         char const *aIncluderName,
                                         size_t      aDepth)
    {
        // Discard popped include directories, and
        // initialize when at parse-time first level.
        mDirectoryStack.resize(aDepth + mExternalLocalDirectoryCount);

        if (1 == aDepth)
        {
            mDirectoryStack.back() = getDirectory(aIncluderName);
        }

        engine::CReverseIterationAdapter reverseSeq(mDirectoryStack);

        // Find a directory that works, using a reverse search of the include stack.
        for(std::string const &aDirectory : reverseSeq)
        {
            std::string path = aDirectory + '/' + aHeaderName;

            std::replace(path.begin(), path.end(), '\\', '/');

            std::ifstream file(path, std::ios_base::binary | std::ios_base::ate);
            if (file.operator bool())
            {
                std::string directory = getDirectory(path);
                mDirectoryStack.push_back(directory);

                IncludeResult *result = newIncludeResult(path, file, file.tellg());
                return result;
            }
        }

        return nullptr;
    }

    /**
     * Search for a valid <system> path.
     * Not implemented yet; returning nullptr signals failure to find.
     *
     * @param aHeaderName
     * @return
     */
    virtual IncludeResult* readSystemPath(char const *aHeaderName) const
    {
        SHIRABE_UNUSED(aHeaderName);

        return nullptr;
    }

    /**
     * Do actual reading of the file, filling in a new include result.
     *
     * @param path
     * @param file
     * @param length
     * @return
     */
    virtual IncludeResult* newIncludeResult(std::string   const &aPath,
                                            std::ifstream       &aFile,
                                            int64_t      const  aLength) const
    {
        char* content = new UserDataElement_t[static_cast<uint64_t>(aLength)];

        aFile.seekg(0, aFile.beg);
        aFile.read(content, aLength);

        IncludeResult *result = new IncludeResult(aPath, content, static_cast<uint64_t>(aLength), content);
        return result;
    }

    /**
     * If no path markers, return current working directory.
     * Otherwise, strip file name and return path leading up to it.
     *
     * @param aPath
     * @return
     */
    virtual std::string getDirectory(std::string const &aPath) const
    {
        size_t last = aPath.find_last_of("/\\");

        return (std::string::npos == last)
                    ? "."
                    : aPath.substr(0, last);
    }

private_members:
    std::vector<std::string> mDirectoryStack;
    size_t                   mExternalLocalDirectoryCount;
};

#endif
