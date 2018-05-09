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
      FrameGraphGraphVizSerializer::serializeGraph(Graph const&graph)
    {
      std::function<std::vector<FrameGraphResourceId_t>(std::vector<FrameGraphResourceId_t> const&, FrameGraphResourceType const&)> filter =
        [&] (std::vector<FrameGraphResourceId_t> const&ids, FrameGraphResourceType const&type) -> std::vector<FrameGraphResourceId_t>
      {
        std::vector<FrameGraphResourceId_t> output {};
        if(ids.empty())
          return output;

        FrameGraphResourceMap const&resources = graph.m_resources;

        std::copy_if(
          ids.begin(),
          ids.end(),
          std::back_inserter(output),
          [&] (FrameGraphResourceId_t const&id) -> bool
        {
          if(resources.find(id) == resources.end())
            return false;

          return (resources.at(id).type == type);
        });
      };

      beginGraph();

      AdjacencyListMap<PassUID_t>
        const&passAdjacency = graph.m_passAdjacency;
      AdjacencyListMap<PassUID_t, FrameGraphResourceId_t> 
        const&passResourcesAdj = graph.m_passToResourceAdjacency;

      std::stack<PassUID_t>
        passOrderCopy = graph.m_passExecutionOrder;

      while(!passOrderCopy.empty()) {
        // Write all passes and their connections
        PassUID_t sourceUID  = passOrderCopy.top();
        if(sourceUID > 0) {
          Ptr<PassBase> sourcePass = graph.m_passes.at(sourceUID);

          sourcePass->acceptSerializer(GetNonDeletingSelfPtrType(this));

          if(passAdjacency.find(sourceUID) != passAdjacency.end()) {
            for(PassUID_t const&targetUID : passAdjacency.at(sourceUID)) {
              writePassEdge(sourceUID, targetUID);
            }
          }
        }

        // Write out the passes' resources
        if(passResourcesAdj.find(sourceUID) != passResourcesAdj.end()) {
          std::vector<FrameGraphResourceId_t> passResources = passResourcesAdj.at(sourceUID);
          // Create
          try {
            std::vector<FrameGraphResourceId_t> creations = filter(passResources, FrameGraphResourceType::Texture);
            for(FrameGraphResourceId_t const&id : creations) {
              FrameGraphResource const&resource = graph.m_resources.at(id);
              FrameGraphTexture  const&texture  = std::get<FrameGraphTexture>(resource.data);
              writeTextureResource(id, resource, texture);
            }
            // Read/Write
            std::vector<FrameGraphResourceId_t> views = filter(passResources, FrameGraphResourceType::TextureView);
            for(FrameGraphResourceId_t const&id : views) {
              FrameGraphResource    const&resource       = graph.m_resources.at(id);
              FrameGraphResource    const&parentResource = graph.m_resources.at(resource.parentResource);
              FrameGraphTextureView const&view           = std::get<FrameGraphTextureView>(resource.data);
              writeTextureResourceView(id, parentResource, resource, view);
            }
          }
          catch(std::bad_variant_access const&bva) {
            Log::Error(logTag(), "Internal resource type error.");
          }
        }

        passOrderCopy.pop();
      }
      
      endGraph();

      return true;
    }

    bool
      FrameGraphGraphVizSerializer::serializePass(PassBase const&pass)
    {
      writePass(pass);

      return true;
    }

    bool
      FrameGraphGraphVizSerializer::serializeResource(FrameGraphResource const&resource)
    {
      return true;
    }

    bool
      FrameGraphGraphVizSerializer::deserializeGraph(Graph &graph)
    {
      return true;
    }

    bool
      FrameGraphGraphVizSerializer::deserializePass(PassBase &pass)
    {
      return true;
    }

    bool
      FrameGraphGraphVizSerializer::deserializeResource(FrameGraphResource &resource)
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
      m_stream << "  Pass-" << pass.passUID() << " [shape=polygon,sides=6,label=\"" << pass.passName() << "\"];\n";
    }

    void
      FrameGraphGraphVizSerializer::writePassEdge(
        PassUID_t const&source,
        PassUID_t const&target)
    {
      m_stream << " Pass-" << source << " -> " << "Pass-" << target << ";\n";
    }

    void 
      FrameGraphGraphVizSerializer::writeTextureResource(
        FrameGraphResourceId_t const&id,
        FrameGraphResource     const&resource,
        FrameGraphTexture      const&texture)
    {
      m_stream << "  Texture-" << id << " [shape=box,label=\"" << resource.readableName << "\"]\n";
      m_stream << "  Pass-" << resource.assignedPassUID << " -> Texture-" << id << " [style=dotted]\n";
    }

    void
      FrameGraphGraphVizSerializer::writeTextureResourceView(
        FrameGraphResourceId_t const&id,
        FrameGraphResource     const&parentResource,
        FrameGraphResource     const&resource,
        FrameGraphTextureView  const&view)
    {
      if(parentResource.type == FrameGraphResourceType::Texture)
        m_stream << "  Texture-";
      else if(parentResource.type == FrameGraphResourceType::TextureView)
        m_stream << "  TextureView-" << parentResource.resourceId << " -> " << "TextureView-" << id << ";\n";

      m_stream << parentResource.resourceId << " -> " << "TextureView-" << id;
      
      if(view.mode.check(FrameGraphViewAccessMode::Read))
        m_stream << " [label=\"<<read>>\"]";
      else if(view.mode.check(FrameGraphViewAccessMode::Write))
        m_stream << " [label=\"<<write>>\"]";

      m_stream << ";";
    }

  }
}