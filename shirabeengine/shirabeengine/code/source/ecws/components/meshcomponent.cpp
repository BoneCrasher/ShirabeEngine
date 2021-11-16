#include "ecws/components/meshcomponent.h"
#include "ecws/entity.h"

namespace engine::ecws
{
    CMeshComponent::CMeshComponent(String aName)
        : AComponentBase<SMeshComponentState>(std::move(aName))
    {

    }

    CMeshComponent::~CMeshComponent()
    {

    }

    EEngineStatus CMeshComponent::update(CTimer const &aTimer)
    {
        SHIRABE_UNUSED(aTimer);

        return EEngineStatus::Ok;
    }
}
