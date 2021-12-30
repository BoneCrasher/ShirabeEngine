//
// Created by dottideveloper on 27.12.21.
//

#ifndef __SHIRABEDEVELOPMENT_ENGINE_LOCATOR_H__
#define __SHIRABEDEVELOPMENT_ENGINE_LOCATOR_H__

#include <base/declaration.h>
#include <platform/platform.h>
#include <core/enginestatus.h>
#include <core/enginetypehelper.h>

namespace engine::util
{
    class SHIRABEENGINE_LIBRARY_EXPORT CEngineLocator final
    {
    public_static_functions:
        SHIRABE_INLINE static CEngineLocator &get()
        {
            static CEngineLocator gLocator;
            return gLocator;
        }

    private_constructors:
        CEngineLocator() = default;

    public_destructors:
        ~CEngineLocator() = default;

    public_methods:
        template <typename TEngineComponentType>
        Shared<TEngineComponentType> locateEngineComponentByType()
        {

        };

        template <typename TEngineComponentType>
        EEngineStatus registerEngineComponentByType(Shared<TEngineComponentType> aComponent)
        {

        }

        template <typename TEngineComponentType>
        EEngineStatus unregisterEngineComponentByType()
        {

        }
    };
}

#endif //__SHIRABEDEVELOPMENT_ENGINE_LOCATOR_H__
