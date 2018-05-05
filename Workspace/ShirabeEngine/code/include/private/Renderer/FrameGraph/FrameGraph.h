#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__

#include <string>
#include <functional>
#include <stack>

#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"
#include "Resources/Core/ResourceDTO.h"

#include "Renderer/FrameGraph/FrameGraphSerialization.h"
#include "Renderer/FrameGraph/Pass.h"
#include "Renderer/FrameGraph/FrameGraphData.h"

namespace Engine {
  namespace FrameGraph {

    class SHIRABE_TEST_EXPORT Graph
      : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
    {
      DeclareLogTag(Graph);

      friend class GraphBuilder;
      friend class Serialization::FrameGraphGraphVizSerializer;

    public:
      Graph()  = default;
      ~Graph() = default;

      bool
        execute();

      virtual inline
        void acceptSerializer(Ptr<IFrameGraphSerializer> s)
      {
        s->serializeFrameGraph(*this);
      }

      virtual inline
        void acceptDeserializer(Ptr<IFrameGraphDeserializer> const&d)
      {
        d->deserializeFrameGraph(*this);
      }

    private:
      PassMap &passes();

      bool addPass(Ptr<PassBase> const&);

      PassMap                     m_passes;
      AdjacencyListMap<PassUID_t> m_passAdjacency;
      std::stack<PassUID_t>       m_passExecutionOrder;
    };

  }
}

#endif