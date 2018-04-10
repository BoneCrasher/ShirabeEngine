#ifndef __SHIRABE_SERIALIZATION_H__
#define __SHIRABE_SERIALIZATION_H__

#include <string>

#include "Core/EngineTypeHelper.h"

namespace Engine {
  namespace Serialization {

    class ISerializationResult {
    public:
      virtual ~ISerializationResult() = default;

      ISerializationResult(ISerializationResult const&)            = delete;
      ISerializationResult(ISerializationResult &&)                = delete;
      ISerializationResult& operator=(ISerializationResult const&) = delete;
      ISerializationResult& operator=(ISerializationResult &&)     = delete;

      virtual std::string serializeResultToString() = 0;

    protected:
      ISerializationResult() = default;
    };

    template <
      typename TInterfaceSerialization,
      typename TInterfaceDeserialization>
    DeclareTemplatedInterface(ISerializable, Template(ISerializable<TInterfaceSerialization, TInterfaceDeserialization>));

        virtual void acceptSerializer(Ptr<TInterfaceSerialization>&)           = 0;
        virtual void acceptDeserializer(Ptr<TInterfaceDeserialization> const&) = 0;

    DeclareInterfaceEnd(ISerializable);

  }
}

#endif