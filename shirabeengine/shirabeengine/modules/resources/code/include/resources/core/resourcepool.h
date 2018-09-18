#ifndef __SHIRABE_IRESOURCEPOOL_H__
#define __SHIRABE_IRESOURCEPOOL_H__

#include <map>

#include "core/enginetypehelper.h"

namespace engine
{
    namespace resources
    {

        /**
         * Storage container for arbitrarily typed resources.
         * Basically a map...
         *
         * @tparam TIndex Index-type of the stored assignment.
         * @tparam TValue Value-type of the resources to store.
         */
        template<
                typename TIndex,
                typename TValue
                >
        class IndexedResourcePool
        {
        public_typedefs:
            using index_type            = TIndex;
            using value_type            = TValue;
            using internal_storage_type = std::map < index_type, value_type >           ;
            using const_iterator        = typename internal_storage_type::const_iterator;
            using iterator              = typename internal_storage_type::iterator      ;

        public_members:
            /**
             * Store a resource in this pool.
             *
             * @param aIndex    The index by which to address the resource.
             * @param aResource The resource to be stored.
             * @return          True, if successful. False, otherwise.
             */
            bool addResource(TIndex const &aIndex, TValue const &aResource);
            /**
             * Remove a resource from this pool.
             *
             * @param aIndex The index by which to address the resource.
             * @return       True, if successful. False otherwise.
             */
            bool removeResource(TIndex const &aIndex);

            /**
             * Check, whether a resource identified by aIndex is registered.
             *
             * @param aIndex The index by which to address the resource.
             * @return       True, if successful. False otherwise.
             */
            bool hasResource(TIndex const &aIndex) const;

            /**
             * Access the resource identified by aIndex immutably.
             *
             * @param aIndex The index by which to address the resource.
             * @return       Returns a const reference to the stored resource.
             * @throw        std::runtime_error, if not registered.
             */
            OptionalRef_t<TValue> const getResource(const TIndex& index) const;

            /**
             * Access the resource identified by aIndex.
             *
             * @param aIndex The index by which to address the resource.
             * @return       Returns a reference to the stored resource.
             * @throw        std::runtime_error, if not registered.
             */
            OptionalRef_t<TValue> getResource(const TIndex& index);

            /**
             * Begin iterating over the stored resources.
             *
             * @return Iterator to the stored map's begin-iterator.
             */
            const_iterator begin() const { return mResources.begin(); }
            /**
             * Begin iterating over the stored resources.
             *
             * @return Iterator to the stored map's begin-iterator.
             */
            iterator begin() { return mResources.begin(); }

            /**
             * End iterating over the stored resources.
             *
             * @return Iterator to the stored map's end-iterator.
             */
            const_iterator end() const { return mResources.end(); }
            /**
             * End iterating over the stored resources.
             *
             * @return Iterator to the stored map's end-iterator.
             */
            iterator end() { return mResources.end(); }

        private_members:
            Map<TIndex, TValue> mResources;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TIndex, typename TValue>
        bool IndexedResourcePool<TIndex, TValue>::addResource(
                TIndex const&index,
                TValue const&resource)
        {
            if(hasResource(index))
                return false;

            mResources[index] = resource;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TIndex, typename TValue>
        bool
        IndexedResourcePool<TIndex, TValue>::removeResource(
                TIndex const&index)
        {
            if(!hasResource(index))
                return false;

            mResources.erase(mResources.find(index));
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TIndex, typename TValue>
        bool
        IndexedResourcePool<TIndex, TValue>::hasResource(
                TIndex const&index) const
        {
            return (mResources.find(index) != mResources.end());
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TIndex, typename TValue>
        OptionalRef_t<TValue> const
        IndexedResourcePool<TIndex, TValue>::getResource(
                TIndex const&index) const
        {
            if(!hasResource(index))
                throw std::runtime_error("Resource not found!");

            return mResources.at(index);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TIndex, typename TValue>
        OptionalRef_t<TValue>
        IndexedResourcePool<TIndex, TValue>::getResource(
                TIndex const&index)
        {
            if(!hasResource(index))
                throw std::runtime_error("Resource not found!");

            return mResources.at(index);
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
