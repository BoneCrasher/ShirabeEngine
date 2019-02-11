//
// Copyright (C) 2013 LunarG, Inc.
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
#ifndef __SHIRABE_THIRDPARTY_GLSLANG_WORKLIST_H__
#define __SHIRABE_THIRDPARTY_GLSLANG_WORKLIST_H__

// #include "../glslang/OSDependent/osinclude.h"
#include <list>
#include <mutex>
#include <string>
#include <base/declaration.h>
#include <platform/platform.h>

namespace glslang_wrapper
{
    /**
     * @brief The CWorkItem class
     */
    class CWorkItem
    {
    public_constructors:
        /**
         * @brief TWorkItem
         */
        CWorkItem() = default;

        /**
         * @brief TWorkItem
         * @param s
         */
        explicit CWorkItem(std::string const &aItemName)
            : mItemName(aItemName)
        { }

        /**
         * @brief name
         * @return
         */
        std::string const &name() const
        {
            return mItemName;
        }

        /**
         * @brief name
         * @return
         */
        std::string &name()
        {
            return mItemName;
        }

        /**
         * @brief results
         * @return
         */
        std::string const &results() const
        {
            return mResults;
        }

        /**
         * @brief results
         * @return
         */
        std::string &results()
        {
            return mResults;
        }

    private_members:
        std::string mItemName;
        std::string mResults;
        std::string mResultsIndex;
    };

    /**
     * @brief The TWorklist class
     */
    class CWorklist
    {
    public_constructors:
        /**
         * @brief TWorklist
         */
        CWorklist() = default;

    public_destructors:
        virtual ~CWorklist() = default;

    public_methods:
        /**
         * @brief add
         * @param aItem
         */
        void add(CWorkItem* aItem)
        {
            std::lock_guard<std::mutex> guard(mMutex);
            mWorklist.push_back(aItem);
        }

        /**
         * @brief remove
         * @param aOutItem
         * @return
         */
        bool remove(CWorkItem*& aOutItem)
        {
            std::lock_guard<std::mutex> guard(mMutex);

            if (mWorklist.empty())
            {
                return false;
            }

            aOutItem = mWorklist.front();
            mWorklist.pop_front();

            return true;
        }

        /**
         * @brief size
         * @return
         */
        SHIRABE_INLINE uint32_t size()
        {
            return mWorklist.size();
        }

        /**
         * @brief empty
         * @return
         */
        SHIRABE_INLINE bool empty()
        {
            return mWorklist.empty();
        }

    protected:
        std::mutex            mMutex;
        std::list<CWorkItem*> mWorklist;
    };

} // end namespace glslang

#endif
