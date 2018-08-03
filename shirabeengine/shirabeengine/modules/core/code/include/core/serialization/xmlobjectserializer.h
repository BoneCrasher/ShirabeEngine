#ifndef __SHIRABE_SERIALIZATION_XMLSERIALIZER_H__
#define __SHIRABE_SERIALIZATION_XMLSERIALIZER_H__

#include "Core/Serialization/Serialization.h"

namespace engine {
  namespace Serialization {

    struct XMLOutputTag_t {};

    /**********************************************************************************************//**
     * \class XMLObjectSerializer
     *
     * \brief An XML object serializer implementing a specific CRTP injected serializer specialization.
     *
     * \tparam  TInterfaceSerializer    Type of the interface serializer.
     * \tparam  TInterfaceDeserializer  Type of the interface deserializer.
     **************************************************************************************************/
    template <
      typename IInterfaceSerializer,
      typename IInterfaceDeserializer>
    class Serializer<XMLOutputTag_t, IInterfaceSerializer, IInterfaceDeserializer>
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