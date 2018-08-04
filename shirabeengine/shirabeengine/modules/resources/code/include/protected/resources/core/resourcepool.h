#ifndef __SHIRABE_IRESOURCEPOOL_H__
#define __SHIRABE_IRESOURCEPOOL_H__

#include <map>

#include "core/enginetypehelper.h"

namespace engine {
  namespace Resources {

    template <typename TIndex, typename TValue>
    class IndexedResourcePool {
    public:
      typedef TIndex index_type;
      typedef TValue value_type;

      typedef std::map < index_type, value_type >            internal_storage_type;
      typedef typename internal_storage_type::const_iterator const_iterator;
      typedef typename internal_storage_type::iterator       iterator;

      bool addResource(const TIndex& index, const TValue& resource);
      bool removeResource(const TIndex& index);

      bool          hasResource(const TIndex& index) const;
      TValue const& getResource(const TIndex& index) const;
      TValue&       getResource(const TIndex& index);

      const_iterator begin() const { return m_resources.begin(); }
      iterator       begin() { return m_resources.begin(); }

      const_iterator end() const { return m_resources.end(); }
      const_iterator end() { return m_resources.end(); }

    private:
      Map<TIndex, TValue> m_resources;
    };

    template <typename TIndex, typename TValue>
    bool
      IndexedResourcePool<TIndex, TValue>::addResource(
        TIndex const&index,
        TValue const&resource)
    {
      if(hasResource(index))
        return false;

      m_resources[index] = resource;
      return true;
    }

    template <typename TIndex, typename TValue>
    bool
      IndexedResourcePool<TIndex, TValue>::removeResource(
        TIndex const&index)
    {
      if(!hasResource(index))
        return false;

      m_resources.erase(m_resources.find(index));
      return true;
    }

    template <typename TIndex, typename TValue>
    bool
      IndexedResourcePool<TIndex, TValue>::hasResource(
        TIndex const&index) const
    {
      return (m_resources.find(index) != m_resources.end());
    }

    template <typename TIndex, typename TValue>
    TValue const&
      IndexedResourcePool<TIndex, TValue>::getResource(
        TIndex const&index) const
    {
      static TValue gDefault = TValue();
      if(!hasResource(index))
        return gDefault;

      return m_resources.at(index);
    }

    template <typename TIndex, typename TValue>
    TValue&
      IndexedResourcePool<TIndex, TValue>::getResource(
        TIndex const&index)
    {
      static TValue gDefault = TValue();
      if(!hasResource(index))
        return gDefault;

      return m_resources.at(index);
    }

  }
}

#endif