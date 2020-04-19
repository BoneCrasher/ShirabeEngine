#include "ecws/meshcomponent.h"
#include "ecws/entity.h"

namespace engine::ecws
{
    CMeshComponent::CMeshComponent(std::string const &aName)
        : CComponentBase(aName)
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
