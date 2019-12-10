#ifndef __SHIRABE_COMPONENT_MESH_H__
#define __SHIRABE_COMPONENT_MESH_H__

#include <mesh/declaration.h>

#include "icomponent.h"

namespace engine::ecws
{

	class CMeshComponent
		: public IComponent
	{
	public_constructors:
		CMeshComponent();

    public_destructors:
		~CMeshComponent() override;

    public_methods:
        [[nodiscard]]
        SHIRABE_INLINE
        std::string const &name() const final { return mName; }

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
        std::string                 mName;
	    Shared<mesh::CMeshInstance> mMeshInstance;
	};

}
#endif
