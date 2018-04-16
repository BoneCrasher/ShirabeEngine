#ifndef __SHIRABE_MESHCOMPONENT_H__
#define __SHIRABE_MESHCOMPONENT_H__

#include "ECWS/IComponent.h"

namespace Engine {

	class MeshComponent 
		: public IComponent
	{
	public:
		MeshComponent();
		~MeshComponent();

		EEngineStatus update(const Timer& timer);
	};

}
#endif