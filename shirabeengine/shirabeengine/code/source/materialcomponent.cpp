#include "ecws/materialcomponent.h"

namespace engine
{
    CMaterialComponent::CMaterialComponent()
        : IComponent()
        , mMaterialInstance(nullptr)
    {}

    CMaterialComponent::~CMaterialComponent()
    {
        mMaterialInstance = nullptr;
    }

    EEngineStatus CMaterialComponent::update(CTimer const &aTimer)
    {
        SHIRABE_UNUSED(aTimer);

        return EEngineStatus::Ok;
    }
}
