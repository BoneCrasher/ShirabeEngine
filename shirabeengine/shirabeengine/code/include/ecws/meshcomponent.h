#ifndef __SHIRABE_COMPONENT_MESH_H__
#define __SHIRABE_COMPONENT_MESH_H__

#include <mesh/declaration.h>

#include "ecws/componentbase.h"

namespace engine::ecws
{

	class CMeshComponent
		: public CComponentBase
	{
	public_constructors:
		CMeshComponent(std::string const &aName);

    public_destructors:
		~CMeshComponent() override;

    public_methods:

		EEngineStatus update(CTimer const &aTimer) override;

		SHIRABE_INLINE
		void setMeshInstance(Shared<mesh::CMeshInstance> aInstance)
        {
		    mMeshInstance = std::move(aInstance);
        }

        SHIRABE_INLINE
        Shared<mesh::CMeshInstance> const getMeshInstance()
        {
		    return mMeshInstance;
        }

	private:
	    Shared<mesh::CMeshInstance> mMeshInstance;
	};

}
#endif
