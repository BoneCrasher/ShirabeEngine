#ifndef __SHIRABE_SERIALIZATION_GRAPHVIZ_DOT_SERIALIZER_H__
#define __SHIRABE_SERIALIZATION_GRAPHVIZ_DOT_SERIALIZER_H__

#include "core/enginetypehelper.h"

#include "core/serialization/serialization.h"

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