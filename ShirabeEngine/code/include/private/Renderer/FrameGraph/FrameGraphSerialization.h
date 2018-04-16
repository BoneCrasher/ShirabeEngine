#ifndef __SR_SHIRABE_FRAMEGRAPH_SERIALIZATION_H__
#define __SR_SHIRABE_FRAMEGRAPH_SERIALIZATION_H__

#include "Core/EngineTypeHelper.h"

#include "Serialization/JSONObjectSerializer.h"

namespace Engine {
  // Forward declarations in proper namespaces
  namespace FrameGraph {
    class PassBase;
  }

  namespace Serialization {
    using namespace FrameGraph;

    /**********************************************************************************************//**
     * \class IObjectSerializer
     *
     * \brief A value tree serializer.
     **************************************************************************************************/
    DeclareInterface(IFrameGraphSerializer);

      virtual bool serializePass(Ptr<PassBase> const&) = 0;
    
    DeclareInterfaceEnd(IFrameGraphSerializer);

    /**********************************************************************************************//**
     * \class IObjectDeserializer
     *
     * \brief A value tree deserializer.
     **************************************************************************************************/
    DeclareInterface(IFrameGraphDeserializer);

      virtual bool deserializePass(Ptr<PassBase> &) = 0;

    DeclareInterfaceEnd(IFrameGraphSerializer);

    /**********************************************************************************************//**
     * \class FrameGraphJSONSerializer
     *
     * \brief A frame graph JSON serializer.
     **************************************************************************************************/
    class FrameGraphJSONSerializer
      : public JSONObjectSerializer<IFrameGraphSerializer, IFrameGraphDeserializer>
    {
      bool serializePass(Ptr<PassBase> const&);

      bool deserializePass(Ptr<PassBase> &);
    };
  }
}

#endif