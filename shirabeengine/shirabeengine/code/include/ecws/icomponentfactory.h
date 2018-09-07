#ifndef __SHIRABE_ICOMPONENTFACTORY_H__
#define __SHIRABE_ICOMPONENTFACTORY_H__

#include <core/enginetypehelper.h>
#include "ecws/icomponent.h"

namespace engine
{
    /**
     * The IComponentFactory interface describes valid factory signatures of a
     * component factory to create specific components.
     */
    class IComponentFactory
    {
        SHIRABE_DECLARE_INTERFACE(IComponentFactory);

    public_api:
        virtual CStdSharedPtr_t<IComponent> createComponent(/* id */) const = 0;

    };
}

#endif
