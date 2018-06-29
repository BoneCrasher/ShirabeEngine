#ifndef __SHIRABE_COMPONENT_H__
#define __SHIRABE_COMPONENT_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"

#include "Core/Benchmarking/Timer/Timer.h"

namespace Engine {

	DeclareInterface(IComponent);

	virtual const std::string& name() const = 0;
	virtual void name(const std::string& name) = 0;

	/**************************************************************************************************
	 * Fn:	virtual EEngineStatus update(const Timer& timer) = 0;
	 *
	 * Summary:	Update the component and provide current time state for optional use.
	 *
	 * Parameters:
	 * timer - 	The first parameter.
	 *
	 * Returns:
	 *  EEngineStatus.Ok, if the component was updated successfully. An arbitrary error code
	 *  otherwise.
	 **************************************************************************************************/
	virtual EEngineStatus update(const Timer& timer) = 0;

	DeclareInterfaceEnd(IComponent);
	DeclareSharedPointerType(IComponent);
	DeclareListType(IComponentPtr, IComponent);
	DeclareTemplateListType(TComponent, TComponent);

}

#endif