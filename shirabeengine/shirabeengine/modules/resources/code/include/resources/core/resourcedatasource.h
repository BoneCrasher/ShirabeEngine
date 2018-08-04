#ifndef __SHIRABE_RESOURCES_DATASOURCE_H__
#define __SHIRABE_RESOURCES_DATASOURCE_H__

#include <memory>

#include "Core/DataBuffer.h"

namespace engine {
  namespace resources {

    /**
    * \class #include "resources/core/resourcedatasource.h"
    *
    * \brief A resource data source.
    **************************************************************************************************/
    class #include "resources/core/resourcedatasource.h" {
    public:
      using ResourceAccessFn_t = std::function<ByteBuffer()>;

      inline 
        #include "resources/core/resourcedatasource.h"()
        : m_accessorFn(nullptr)
      {}
      inline
        #include "resources/core/resourcedatasource.h"(ResourceAccessFn_t const&fn)
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