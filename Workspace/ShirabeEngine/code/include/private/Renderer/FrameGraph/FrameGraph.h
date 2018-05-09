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
      bool
        execute();

      virtual inline
        void acceptSerializer(Ptr<IFrameGraphSerializer> s)
      {
        s->serializeGraph(*this);
      }

      virtual inline
        void acceptDeserializer(Ptr<IFrameGraphDeserializer> const&d)
      {
        d->deserializeGraph(*this);
      }

      inline 
        Graph& operator=(Graph const&other)
      {
        m_passes                  = other.m_passes;
        m_passAdjacency           = other.m_passAdjacency;
        m_passExecutionOrder      = other.m_passExecutionOrder;
        m_resourceAdjacency       = other.m_resourceAdjacency;
        m_resourceOrder           = other.m_resourceOrder;
        m_passToResourceAdjacency = other.m_passToResourceAdjacency;

        return (*this);
      }

    private:
      PassMap &passes();

      bool addPass(Ptr<PassBase> const&);

      PassMap                     m_passes;
      AdjacencyListMap<PassUID_t> m_passAdjacency;
      std::stack<PassUID_t>       m_passExecutionOrder;

      FrameGraphResourceMap                    m_resources;
      AdjacencyListMap<FrameGraphResourceId_t> m_resourceAdjacency;
      std::stack<FrameGraphResourceId_t>       m_resourceOrder;

      AdjacencyListMap<PassUID_t, FrameGraphResourceId_t> m_passToResourceAdjacency;
    };

  }
}

#endif