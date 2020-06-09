//
// Created by dottideveloper on 02.06.20.
//
#ifndef __SHIRABEDEVELOPMENT_ALLOCATORS_H__
#define __SHIRABEDEVELOPMENT_ALLOCATORS_H__

#include <cassert>
#include <cstdint>

#include <platform/platform.h>
#include <base/declaration.h>

namespace engine::memory::allocators
{
    namespace arithmetics
    {
        void* add(void *aPtr, std::size_t aSize);
        void* sub(void *aPtr, std::size_t aSize);
    }

    namespace alignment
    {
        void* alignForward(void* aAddress, uint8_t aAlignment);

        uint8_t alignForwardAdjustment(void const* aAddress, uint8_t aAlignment);

        uint8_t alignForwardAdjustmentWithHeader(void const* aAddress, uint8_t aAlignment, uint8_t aHeaderSize);
    }

    class CAllocator
    {
        public_constructors:
            CAllocator(CAllocator *aParentAllocator, std::size_t aSizeInBytes, void *aDataPtr);

        public_destructors:
            virtual ~CAllocator();

        public_methods:
            [[nodiscard]]
            void* data() const { return mDataPtr; }

            [[nodiscard]]
            void* data() { return mDataPtr; }

            [[nodiscard]] std::size_t size()            const { return mSizeInBytes; }
            [[nodiscard]] std::size_t memoryUsed()      const { return mMemoryUsed; }
            [[nodiscard]] std::size_t allocationCount() const { return mAllocationCount; }

            virtual void* allocate(std::size_t aSizeInBytes, uint8_t const aAlignment) = 0;
            virtual void  deallocate(void *aPtr) = 0;

        protected_members:
            CAllocator *mParentAllocator;
            void       *mDataPtr;
            std::size_t mSizeInBytes;
            std::size_t mMemoryUsed;
            std::size_t mAllocationCount;
    };

    template <typename T>
    T* allocate(CAllocator &aAllocator, uint8_t const aAlignment)
    {
        return new (aAllocator.allocate(sizeof(T), aAlignment)) T;
    }

    template <typename T>
    T* allocate(CAllocator &aAllocator, T const &aInstance, uint8_t const aAlignment)
    {
        return new (aAllocator.allocate(sizeof(T), aAlignment)) T(aInstance);
    }

    template <typename T>
    void deallocate(CAllocator &aAllocator, T &aInstance)
    {
        aInstance.~T();
        aAllocator.deallocate(&aInstance);
    }

    template <typename T>
    T* allocateArray(CAllocator &aAllocator, std::size_t const aLength, uint8_t const aAlignment)
    {
        uint8_t const headerSize = (sizeof(std::size_t) / sizeof(T));

        T* ptr = ( static_cast<T*>(aAllocator.allocate(sizeof(T) * (aLength + headerSize), aAlignment)) ) + headerSize;
        *(static_cast<std::size_t*>(ptr) - 1) = aLength;

        for(std::size_t k=0; k<aLength; ++k)
        {
            new (ptr + k) T;
        }

        return ptr;
    }

    template <typename T>
    void deallocateArray(CAllocator &aAllocator, T *aPtr)
    {
        std::size_t const length = (static_cast<std::uintptr_t>(aPtr) - 1);

        for(std::size_t k=0; k<length; ++k)
        {
            (aPtr + k)->~T();
        }

        uint8_t headerSize = (sizeof(std::size_t) / sizeof(T));
        if(0 < (sizeof(std::size_t) % sizeof(T)))
        {
            headerSize += 1;
        }

        aAllocator.deallocate(aPtr - headerSize);
    }

    template <typename T>
    T* allocate(CAllocator &aAllocator)
    {
        return allocate<T>(aAllocator, __alignof(T));
    }

    template <typename T>
    T* allocate(CAllocator &aAllocator, T const &aInstance)
    {
        return new (aAllocator.allocate(sizeof(T), aInstance, __alignof(T))) T(aInstance);
    }

    template <typename T>
    T* allocateArray(CAllocator &aAllocator, std::size_t const aLength)
    {
        return allocateArray<T>(aAllocator, aLength, __alignof(T));
    }

    class CLinearAllocator
        : public CAllocator
    {
        public_constructors:
            CLinearAllocator(CAllocator *aParentAllocator, std::size_t aSizeInBytes, void *aDataPtr);

            CLinearAllocator(CLinearAllocator const &) = delete;
            CLinearAllocator &operator=(CLinearAllocator const &) = delete;

        public_destructors:
            ~CLinearAllocator() final;

        public_methods:
            void* allocate(std::size_t aSizeInBytes, uint8_t aAlignment) final;
            void  deallocate(void *aPtr) final;

            void clear();

        private_members:
           void *mCurrentPtr;
    };

    class CBlockAllocator
        : public CAllocator
    {
        private_structs:
        struct SAllocationHeader
        {
            std::size_t size;
            uint8_t     adjustment;
        };

        struct SBlock
        {
            std::size_t size;
            SBlock      *next;
        };

        public_constructors:
            CBlockAllocator(CAllocator *aParentAllocator, std::size_t aSizeInBytes, void *aDataPtr);

            CBlockAllocator(CBlockAllocator const&) = delete;
            CBlockAllocator &operator=(CBlockAllocator const&) = delete;

        public_destructors:
            ~CBlockAllocator() final;

        public_methods:
            void* allocate(std::size_t aSizeInBytes, uint8_t aAlignment) final;
            void deallocate(void *aPtr) final;

        private_members:
            SBlock *mBlocks;
    };

    class CPoolAllocator
        : public CAllocator
    {
        public_constructors:
            CPoolAllocator(CAllocator   *aParentAllocator
                           , uint32_t    aInstanceSize
                           , uint8_t     aAlignment
                           , std::size_t aSizeInBytes
                           , void       *aDataPtr);

            CPoolAllocator(CPoolAllocator const&) = delete;
            CPoolAllocator &operator=(CPoolAllocator const &) = delete;

        public_destructors:
            ~CPoolAllocator() final;

        public_methods:
            void* allocate(std::size_t aSizeInBytes, uint8_t aAlignment) final;
            void  deallocate(void *aPtr) final;

        private_members:
            uint8_t     mAlignment;
            std::size_t mInstanceSize;
            void      **mSlots;
    };
}

#endif //__SHIRABEDEVELOPMENT_ALLOCATORS_H__
