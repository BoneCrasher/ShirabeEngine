#include "Renderer/FrameGraph/FrameGraphSerialization.h"
#include "Renderer/FrameGraph/FrameGraphData.h"
#include "Renderer/FrameGraph/FrameGraph.h"
#include "Renderer/FrameGraph/Pass.h"

namespace Engine {
  namespace Serialization {
    using namespace Engine::FrameGraph;

    bool 
      FrameGraphGraphVizSerializer::initialize()
    {
      return true;
    }

    bool
      FrameGraphGraphVizSerializer::deinitialize()
    {
      m_stream.str("");
      return true;
    }

    bool
      FrameGraphGraphVizSerializer::serializeFrameGraph(Graph const&graph)
    {
      beginGraph();

      std::stack<PassUID_t>
        orderCopy = graph.m_passExecutionOrder;
      AdjacencyListMap<PassUID_t>
        const&adjacency = graph.m_passAdjacency;

      while(!orderCopy.empty()) {
        PassUID_t sourceUID  = orderCopy.top();
        if(sourceUID > 0) {
          Ptr<PassBase> sourcePass = graph.m_passes.at(sourceUID);

          writePass(*sourcePass);

          if(adjacency.find(sourceUID) != adjacency.end()) {
            for(PassUID_t const&targetUID : adjacency.at(sourceUID)) {
              writePassEdge(sourceUID, targetUID);
            }
          }
        }

        orderCopy.pop();
      }

      endGraph();

      return true;
    }

    bool
      FrameGraphGraphVizSerializer::serializePass(PassBase const&pass)
    {
      return true;
    }

    bool
      FrameGraphGraphVizSerializer::serializeFrameGraphResource(FrameGraphResource const&resource)
    {
      return true;
    }

    bool
      FrameGraphGraphVizSerializer::deserializeFrameGraph(Graph &graph)
    {
      return true;
    }

    bool
      FrameGraphGraphVizSerializer::deserializePass(PassBase &pass)
    {
      return true;
    }

    bool
      FrameGraphGraphVizSerializer::deserializeFrameGraphResource(FrameGraphResource &resource)
    {
      return true;
    }

    bool 
      FrameGraphGraphVizSerializer::writeToFile(
        std::string const&filename)
    {
      try {
        std::ofstream out(std::string(filename).append(".gv"));
        out << m_stream.str();
        out.close();

        return true;
      }
      catch(...) {
        return false;
      }
    }

    std::string 
      FrameGraphGraphVizSerializer::serializeResultToString()
    {
      return m_stream.str();
    }

    void FrameGraphGraphVizSerializer::beginGraph() {
      m_stream << "digraph FrameGraph {\n";
    }

    void FrameGraphGraphVizSerializer::endGraph() {
      m_stream << "}";
    }

    void
      FrameGraphGraphVizSerializer::writePass(
        PassBase const&pass)
    {
      m_stream << pass.passUID() << " [shape=box,label=\"" << pass.passName() << "\"];\n";
    }

    void
      FrameGraphGraphVizSerializer::writePassEdge(
        PassUID_t const&source,
        PassUID_t const&target)
    {
      m_stream << source << " -> " << target << ";\n";
    }


  }
}