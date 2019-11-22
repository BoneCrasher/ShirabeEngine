#ifndef __SHIRABE_COMPONENT_MESH_H__
#define __SHIRABE_COMPONENT_MESH_H__

#include "icomponent.h"

namespace engine::ecws
{

	class CMeshComponent
		: public IComponent
	{
	public:
		CMeshComponent();
		~CMeshComponent() override;

		EEngineStatus update(CTimer const &aTimer) override;
	};

}
#endif
