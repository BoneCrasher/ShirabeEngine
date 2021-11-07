#ifndef __SHIRABE_ENTITY_H__
#define __SHIRABE_ENTITY_H__

#include <core/enginestatus.h>
#include <core/benchmarking/timer/timer.h>
#include "ecws/componentbase.h"

namespace engine::ecws
{
    /**
     * A CEntity instance describes an engine entity, which can be enriched by various
     * types of engine components.
     */

    class CEntity
    {
    public_constructors:
        explicit CEntity(std::string aName);

    public_destructors:
        ~CEntity();

    public_methods:
        Unique<CComponentBase>& rootComponent() { return mRootComponent; };
        Unique<CComponentBase> const& rootComponent() const { return mRootComponent; };

        EEngineStatus initialize();
        EEngineStatus deinitialize();

        EEngineStatus update(CTimer const &aTimer);
    private_members:
        std::string mName;
        Unique<CComponentBase> mRootComponent;
    };

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SHIRABE_DECLARE_LIST_OF_TYPE(Unique<CEntity>, Entity);
    //<-----------------------------------------------------------------------------
}

#endif
