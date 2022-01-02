//
// Created by dotti on 23.10.19.
//

#ifndef __SHIRABE_ADJACENCYTREE_H__
#define __SHIRABE_ADJACENCYTREE_H__

// Example program
#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <core/basictypes.h>

namespace engine::datastructures
{
    template<typename K, typename V>
    using Map_t = std::unordered_map<K, V>;

    template<typename V>
    using List_t = std::vector<V>;

    template<typename T>
    using Tree_t = Map_t<T, List_t<T>>;

    using Id_t = uint32_t;

    using PrintFn_t = std::function<void(  Tree_t<uint32_t> const &
                                         , List_t<uint32_t> const &
                                         , uint32_t         const)>;

    using ReverseTreeDerivationFn_t = std::function<void(  Tree_t<uint32_t> const &
                                                         , List_t<uint32_t> const &
                                                         , uint32_t         const &
                                                         , Tree_t<uint32_t>       &
                                                         , List_t<uint32_t>       &)>;

    using DeterminationFn_t = std::function<void(Tree_t<uint32_t>   const &
                                                 , uint32_t         const &
                                                 , List_t<uint32_t> const &
                                                 , List_t<uint32_t>       &
                                                 , bool)>;


    using TraverseTreeCallbackFn_t = std::function<void(List_t<uint32_t> const &)>;
    using TraverseTreeFn_t         = std::function<void(  Tree_t<uint32_t>         const & /* tree */
                                                        , uint32_t                 const & /* root */
                                                        , List_t<uint32_t>               & /* random access stack */
                                                        , TraverseTreeCallbackFn_t const & /* callback */)>;

    class SHIRABE_LIBRARY_EXPORT CAdjacencyTreeHelper
    {
    public_static_functions:
        template<typename K, typename V>
        static bool mapContainsElementFn(  Map_t<K, V> const &aTree
                                         , K           const &aElement)
        {
            return (aTree.end()!=aTree.find(aElement));
        }

        template<typename T>
        static bool treeContainsElementFn(  Tree_t<T> const &aTree
                                          , T         const &aElement)
        {
            return (aTree.end()!=aTree.find(aElement));
        }

        template<typename T>
        static bool listContainsElementFn(  List_t<T> const &aList
                                          , T         const &aElement)
        {
            return (aList.end()!=std::find(aList.begin(), aList.end(), aElement));
        }

        template<typename T>
        static bool insertTreeEntryIfNotAddedFn(  Tree_t<T>       &aInOutTree
                                                , T         const &aId)
        {
            if( not treeContainsElementFn(aInOutTree, aId))
            {
                aInOutTree.insert({aId, {}});
                return true;
            }

            return false;
        }

        template <typename T>
        static bool removeTreeEntryIfAdded(  Tree_t<T>       &aInOutTree
                                           , T         const &aId)
        {
            if( not treeContainsElementFn(aInOutTree, aId))
            {
                return false;
            }

            for(auto &[id, references] : aInOutTree)
            {
                removeListEntryIfAdded(references, aId);
            }

            aInOutTree.erase(aId);
            return true;
        }

        template<typename T>
        static void insertListEntryIfNotAddedFn(  List_t<T>       &aInOutList
                                                , T         const &aId)
        {
            if( not listContainsElementFn(aInOutList, aId))
            {
                aInOutList.insert(aInOutList.end(), aId);
            }
        }

        template <typename T>
        static bool removeListEntryIfAdded(  List_t<T>       &aInOutList
                                           , T         const &aId)
        {
            if( not listContainsElementFn(aInOutList, aId))
            {
                return false;
            }

            aInOutList.erase(std::find(aInOutList.begin(), aInOutList.end(), aId));
            return true;
        }

        static auto printTree(  Tree_t<uint32_t> const &aTree
                              , List_t<uint32_t> const &aRoots
                              , uint32_t         const  aLevel)->void
        {
            for( auto const &root : aRoots )
            {
                if( not treeContainsElementFn(aTree, root))
                {
                    continue;
                }

                std::cout << std::setw(5*aLevel) << root << "\n";

                List_t<uint32_t> const &subroots=aTree.at(root);

                printTree(aTree, subroots, (aLevel+1));
            }
        };

