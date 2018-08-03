#ifndef __SHIRABE_RESOURCES_DATASOURCE_H__
#define __SHIRABE_RESOURCES_DATASOURCE_H__

#include <memory>

#include "Core/DataBuffer.h"

namespace engine {
  namespace Resources {

    /**
    * \class ResourceDataSource
    *
    * \brief A resource data source.
    **************************************************************************************************/
    class ResourceDataSource {
    public:
      using ResourceAccessFn_t = std::function<ByteBuffer()>;

      inline 
        ResourceDataSource()
        : m_accessorFn(nullptr)
      {}
      inline
        ResourceDataSource(ResourceAccessFn_t const&fn)
        : m_accessorFn(fn)
      {}

      inline
        ByteBuffer getData() {
        if(m_accessorFn)
          return m_accessorFn();
        else
          return ByteBuffer{ };
      }

    private:
      ResourceAccessFn_t m_accessorFn;
    };

  }
}

#endif