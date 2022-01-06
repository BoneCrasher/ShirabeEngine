//
// Created by dottideveloper on 22.11.19.
//
#include "ecws/entity.h"

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEntity::CEntity(String aName)
        : mUid(0)
        , mName(std::move(aName))
        , mRootComponent(nullptr)
        , mAssignedComponents()
        , mComponentHierarchy()
        , mInitialized(false)
        , mStateIsValid(false)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEntity::~CEntity()
    {
        deinitialize();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool CEntity::foreachChildOf(Shared<IComponent>                                 &aComponent
                                 , std::function<EEngineStatus(Shared<IComponent>&)> aForeachChildFn
                                 , CAdjacencyTree<PublicComponentId_t>::EOrder       aOrder)
    {
        auto const iterator =
               [&, this] (PublicComponentId_t  const &aParentId
                         , PublicComponentId_t const &aChildId) -> bool
                   {
                       SHIRABE_UNUSED(aParentId);
                       auto &currentChild = mAssignedComponents.at(aChildId);
                       if(nullptr != currentChild)
                       {
                           EEngineStatus const status = aForeachChildFn(currentChild);
                           #if SHIRABE_DEBUG
                           if(CheckEngineError(status))
                           {
                               CLog::Error(logTag(), TEXT("Failed to process component '%s' of entity '%s'"), currentChild->getComponentName(), this->name());
                               return false;
                           }
                           #else
                               return (not CheckEngineError(status));
                           #endif
                       }
                       return true;
                   };

        bool const bChildTraversalSuccessful
            = mComponentHierarchy.foreachEdgeFromRoot(
                iterator
                , aComponent->getComponentId()
                , aOrder
                , /* aAbortOnFirstError */ false);

        EEngineStatus const status = aForeachChildFn(aComponent);
        bool const bRootHandlingSuccessful = not CheckEngineError(status);
        #if SHIRABE_DEBUG
        if(not bRootHandlingSuccessful)
        {
            CLog::Error(logTag(), TEXT("Failed to process component '%s' of entity '%s'"), mRootComponent->getComponentName(), this->name());
        }
        #endif

        return (bChildTraversalSuccessful && bRootHandlingSuccessful);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CEntity::initialize()
    {
        if(mInitialized)
        {
            return EEngineStatus::Ok;
        }

        mUid = CEntity::getUID();

        if(mRootComponent)
        {
            auto const handlerFn
               = [] (Shared<IComponent> &c) -> EEngineStatus
                    {
                        return (c ? c->initialize() : EEngineStatus::Error);
                    };
            bool const bTraversalSuccessful = foreachChildOf(mRootComponent, handlerFn);
            if(not bTraversalSuccessful)
            {
                mStateIsValid = false;
            }
        }

        mInitialized = true;
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CEntity::deinitialize()
    {
        if(mRootComponent)
        {
            auto const handlerFn
                = [] (Shared<IComponent> &c) -> EEngineStatus
                    {
                        return (c ? c->deinitialize() : EEngineStatus::Error);
                    };
            bool const bTraversalSuccessful = foreachChildOf(mRootComponent, handlerFn);
            if(not bTraversalSuccessful)
            {
                mStateIsValid = false;
            }
        }
        mComponentHierarchy.reset();
        mAssignedComponents.clear();

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool CEntity::addComponent(PublicComponentId_t         aParentComponentId
                               , Shared<IComponent> const& aComponent)
    {
        if (   gInvalidComponentId == aParentComponentId
            || not aComponent
            || not containsComponent(aComponent->getComponentId()))
        {
            return false;
        }

        PublicComponentId_t const childComponentId = aComponent->getComponentId();
        mAssignedComponents.insert({childComponentId, aComponent});
        mComponentHierarchy.add(childComponentId);
        mComponentHierarchy.connect(aParentComponentId, childComponentId);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool CEntity::removeComponent(PublicComponentId_t aComponentId, bool bReattachChildren /* = true */)
    {
        if(gInvalidComponentId == aComponentId || not containsComponent(aComponentId))
        {
            return false;
        }

        auto &component = mAssignedComponents.at(aComponentId);

        // TODO: Handle non-leaf removals, i.e. by reattaching all children to the previous parent.
        if(bReattachChildren)
        {
            auto const parents  = mComponentHierarchy.getParents(aComponentId);
            auto const children = mComponentHierarchy.getChildren(aComponentId);
            for(auto &parentId : parents)
            {
                for(auto &childId : children)
                {
                    mComponentHierarchy.connect(parentId, childId);
                }
            }
        }
        else
        {
            auto const handlerFn
                = [this] (Shared<IComponent> &child) -> EEngineStatus
                    {
                        if(child)
                        {
                            EEngineStatus const status = child->deinitialize();
                            mComponentHierarchy.remove(child->getComponentId());
                            mAssignedComponents.erase(child->getComponentId());
                            return status;
                        }
                    };
            bool const bTraversalSuccessful = foreachChildOf(component, handlerFn);
            if(not bTraversalSuccessful)
            {
                mStateIsValid = false;
            }
        }

        mComponentHierarchy.remove(aComponentId);
        mAssignedComponents.erase(aComponentId);
        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool CEntity::containsComponent(PublicComponentId_t aComponentId)
    {
        return (mAssignedComponents.end() != mAssignedComponents.find(aComponentId));
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CEntity::update(CTimer const &aTimer)
    {
        if(mRootComponent)
        {
            auto const handlerFn
               = [&, this] (Shared<IComponent> &child) -> EEngineStatus
                    {
                        if(child)
                        {
                            return child->update(aTimer);
                        }
                    };

            bool const bTraversalSuccessful = foreachChildOf(mRootComponent, handlerFn, CAdjacencyTree<PublicComponentId_t>::EOrder::RootFirst);
            if(not bTraversalSuccessful)
            {
                mStateIsValid = false;
            }
        }
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}
