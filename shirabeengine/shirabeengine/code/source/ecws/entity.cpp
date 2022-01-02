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
    EEngineStatus CEntity::initialize()
    {
        if(mInitialized)
        {
            return EEngineStatus::Ok;
        }

        mUid = CEntity::getUID();

        if(mRootComponent)
        {
            auto const initializeComponentTree =
               [this] (PublicComponentId_t const &aParentId
                       , PublicComponentId_t const &aChildId) -> bool
                   {
                       SHIRABE_UNUSED(aParentId);
                       auto &currentChild = mAssignedComponents.at(aChildId);
                       if(nullptr != currentChild)
                       {
                           EEngineStatus const status = currentChild->initialize();
                           if(CheckEngineError(status))
                           {
                               CLog::Error(logTag(), TEXT("Failed to initialize component '%s' of entity '%s'"), currentChild->getComponentName(), this->name());
                               return false;
                           }
                       }
                       return true;
                   };

            mComponentHierarchy.foreachEdgeFromRoot(
                initializeComponentTree
                , mRootComponent->getComponentId()
                , CAdjacencyTree<PublicComponentId_t>::EOrder::LeafFirst);

            EEngineStatus const status = mRootComponent->initialize();
            if(CheckEngineError(status))
            {
                CLog::Error(logTag(), TEXT("Failed to initialize component '%s' of entity '%s'"), mRootComponent->getComponentName(), this->name());
                return EEngineStatus::Error;
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
            auto const deinitializeComponentTree =
                [this] (PublicComponentId_t const &aParentId
                      , PublicComponentId_t const &aChildId) -> bool
                    {
                        SHIRABE_UNUSED(aParentId);
                        auto &currentChild = mAssignedComponents.at(aChildId);
                        if(nullptr != currentChild)
                        {
                            EEngineStatus const status = currentChild->deinitialize();
                            if(CheckEngineError(status))
                            {
                                CLog::Error(logTag(), TEXT("Failed to deinitialize component '%s' of entity '%s'"), currentChild->getComponentName(), this->name());
                                return false;
                            }
                        }
                        return true;
                    };

            mComponentHierarchy.foreachEdgeFromRoot(
                deinitializeComponentTree
                , mRootComponent->getComponentId()
                , CAdjacencyTree<PublicComponentId_t>::EOrder::LeafFirst);

            EEngineStatus const status = mRootComponent->deinitialize();
            if(CheckEngineError(status))
            {
                CLog::Error(logTag(), TEXT("Failed to deinitialize component '%s' of entity '%s'"), mRootComponent->getComponentName(), this->name());
                return EEngineStatus::Error;
            }
            mComponentHierarchy.reset();
            mAssignedComponents.clear();
        }
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
            auto const children = mComponentHierarchy.getChildren(aComponentId);
            asdkj;asfdkl
            Use tree traversal functions to delete all chilren leaf first!
            Add functions like deinitialize/initialize/update children for reuse!
        }

        mComponentHierarchy.remove(aComponentId);
        mAssignedComponents.erase(aComponentId);
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
            auto const updateComponentTree =
               [&, this] (PublicComponentId_t   const &aParentId
                          , PublicComponentId_t const &aChildId) -> bool
                   {
                       SHIRABE_UNUSED(aParentId);
                       auto &currentChild = mAssignedComponents.at(aChildId);
                       if(nullptr != currentChild)
                       {
                           EEngineStatus const status = currentChild->update(aTimer);
                           if(CheckEngineError(status))
                           {
                               CLog::Error(logTag(), TEXT("Failed to update component '%s' of entity '%s'"), currentChild->getComponentName(), this->name());
                               return false;
                           }
                       }
                       return true;
                   };

            // TODO: Trace then Bubble? What about cases, where we need to propagate something
            //       down first and then handle reactions to it?
            mComponentHierarchy.foreachEdgeFromRoot(
                updateComponentTree
                , mRootComponent->getComponentId()
                , CAdjacencyTree<PublicComponentId_t>::EOrder::RootFirst);

            EEngineStatus const status = mRootComponent->update(aTimer);
            if(CheckEngineError(status))
            {
                CLog::Error(logTag(), TEXT("Failed to update component '%s' of entity '%s'"), mRootComponent->getComponentName(), this->name());
                return EEngineStatus::Error;
            }
        }
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}
