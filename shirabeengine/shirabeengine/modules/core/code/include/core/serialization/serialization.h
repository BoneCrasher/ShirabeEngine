#ifndef __SHIRABE_SERIALIZATION_H__
#define __SHIRABE_SERIALIZATION_H__

#include <string>

#include "Core/EngineTypeHelper.h"

namespace engine {
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
      typename IInterfaceSerialization,
      typename IInterfaceDeserialization
    >
      DeclareTemplatedInterface(ISerializable, Template(ISerializable<IInterfaceSerialization, IInterfaceDeserialization>));

    virtual void acceptSerializer(CStdSharedPtr_t<IInterfaceSerialization>)            = 0;
    virtual void acceptDeserializer(CStdSharedPtr_t<IInterfaceDeserialization> const&) = 0;

    DeclareInterfaceEnd(ISerializable);

    template <
      typename OutputTag_t,
      typename IInterfaceSerialization,
      typename IInterfaceDeserialization
    >
      class Serializer
    {
    public:
      virtual bool writeToFile(std::string const&) = 0;
    };

  }
}

#endif