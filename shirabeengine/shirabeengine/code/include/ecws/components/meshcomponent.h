#ifndef __SHIRABE_COMPONENT_MESH_H__
#define __SHIRABE_COMPONENT_MESH_H__

#include <asset/mesh/declaration.h>

#include "ecws/componentsystem.h"

namespace engine::ecws
{
    struct SMeshComponentState
    {
        Shared<mesh::CMeshInstance> mesh;
    };

	class CMeshComponent
		: public AComponentBase<SMeshComponentState>
	{
	public_constructors:
		explicit CMeshComponent(String aName);

    public_destructors:
		~CMeshComponent() override;

    public_methods:
		EEngineStatus update(CTimer const &aTimer) override;

		SHIRABE_INLINE
		void setMeshInstance(Shared<mesh::CMeshInstance> aInstance)
        {
            getMutableComponentState().mesh = std::move(aInstance);
        }

        SHIRABE_INLINE
        Weak<mesh::CMeshInstance> getMeshInstance()
        {
		    return getMutableComponentState().mesh;
        }
	};

}
#endif