        static auto printList(List_t<uint32_t> const &aList)
        {
            for( auto const &v : aList )
            {
                std::cout << v << ", ";
            }
            std::cout << "\n";
        };

        template <typename T>
        static auto deriveReverseTree(  Tree_t<T> const &aForwardTree
                                      , List_t<T> const &aForwardRoots
                                      , T         const &aParentUid
                                      , Tree_t<T>       &aInOutReverseTree
                                      , List_t<T>       &aInOutReverseRoots) -> void
        {
            for( auto const &root : aForwardRoots )
            {
                insertTreeEntryIfNotAddedFn(aInOutReverseTree, root);

                List_t<T> &reverseChildren = aInOutReverseTree.at(root);
                insertListEntryIfNotAddedFn(reverseChildren, aParentUid);

                List_t<T> const &children=aForwardTree.at(root);
                if( not children.empty())
                {
                    deriveReverseTree(aForwardTree, children, root, aInOutReverseTree, aInOutReverseRoots);
                }
                else
                {
                    insertListEntryIfNotAddedFn(aInOutReverseRoots, root);
                }
            }
        }
    };

    enum class EDisconnectAllType
    {
        Sources,
        Targets,
        All
    };

    template <typename TIdType>
    class SHIRABE_LIBRARY_EXPORT CAdjacencyTree
    {
    public_enums:
        enum class EOrder
                : int8_t
        {
            RootFirst   = 1
            , LeafFirst = 2
        };

    public_api:
        void reset();

        bool add       (TIdType const &aId);
        bool remove    (TIdType const &aId);
        bool connect   (TIdType const &aSource, TIdType const &aTarget);
        bool disconnect(TIdType const &aSource, TIdType const &aTarget);
        bool disconnectMany(TIdType const &aId, EDisconnectAllType const aType = EDisconnectAllType::All);

        List_t<TIdType> getParents(TIdType aElementId) const;
        List_t<TIdType> getChildren(TIdType aElementId) const;
        List_t<TIdType> getAdjacentFor(TIdType const &aId) const;

        std::vector<TIdType> topologicalSort();

        bool foreachEdgeFromRoot(std::function<bool(TIdType const &aSource, TIdType const &aTarget)> aCallback, TIdType aRoot, EOrder const aOrder = EOrder::RootFirst, bool const aAbortOnFirstError = true)
        {
            bool successful = true;

            List_t<Edge_t> const edges = getEdges(aRoot, aOrder);
            for(Edge_t const &edge : edges)
            {
                successful &= aCallback(edge.source, edge.target);
                if(aAbortOnFirstError && not successful)
                {
                    break;
                }
            }

            return successful;
        }

    private_structs:
        struct Edge_t
        {
            TIdType source;
            TIdType target;

            static bool isContainedInList(  List_t<Edge_t> const &aEdgesToCheck
                                          , TIdType        const &aSource
                                          , TIdType        const &aTarget)
            {
                bool contained = false;

                for(Edge_t const &edge : aEdgesToCheck)
                {
                    contained |= (edge.source == aSource && edge.target == aTarget);
                }

                return contained;
            }
        };

    private_api:
        List_t<Edge_t> const getEdges(TIdType const &aRoot, EOrder const aOrder = EOrder::RootFirst) const
        {
            return getEdgesImpl({ aRoot }, aOrder);
        };

        List_t<Edge_t> const getAllEdges(EOrder const aOrder = EOrder::RootFirst) const
        {
            return getEdgesImpl(mForwardRoots, aOrder);
        }

        List_t<Edge_t> const getEdgesImpl(List_t<TIdType> const &aRoots, EOrder const aOrder = EOrder::RootFirst) const
        {
            std::vector<Edge_t> edges {};

            std::function<void(TIdType const&)> traverseForwardFn = nullptr;
            traverseForwardFn = [&, this] (TIdType const &aId)
            {
                List_t<TIdType> const children = mForwardTree.at(aId);
                if(0 < children.size())
                {
                    for( auto const &child : children )
                    {
                        if(Edge_t::isContainedInList(edges, aId, child))
                        {
                            continue;
                        }

                        if(EOrder::RootFirst == aOrder)
                        {
                            edges.push_back({aId, child}); // Ordering: From child to parent.
                        }

                        traverseForwardFn(child);

                        if(EOrder::LeafFirst == aOrder)
                        {
                            edges.push_back({aId, child}); // Ordering: From child to parent.
                        }
                    }
                }
            };

            for(auto const &root : aRoots)
            {
                traverseForwardFn(root);
            }

            return edges;
        };

