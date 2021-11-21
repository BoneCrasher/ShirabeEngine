#ifndef __SHIRABE_ICOMPONENTFACTORY_H__
#define __SHIRABE_ICOMPONENTFACTORY_H__

#include <core/enginetypehelper.h>
#include "ecws/ecomponenttype.h"
#include "ecws/componentsystem.h"

namespace engine::ecws
{

    /**
     * The IComponentFactory interface describes valid factory signatures of a
     * component factory to create specific components.
     */
    class IComponentFactory
    {
        SHIRABE_DECLARE_INTERFACE(IComponentFactory);

    public_api:
        [[nodiscard]]
        virtual Shared<IComponent> createComponent(EComponentType const &aComponentType) const = 0;

    };
}

#endif
