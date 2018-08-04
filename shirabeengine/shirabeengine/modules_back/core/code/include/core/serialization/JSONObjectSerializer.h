#ifndef __SHIRABE_SERIALIZATION_JSONSERIALIZER_H__
#define __SHIRABE_SERIALIZATION_JSONSERIALIZER_H__

#include "core/enginetypehelper.h"

#include "core/serialization/serialization.h"

namespace Engine {
  namespace Serialization {

    struct JSONOutputTag_t {};

    template <
      typename IInterfaceSerializer,
      typename IInterfaceDeserializer>
      class Serializer<JSONOutputTag_t, IInterfaceSerializer, IInterfaceDeserializer>
      : public IInterfaceSerializer
      , public IInterfaceDeserializer
      , public ISerializationResult
    {
    public:
    private:
    };

  }
}

#endif