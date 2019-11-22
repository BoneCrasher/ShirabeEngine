#include "ecws/meshcomponent.h"

namespace engine::ecws
{
    CMeshComponent::CMeshComponent()
        : IComponent()
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
