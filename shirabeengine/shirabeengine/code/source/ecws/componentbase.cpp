#include "ecws/componentbase.h"
#include <core/benchmarking/timer/timer.h>

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CComponentBase::CComponentBase(std::string aName)
        : mName(std::move(aName))
        , mParent()
        , mChildren()
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CComponentBase::initialize()
    {
        EEngineStatus status = EEngineStatus::Ok;
        for(auto const &child : mChildren)
        {
            if(auto comp = child.component)
            {
                EEngineStatus const childStatus = comp->initialize();
                if(CheckEngineError(childStatus))
                {
                    // TODO: Log
                    status = EEngineStatus::ECWS_ChildComponentInitError;
                }
            }
        }

        return status;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CComponentBase::deinitialize()
    {
        EEngineStatus status = EEngineStatus::Ok;
        for(auto const &child : mChildren)
        {
            if(auto comp = child.component)
            {
                EEngineStatus const childStatus = comp->deinitialize();
                if(CheckEngineError(childStatus))
                {
                    // TODO: Log
                    status = EEngineStatus::ECWS_ChildComponentInitError;
                }
            }
        }
        mChildren.clear();

        return status;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CComponentBase::update(CTimer const &aTimer)
    {
        for(auto const &child : mChildren )
        {
            if(auto p = child.component)
            {
                EEngineStatus const status = p->update(aTimer);
                if(CheckEngineError(status))
                {
                    // TODO: Log
                }
            }
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    void CComponentBase::setParent(Weak<CComponentBase> aEntity)
    {
        mParent = std::move(aEntity);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
}
