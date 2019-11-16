#include "ecws/meshcomponent.h"

namespace engine
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
    }
}
