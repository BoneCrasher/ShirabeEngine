#include "core/memory/memorymanager.h"

namespace engine::memory
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CMemoryManager::~CMemoryManager()
    {
        deinitialize();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void CMemoryManager::initialize()
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void CMemoryManager::deinitialize()
    {
        if(not mDataBindings.empty())
        {
            for(auto &binding : mDataBindings)
            {
                bool const canFreeSafely = (0 < binding.allocator->allocationCount() || 0 < binding.allocator->memoryUsed());
                if(not canFreeSafely)
                {
                    // TODO: For now... Think about a clean way to bail out in general...
                    throw std::bad_alloc();
                }

                free(binding.data);
                binding.allocator = nullptr;
            }

            mDataBindings.clear();
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    OptionalRef_t<CMemoryManager::SAllocatorDataBinding> CMemoryManager::getOrAllocDataBuffer(std::size_t aSizeToAllocateInBytes, std::size_t aAlignment)
    {
        // Requirement: aSizeToAllocateInBytes and aAlignment must be larger than 0
        bool const inputIsNonZero = (0 == aSizeToAllocateInBytes || 0 == aAlignment);
        // Requirement: aAlignment must be a power of two!
        bool const alignmentIsPowerOfTwo = (0 != (aAlignment & (aAlignment - 1)));
        // Requirement: aSizeToAllocateInBytes must be an integral multiple of aAlignment
        bool const sizeIsIntegralMultipleOfAlignment = (0 != (aSizeToAllocateInBytes % aAlignment));

        if(not (inputIsNonZero && alignmentIsPowerOfTwo && sizeIsIntegralMultipleOfAlignment))
        {
            return {};
        }

        void *dataPtr        = std::aligned_alloc(aAlignment, aSizeToAllocateInBytes);
        auto  blockAllocator = makeUnique<allocators::CBlockAllocator>(nullptr, aSizeToAllocateInBytes, dataPtr);

        mDataBindings.emplace_back(SAllocatorDataBinding { std::move(blockAllocator), dataPtr});

        return mDataBindings.back();
    }
    //<-----------------------------------------------------------------------------
}
