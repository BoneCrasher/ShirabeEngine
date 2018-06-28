#ifndef __SHIRABE_SERIALIZATION_GRAPHVIZ_DOT_SERIALIZER_H__
#define __SHIRABE_SERIALIZATION_GRAPHVIZ_DOT_SERIALIZER_H__

#include "Core/EngineTypeHelper.h"

#include "Core/Serialization/Serialization.h"

namespace Engine {
  namespace Serialization {

    struct GraphVizDotOutputTag_t {};

    template <
      typename IInterfaceSerializer,
      typename IInterfaceDeserializer>
      class Serializer<GraphVizDotOutputTag_t, IInterfaceSerializer, IInterfaceDeserializer>
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