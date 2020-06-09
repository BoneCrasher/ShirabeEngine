#ifndef __SHIRABEDEVELOPMENT_MEMORYMANAGER_H__
#define __SHIRABEDEVELOPMENT_MEMORYMANAGER_H__

#include <platform/platform.h>
#include <base/declaration.h>
#include "core/enginetypehelper.h"
#include "core/memory/allocators/allocators.h"

namespace engine::memory
{
    class SHIRABE_LIBRARY_EXPORT CMemoryManager
    {
    private_structs:
        struct SAllocatorDataBinding
        {
            Unique<allocators::CBlockAllocator> allocator;
            void *data;
        };

    public_constructors:
        CMemoryManager() = default;
        CMemoryManager(CMemoryManager &&aOther) = default;
        CMemoryManager(CMemoryManager const &)  = delete;

    public_destructors:
        ~CMemoryManager();

    public_operators:
        CMemoryManager& operator=(CMemoryManager &&aOther) = default;
        CMemoryManager& operator=(CMemoryManager const &)  = delete;

    public_methods:
        void initialize();

        void deinitialize();

        template <typename TAllocator, typename... TArgs>
        Shared<TAllocator> getAllocator(std::size_t aSizeToAllocInBytes, std::size_t aAlignment, TArgs &&...aArgs)
        {
            OptionalRef_t<CMemoryManager::SAllocatorDataBinding> bindingOpt = getOrAllocDataBuffer(aSizeToAllocInBytes, aAlignment);
            if(not bindingOpt.has_value())
            {
                return nullptr;
            }
            CMemoryManager::SAllocatorDataBinding &binding = *bindingOpt;

            void *data = binding.allocator->allocate(aSizeToAllocInBytes, aAlignment);

            Shared<TAllocator> allocator = makeShared<TAllocator>(binding.allocator.get(), std::forward<TArgs>(aArgs)..., aSizeToAllocInBytes, data);
            return allocator;
        }

    private_methods:
        OptionalRef_t<CMemoryManager::SAllocatorDataBinding> getOrAllocDataBuffer(std::size_t aSizeToAllocateInBytes, std::size_t aAlignment);

    private_members:
        std::vector<SAllocatorDataBinding> mDataBindings;
    };
}

#endif //__SHIRABEDEVELOPMENT_MEMORYMANAGER_H__
