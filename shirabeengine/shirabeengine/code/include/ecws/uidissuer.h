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
        using Uid_t = TUidType;

    private_members:
        Uid_t         mUid;
        Vector<Uid_t> mReturnedUids;

    public_constructors:
        CUidIssuer()
            : mUid(0)
            , mReturnedUids()
        {};

    public_destructors:
        ~CUidIssuer()
        {
            mUid = 0;
            mReturnedUids.clear();
        };

    public_methods:
        Uid_t fetchNextAvailableUid()
        {
            if(mReturnedUids.empty())
            {
                return (++mUid);
            }

            return mReturnedUids.front();
        };

        void releaseUid(Uid_t aUid)
        {
            mReturnedUids.push_back(aUid);
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

            static TPrimaryUidType extractPrimary(TComposedUidType aComposedUidType)
            {
                static_assert(sizeof(TComposedUidType) == (sizeof(TPrimaryUidType) + sizeof(TSecondaryUidType)), "Can't combine types.");
                return (TPrimaryUidType) (aComposedUidType >> (CHAR_BIT * sizeof(TSecondaryUidType)));
            }

            static TSecondaryUidType extractSecondary(TComposedUidType aComposedUidType)
            {
                static_assert(sizeof(TComposedUidType) == (sizeof(TPrimaryUidType) + sizeof(TSecondaryUidType)), "Can't combine types.");
                return (TSecondaryUidType) (aComposedUidType & ((1 << (CHAR_BIT * sizeof(TSecondaryUidType))) - 1));
            }
        };
    }
}
#endif
