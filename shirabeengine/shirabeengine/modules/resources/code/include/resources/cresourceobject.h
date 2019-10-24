//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__

#include <platform/platform.h>

#include "resources/iresourceobject.h"

class CResourceManager; // Declared/defined in resources/cresourcemanager.h

namespace engine
{
    namespace resources
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescription>
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT CResourceObject
            : public IResourceObject
        {
            friend class CResourceManager;
        public_typedefs:
            using Descriptor_t = TDescription;

        public_constructors:
            explicit CResourceObject(TDescription const &aDescription);

        public_destructors:
            ~CResourceObject() override = default;

        public_api:
            // IResourceObject
            CEngineResult<> bind()     override;
            CEngineResult<> unbind()   override;
            CEngineResult<> transfer() override;

        private_api:
            Shared<IResourceObjectPrivate> getPrivateObject() override;

        public_methods:
            SHIRABE_INLINE TDescription const &getDescription() const
            { return mDescription; }

        private_members:
            TDescription                   const mDescription;
            Shared<IResourceObjectPrivate>       mPrivateObject;
        };

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CResourceObject<TDescriptor>::CResourceObject(const TDescriptor &aDescription)
            : mDescription(aDescription)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> CResourceObject<TDescriptor>::bind()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> CResourceObject<TDescriptor>::unbind()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        CEngineResult<> CResourceObject<TDescriptor>::transfer()
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TDescriptor>
        Shared<IResourceObjectPrivate> CResourceObject<TDescriptor>::getPrivateObject()
        {
            return mPrivateObject;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_CRESOURCEOBJECT_H__
