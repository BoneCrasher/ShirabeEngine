#ifndef __SHIRABE_SERIALIZATION_JSONSERIALIZER_H__
#define __SHIRABE_SERIALIZATION_JSONSERIALIZER_H__

#include "Core/EngineTypeHelper.h"

#include "Serialization/Serialization.h"

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