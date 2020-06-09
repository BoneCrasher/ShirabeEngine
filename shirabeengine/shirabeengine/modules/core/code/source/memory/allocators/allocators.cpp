//
// Created by dottideveloper on 02.06.20.
//

#include "core/memory/allocators/allocators.h"

namespace engine::memory::allocators
{
    namespace arithmetics
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        std::uintptr_t as_address(void *aPtr)
        {
            return reinterpret_cast<std::uintptr_t>(aPtr);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        void* as_ptr(std::uintptr_t aAddress)
        {
            return reinterpret_cast<void*>(aAddress);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        void* add(void *aPtr, std::size_t aSize)
        {
            return as_ptr(as_address(aPtr) + aSize);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        void* sub(void *aPtr, std::size_t aSize)
        {
            return as_ptr(as_address(aPtr) - aSize);
        }
        //<-----------------------------------------------------------------------------
    }

    namespace alignment
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        void* alignForward(void* aPtr, uint8_t aAlignment)
        {
            return
                arithmetics::as_ptr(arithmetics::as_address(arithmetics::add(aPtr, aAlignment - 1)) & static_cast<std::uintptr_t>(~(aAlignment - 1)));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        uint8_t alignForwardAdjustment(void* aPtr, uint8_t aAlignment)
        {
            uint8_t const adjustment = (aAlignment - arithmetics::as_address(aPtr) & static_cast<std::uintptr_t>(aAlignment - 1));

            if(adjustment == aAlignment)
            {
                return 0;
            }

            return adjustment;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        uint8_t alignForwardAdjustmentWithHeader(void *aPtr, uint8_t aAlignment, uint8_t aHeaderSize)
        {
            uint8_t adjustment  = alignForwardAdjustment(aPtr, aAlignment);
            uint8_t neededSpace = aHeaderSize;

            if(adjustment < neededSpace)
            {
                neededSpace -= adjustment;

                adjustment += (aAlignment * (neededSpace / aAlignment));

                if(0 < (neededSpace % aAlignment))
                {
                    adjustment += aAlignment;
                }
            }

            return adjustment;
        }
        //<-----------------------------------------------------------------------------
    }

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CAllocator::CAllocator(CAllocator *aParentAllocator, std::size_t const aSizeInBytes, void *aDataPtr)
        : mParentAllocator(aParentAllocator)
        , mDataPtr        (aDataPtr)
        , mSizeInBytes    (aSizeInBytes)
        , mMemoryUsed     (0)
        , mAllocationCount(0)
    {
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CAllocator::~CAllocator()
    {
        // Assert memory leaks...
        assert(0 == mAllocationCount && 0 == mMemoryUsed);

        if(nullptr != mParentAllocator)
        {
            mParentAllocator->deallocate(mDataPtr);
        }

        mDataPtr     = nullptr;
        mSizeInBytes = 0;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CLinearAllocator::CLinearAllocator(CAllocator *aParentAllocator, std::size_t aSizeInBytes, void *aDataPtr)
        : CAllocator(aParentAllocator, aSizeInBytes, aDataPtr)
          , mCurrentPtr(nullptr)
    {
        assert(0 != aSizeInBytes && nullptr != aDataPtr);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CLinearAllocator::~CLinearAllocator()
    {
        mCurrentPtr = nullptr;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void* CLinearAllocator::allocate(std::size_t aSizeInBytes, uint8_t aAlignment)
    {
        assert(0 != aSizeInBytes && 0 != aAlignment);

        uint8_t const adjustment = alignment::alignForwardAdjustment(mCurrentPtr, aAlignment);

        if(mSizeInBytes < (mMemoryUsed + adjustment + aSizeInBytes))
        {
            return nullptr;
        }

        std::uintptr_t alignedAddress = arithmetics::as_address(mCurrentPtr) + adjustment;
        mCurrentPtr  = arithmetics::as_ptr(alignedAddress + aSizeInBytes);
        mMemoryUsed += (aSizeInBytes + adjustment);
        ++mAllocationCount;

        return arithmetics::as_ptr(alignedAddress);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void CLinearAllocator::deallocate(void *aPtr)
    {
        assert(false);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void CLinearAllocator::clear()
    {
        mAllocationCount = 0;
        mMemoryUsed      = 0;
        mCurrentPtr      = mDataPtr;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CBlockAllocator::CBlockAllocator(CAllocator *aParentAllocator, std::size_t aSizeInBytes, void *aDataPtr)
        : CAllocator(aParentAllocator, aSizeInBytes, aDataPtr)
        , mBlocks(reinterpret_cast<SBlock*>(aDataPtr))
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CBlockAllocator::~CBlockAllocator()
    {
        mBlocks = nullptr;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void* CBlockAllocator::allocate(std::size_t aSizeInBytes, uint8_t aAlignment)
    {
        assert(0 != aSizeInBytes && 0 != aAlignment);

        SBlock *previousFreeBlock = nullptr;
        SBlock *freeBlock         = mBlocks;

        while(nullptr != freeBlock)
        {
            uint8_t const adjustment = alignment::alignForwardAdjustmentWithHeader(freeBlock, aAlignment, sizeof(SAllocationHeader));
            std::size_t   totalSize  = (aSizeInBytes + adjustment);

            // Current free block is not large enough to hold the requested size.
            // Skip!
            if(totalSize > freeBlock->size)
            {
                previousFreeBlock = freeBlock;
                freeBlock         = freeBlock->next;
                continue;
            }

            // Allocation in the current block is possible.
            // Check whether another allocation of at least a header is possible.
            // If not, just allocate the entire current block.
            // Otherwise, split the remaining memory and enchain a the block
            // between the current block's predecessor and it's successor.
            bool canFitAnotherBlock = (sizeof(SAllocationHeader) >= (freeBlock->size - totalSize));
            if(not canFitAnotherBlock)
            {
                totalSize = freeBlock->size;

                if(nullptr != previousFreeBlock)
                {
                    previousFreeBlock->next = freeBlock->next;
                }
                else
                {
                    mBlocks = freeBlock->next;
                }
            }
            else
            {
                auto *nextBlock = (static_cast<SBlock*>(arithmetics::as_ptr(arithmetics::as_address(freeBlock) + totalSize)));
                nextBlock->size = (freeBlock->size - totalSize);
                nextBlock->next = freeBlock->next;

                if(nullptr != previousFreeBlock)
                {
                    previousFreeBlock->next = nextBlock;
                }
                else
                {
                    mBlocks = nextBlock;
                }
            }

            std::uintptr_t alignedAddress = arithmetics::as_address(freeBlock) + adjustment;

            auto *header = static_cast<SAllocationHeader*>(arithmetics::as_ptr(alignedAddress - sizeof(SAllocationHeader)));
            header->size       = totalSize;
            header->adjustment = adjustment;

            mMemoryUsed += totalSize;
            ++mAllocationCount;

            assert(0 == alignment::alignForwardAdjustment(arithmetics::as_ptr(alignedAddress), aAlignment));

            return arithmetics::as_ptr(alignedAddress);
        }

        return nullptr;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void CBlockAllocator::deallocate(void *aPtr)
    {
        assert(nullptr != aPtr);

        std::uintptr_t address = arithmetics::as_address(aPtr);
        auto          *header  = static_cast<SAllocationHeader*>(arithmetics::as_ptr(address - sizeof(SAllocationHeader)));

        std::uintptr_t    blockStart = address - header->adjustment;
        std::size_t const blockSize  = header->size;
        std::uintptr_t    blockEnd   = (blockStart + blockSize);

        SBlock *previousFreeBlock = nullptr;
        SBlock *freeBlock         = mBlocks;

        // Find the first block immediately after the block to be released.
        while(nullptr != freeBlock)
        {
            if(arithmetics::as_address(freeBlock) >= blockEnd)
            {
                break;
            }

            previousFreeBlock = freeBlock;
            freeBlock         = freeBlock->next;
        }

        if(nullptr == previousFreeBlock)
        {
            // The block to be released is before any other free block. Make it root.
            previousFreeBlock = static_cast<SBlock*>(arithmetics::as_ptr(blockStart));
            previousFreeBlock->size = blockSize;
            previousFreeBlock->next = mBlocks;
            mBlocks = previousFreeBlock;
        }
        else if(blockStart == (arithmetics::as_address(previousFreeBlock) + previousFreeBlock->size))
        {
            // The block to be released is immediately subsequent to the current previous block, enlarge!
            previousFreeBlock->size += blockSize;
        }
        else
        {
            // Any other case: Convert the block to be released to a free block and enchain.
            auto *temp = static_cast<SBlock*>(arithmetics::as_ptr(blockStart));
            temp->size = blockSize;
            temp->next = previousFreeBlock->next;
            previousFreeBlock->next = temp;
            previousFreeBlock       = temp;
        }

        if(nullptr != freeBlock && blockEnd == arithmetics::as_address(freeBlock))
        {
            // If there's a subsequent free block immediately subsequent to the block now returned
            // and stored in/appended to previousFreeBlock, enlarge the freeblock to the entire dimension
            // including the subsequent block.
            previousFreeBlock->size += freeBlock->size;
            previousFreeBlock->next  = freeBlock->next;
        }

        --mAllocationCount;
        mMemoryUsed -= blockSize;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CPoolAllocator::CPoolAllocator(CAllocator *aParentAllocator
                                   , uint32_t    aInstanceSize
                                   , uint8_t     aAlignment
                                   , std::size_t aSizeInBytes
                                   , void       *aDataPtr)
        : CAllocator(aParentAllocator, aSizeInBytes, aDataPtr)
        , mAlignment(aAlignment)
        , mInstanceSize(aInstanceSize)
    {
        // We need to store a pointer to the next instance in-memory.
        // Consequently we need the instance-size to be at least large
        // enough to hold a pointer.
        assert(sizeof(void*) < aInstanceSize);

        uint8_t      const adjustment             = alignment::alignForwardAdjustment(aDataPtr, aAlignment);
        std::size_t  const supportedInstanceCount = ((aSizeInBytes - adjustment) / aInstanceSize);

        mSlots = static_cast<void**>(arithmetics::add(aDataPtr, adjustment));

        void **p = mSlots;
        for(std::size_t k=0; k<(supportedInstanceCount - 1); ++k)
        {
            *p = arithmetics::add(p, aInstanceSize);
            p = (void**)*p;
        }

        // Store a nullptr in the last element's pointer-storage to permit detection of "out of memory"
        *p = nullptr;
    }

    CPoolAllocator::~CPoolAllocator()
    {
        mSlots = nullptr;
    }

    void* CPoolAllocator::allocate(std::size_t aSizeInBytes, uint8_t aAlignment)
    {
        assert(aSizeInBytes == mInstanceSize && aAlignment == mAlignment);

        if(nullptr == mSlots)
        {
            return nullptr;
        }

        void *ptr = mSlots;
        mSlots = static_cast<void**>(*mSlots);

        mMemoryUsed += aSizeInBytes;
        ++mAllocationCount;

        return ptr;
    }

    void CPoolAllocator::deallocate(void *aPtr)
    {
        // We prepend the returned element to the slot-array!
        // If mSlots is nullptr, the sequence is still setup properly.
        // If not, the current mSlots array will be referenced.
        // Finally, make the prepended element the new root.
        *(static_cast<void**>(aPtr)) = mSlots;
        mSlots = static_cast<void**>(aPtr);

        mMemoryUsed -= mInstanceSize;
        --mAllocationCount;
    }
}
