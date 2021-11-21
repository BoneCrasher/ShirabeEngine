#ifndef __SHIRABE_ECWS_UIDISSUER_H__
#define __SHIRABE_ECWS_UIDISSUER_H__

#include <limits.h>
#include <base/declaration.h>
#include <core/enginestatus.h>
#include <core/enginetypehelper.h>

using namespace engine::datastructures;

namespace engine::ecws
{
    template <typename TUidType>
    class CUidIssuer
    {
    public_typedefs:
        using Uid_t     = TUidType;
        using Storage_t = Vector<Uid_t>;

    private_members:
        Storage_t mStorage;

    public_constructors:
        CUidIssuer()
            : mStorage(0)
        {};

    public_destructors:
        ~CUidIssuer()
        {
            mStorage.clear();
        };

    public_methods:
        void initialize(typename Storage_t::size_type aCapacity)
        {
            mStorage.resize(aCapacity);
            for(int k=0; k<aCapacity; ++k)
            {
                // Don't assume congruency of index and value after initialization.
                mStorage[k] = k;
            }
        };

        Uid_t fetchUid()
        {
            Uid_t const uid = mStorage.back();
            mStorage.pop_back();
            return uid;
        };

        void releaseUid(Uid_t aUid)
        {
            mStorage.push_back(aUid);
        }

        typename Storage_t::size_type capacity() const
        {
            return mStorage.size();
        }
    };

    namespace uid::composition
    {
        template<typename TComposedUidType, typename TPrimaryUidType, typename TSecondaryUidType>
        class CUidComposer
        {
        public:
            static TComposedUidType compose(TPrimaryUidType aPrimaryUid, TSecondaryUidType aSecondaryUid)
            {
                static_assert(sizeof(TComposedUidType) == (sizeof(TPrimaryUidType) + sizeof(TSecondaryUidType)), "Can't combine types.");
                return (((TComposedUidType) aPrimaryUid << (CHAR_BIT * sizeof(TSecondaryUidType))) | aSecondaryUid);
            }

            static std::tuple<TPrimaryUidType, TSecondaryUidType> extract(TComposedUidType aComposedUid)
            {
                static_assert(sizeof(TComposedUidType) == (sizeof(TPrimaryUidType) + sizeof(TSecondaryUidType)), "Can't combine types.");
                auto const primary   = (TPrimaryUidType)   (aComposedUid >> (CHAR_BIT * sizeof(TSecondaryUidType)));
                auto const secondary = (TSecondaryUidType) (aComposedUid & ((1 << (CHAR_BIT * sizeof(TSecondaryUidType))) - 1));

                return {primary, secondary};
            }
        };
    }
}
#endif
