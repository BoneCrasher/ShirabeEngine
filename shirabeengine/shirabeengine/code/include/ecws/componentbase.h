#ifndef __SHIRABE_COMPONENT_BASE_H__
#define __SHIRABE_COMPONENT_BASE_H__

#include "ecws/icomponent.h"

namespace engine::ecws
{
    class CEntity;

	class CComponentBase
		: public IComponent
	{
	    friend class CEntity;

	public_constructors:
	    SHIRABE_INLINE
	    CComponentBase(std::string const &aName)
	        : IComponent()
	        , mName(aName)
        {}

    public_destructors:
		SHIRABE_INLINE
	    ~CComponentBase() override
        {
	        mParentEntity = nullptr;
        };

	public_methods:
	    [[nodiscard]]
	    SHIRABE_INLINE
	    std::string const &name() const final { return mName; }

    protected_methods:
	    SHIRABE_INLINE
        void setParentEntity(class CEntity const *const aEntity) final
        {
	        mParentEntity = aEntity;
        }

	    [[nodiscard]]
	    SHIRABE_INLINE
	    CEntity const *const getParentEntity() const
        {
	        return mParentEntity;
        }

	private_members:
	    std::string    mName;
	    CEntity const *mParentEntity;
	};

}
#endif
