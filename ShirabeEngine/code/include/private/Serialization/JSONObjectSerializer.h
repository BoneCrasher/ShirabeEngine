#ifndef __SHIRABE_SERIALIZATION_JSONSERIALIZER_H__
#define __SHIRABE_SERIALIZATION_JSONSERIALIZER_H__

#include "Core/EngineTypeHelper.h"

#include "Serialization/Serialization.h"

namespace Engine {
  namespace Serialization {

    template <
      typename TInterfaceSerializer,
      typename TInterfaceDeserializer>
      class JSONObjectSerializer
      : public TInterfaceSerializer
      , public TInterfaceDeserializer
      , public ISerializationResult
    {
    public:
    private:
    };

  }
}

#endif