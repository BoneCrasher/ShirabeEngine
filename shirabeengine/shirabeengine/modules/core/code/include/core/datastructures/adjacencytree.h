//
// Created by dotti on 23.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_ADJACENCYTREE_H__
#define __SHIRABEDEVELOPMENT_ADJACENCYTREE_H__

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
        static void insertTreeEntryIfNotAddedFn(  Tree_t<T>       &aInOutTree
                                                , T         const &aId)
        {
            if( not treeContainsElementFn(aInOutTree, aId))
            {
                aInOutTree.insert({aId, {}});
            }
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

        static auto deriveReverseTree(  Tree_t<uint32_t> const &aForwardTree
                                      , List_t<uint32_t> const &aForwardRoots
                                      , uint32_t         const &aParentUid
                                      , Tree_t<uint32_t>       &aInOutReverseTree
                                      , List_t<uint32_t>       &aInOutReverseRoots) -> void
        {
            for( auto const &root : aForwardRoots )
            {
                insertTreeEntryIfNotAddedFn(aInOutReverseTree, root);

                if( 0<aParentUid )
                {
                    List_t<uint32_t> &reverseChildren = aInOutReverseTree.at(root);
                    insertListEntryIfNotAddedFn(reverseChildren, aParentUid);
                }

                List_t<uint32_t> const &children=aForwardTree.at(root);
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

    template <typename TIdType>
    class SHIRABE_LIBRARY_EXPORT CAdjacencyTree
    {
    public_api:
        bool add       (TIdType const &aId);
        bool remove    (TIdType const &aId);
        bool connect   (TIdType const &aSource, TIdType const &aTarget);
        bool disconnect(TIdType const &aSource, TIdType const &aTarget);

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
        List_t<Edge_t> const getAllEdges() const
        {
            std::vector<Edge_t> edges {};

            std::function<void(TIdType const&)> traverseForwardFn = null;
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

                        edges.emplace_back({ aId, child }); // Ordering: From child to parent.
                        getParentEdgeFn(child);
                    }
                }
            };

            for(auto const &root : mForwardRoots)
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

            std::function<void(TIdType const&)> const getParentEdgeFn = null;
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

    private_members:
        Tree_t<TIdType> mForwardTree;
        List_t<TIdType> mForwardRoots;
        Tree_t<TIdType> mReverseTree;
        List_t<TIdType> mReverseRoots;
    };
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool add       (TIdType const &aId);
    bool remove    (TIdType const &aId);
    bool connect   (TIdType const &aSource, TIdType const &aTarget);
    bool disconnect(TIdType const &aSource, TIdType const &aTarget);
    //<-----------------------------------------------------------------------------
}

#endif //__SHIRABEDEVELOPMENT_ADJACENCYTREE_H__