        List_t<Edge_t> const getPathToRoot(TIdType const &aId)
        {
            if(mReverseTree.end() == mReverseTree.find(aId))
            {
                return {}; // source not contained;
            }

            std::vector<Edge_t> edges {};

            std::function<void(TIdType const&)> const getParentEdgeFn = nullptr;
            getParentEdgeFn = [&, this] (TIdType const &aCurrentChild)
            {
                List_t<TIdType> const parents = mReverseTree.at(aCurrentChild);
                if(0 < parents.size())
                {
                    for( auto const &parent : parents )
                    {
                        if(Edge_t::isContainedInList(edges, aCurrentChild, parent))
                        {
                            continue;
                        }

                        edges.emplace_back({aCurrentChild, parent}); // Ordering: From child to parent.
                        getParentEdgeFn(parent);
                    }
                }
            };
            getParentEdgeFn(aId);

            return edges;
        }

        void regenerateTree();

    private_members:
        Tree_t<TIdType> mForwardTree;
        List_t<TIdType> mForwardRoots;
        Tree_t<TIdType> mReverseTree;
        List_t<TIdType> mReverseRoots;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TIdType>
    void CAdjacencyTree<TIdType>::reset()
    {
        mForwardTree.clear();
        mReverseTree.clear();
        mForwardRoots.clear();
        mReverseRoots.clear();
    }
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TIdType>
    bool CAdjacencyTree<TIdType>::add(TIdType const &aId)
    {
        CAdjacencyTreeHelper::insertTreeEntryIfNotAddedFn(mForwardTree, aId);
        CAdjacencyTreeHelper::insertListEntryIfNotAddedFn(mForwardRoots, aId);
        regenerateTree();
        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TIdType>
    bool CAdjacencyTree<TIdType>::remove(TIdType const &aId)
    {
        CAdjacencyTreeHelper::removeTreeEntryIfAdded(mForwardTree, aId);
        regenerateTree();
        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TIdType>
    bool CAdjacencyTree<TIdType>::connect(TIdType const &aSource, TIdType const &aTarget)
    {
        if(not CAdjacencyTreeHelper::treeContainsElementFn(mForwardTree, aSource))
        {
            return false;
        }

        if(CAdjacencyTreeHelper::listContainsElementFn(mForwardTree[aSource], aTarget))
        {
            return false;
        }

        CAdjacencyTreeHelper::insertListEntryIfNotAddedFn(mForwardTree[aSource], aTarget);
        CAdjacencyTreeHelper::removeListEntryIfAdded(mForwardRoots, aSource);
        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TIdType>
    bool CAdjacencyTree<TIdType>::disconnect(TIdType const &aSource, TIdType const &aTarget)
    {
        if(not CAdjacencyTreeHelper::treeContainsElementFn(mForwardTree, aSource))
        {
            return false;
        }

        if(not CAdjacencyTreeHelper::listContainsElementFn(mForwardTree[aSource], aTarget))
        {
            return false;
        }

        CAdjacencyTreeHelper::removeListEntryIfAdded(mForwardTree[aSource], aTarget);
        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TIdType>
    bool CAdjacencyTree<TIdType>::disconnectMany(TIdType            const &aSource,
                                                 EDisconnectAllType        aType)
    {
        if(not CAdjacencyTreeHelper::treeContainsElementFn(mForwardTree, aSource))
        {
            return false;
        }

        if(EDisconnectAllType::All == aType || EDisconnectAllType::Sources == aType)
        {
            for(auto &[id, list] : mForwardTree)
            {
                CAdjacencyTreeHelper::removeListEntryIfAdded(list, aSource);
            }
        }

        if(EDisconnectAllType::All == aType || EDisconnectAllType::Targets == aType)
        {
            List_t<TIdType> &children = mForwardTree[aSource];
            children.clear();
        }

        return true;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TIdType>
    List_t<TIdType> CAdjacencyTree<TIdType>::getParents(TIdType aElementId) const
    {
        if(not CAdjacencyTreeHelper::treeContainsElementFn(mReverseTree, aElementId))
        {
            return {};
        }
        return mReverseTree.at(aElementId);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TIdType>
    List_t<TIdType> CAdjacencyTree<TIdType>::getChildren(TIdType aElementId) const
    {
        return getAdjacentFor(aElementId);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TIdType>
    List_t<TIdType> CAdjacencyTree<TIdType>::getAdjacentFor(TIdType const &aId) const
    {
        if(not CAdjacencyTreeHelper::treeContainsElementFn(mForwardTree, aId))
        {
            return {};
        }

        return mForwardTree.at(aId);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TIdType>
    void CAdjacencyTree<TIdType>::regenerateTree()
    {
        mForwardRoots.clear();
        mReverseRoots.clear();
        mReverseTree .clear();

        // Copy all known IDs to the forward root list
        // for(auto const &[id, references] : mForwardTree)
        // {
        //     mForwardRoots.push_back(id);
        // }

        // Erase every ID, which is referenced (can't be root).
        // for(auto &[id, references] : mForwardTree)
        // {
        //     for(auto const &aReferenceId : references)
        //     {
        //         std::remove_if(mForwardRoots.begin(), mForwardRoots.end(), [aReferenceId] (TIdType const &aId) -> bool { return aId == aReferenceId; });
        //     }
        // }

        CAdjacencyTreeHelper::deriveReverseTree(mForwardTree, mForwardRoots, TIdType{}, mReverseTree, mReverseRoots);
    }

    template <typename TIdType>
    std::vector<TIdType> CAdjacencyTree<TIdType>::topologicalSort()
    {
        std::vector<TIdType> ordered;

        std::function<void(Tree_t<TIdType> const &,
                           TIdType const &,
                           std::unordered_map<TIdType, bool> &,
                           std::vector<TIdType> &)> DSFi_fn;

        // Define the recursive sort function
        DSFi_fn =
            [&](Tree_t<TIdType> const             &aEdges,
                TIdType const                     &aVertex,
                std::unordered_map<TIdType, bool> &aVisitedEdges,
                std::vector<TIdType>              &aPassOrder) -> void
                {
                    bool const edgeVisited = aVisitedEdges[aVertex];
                    if(edgeVisited)
                    {
                        return;
                    }

                    aVisitedEdges[aVertex] = true;

                    // For each outgoing edge...
                    bool const doesContainEdge = (aEdges.end() != aEdges.find(aVertex));
                    if(doesContainEdge)
                    {
                        for(TIdType const &adjacent : aEdges.at(aVertex))
                        {
                            DSFi_fn(aEdges, adjacent, aVisitedEdges, aPassOrder);
                        }
                    }

                    aPassOrder.push_back(aVertex);
                };

        // Kick-off the sort algorithm
        try
        {
            std::unordered_map<TIdType, bool> visitedEdges = {};
            for(typename Tree_t<TIdType>::value_type &passAdjacency : mForwardTree)
            {
                visitedEdges[passAdjacency.first] = false;
            }

            for(typename Tree_t<TIdType>::value_type &passAdjacency : mForwardTree)
            {
                DSFi_fn(mForwardTree, passAdjacency.first, visitedEdges, ordered);
            }
        }
        catch(std::runtime_error const &aRTE)
        {
            // CLog::Error(logTag(), StaticStringHelpers::format("Failed to perform topological sort: {} ", aRTE.what()));
        }
        catch(...)
        {
            // CLog::Error(logTag(), "Failed to perform topological sort. Unknown error.");
        }

        // Reverse order, as the first item is at the end w/ this algorithm.
        std::reverse(ordered.begin(), ordered.end());

        return ordered;
    }
    //<-----------------------------------------------------------------------------
}

#endif //__SHIRABEDEVELOPMENT_ADJACENCYTREE_H__
