#ifndef __SHIRABE_ICOMPONENTFACTORY_H__
#define __SHIRABE_ICOMPONENTFACTORY_H__

#include "Core/EngineTypeHelper.h"
#include "ECWS/IComponent.h"

namespace engine {

	DeclareInterface(IComponentFactory);

	virtual IComponentPtr createComponent(/* id */) const = 0;

	DeclareInterfaceEnd(IComponentFactory);
	DeclareSharedPointerType(IComponentFactory);

}

#endif