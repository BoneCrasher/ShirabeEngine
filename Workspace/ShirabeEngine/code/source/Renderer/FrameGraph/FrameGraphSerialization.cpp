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

        return output;
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
          m_stream
            << String::format("\n  subgraph cluster_pass%0 {\n", sourceUID);

          Ptr<PassBase> sourcePass = graph.m_passes.at(sourceUID);
          sourcePass->acceptSerializer(GetNonDeletingSelfPtrType(this));

          // Write out the passes' resources
          if(passResourcesAdj.find(sourceUID) != passResourcesAdj.end()) {
            PublicResourceIdList const&passResources = passResourcesAdj.at(sourceUID);
            // Create
            std::vector<FrameGraphResourceId_t> creations = filter(passResources, FrameGraphResourceType::Texture);
            if(!creations.empty())
              for(FrameGraphResourceId_t const&id : creations) {
                FrameGraphResource const&resource = graph.m_resources.at(id);
                FrameGraphTexture  const&texture  = std::get<FrameGraphTexture>(resource.data);
                writeTextureResource(id, resource, texture);
              }
            // Read/Write
            std::vector<FrameGraphResourceId_t> views = filter(passResources, FrameGraphResourceType::TextureView);
            if(!views.empty()) {
              //std::string rankDeclaration = "{ rank=same; ";
              for(FrameGraphResourceId_t const&id : views) {
                FrameGraphResource    const&resource       = graph.m_resources.at(id);
                FrameGraphResource    const&parentResource = graph.m_resources.at(resource.parentResource);
                FrameGraphTextureView const&view           = std::get<FrameGraphTextureView>(resource.data);
                writeTextureResourceView(id, parentResource, resource, view);
                //rankDeclaration += String::format("TextureView%0; ", id);
              }

              //rankDeclaration += " }";

              // m_stream << rankDeclaration << "\n";
            }
          }

          m_stream << "  }\n\n";

          if(passAdjacency.find(sourceUID) != passAdjacency.end()) {
            for(PassUID_t const&targetUID : passAdjacency.at(sourceUID)) {
              writePassEdge(sourceUID, targetUID);
            }
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
      m_stream
        << "digraph FrameGraph {\n"
        << "  rankdir=LR;\n"
        << "  colorscheme=svg;\n"
        << "  graph[fontname=\"verdana\"];\n"
        << "  edge[fontname=\"verdana\"];\n"
        << "  node[fontname=\"verdana\"];\n"
        //<< "  graph [nodesep=1.5,ranksep=1.5,clusterrank=local];\n"
        /*<< "  node [];\n"*/;


    }

    void FrameGraphGraphVizSerializer::endGraph() {
      m_stream << "}";
    }

    void
      FrameGraphGraphVizSerializer::writePass(
        PassBase const&pass)
    {
      static constexpr char const*passStyle = "shape=box,style=filled,color=\"lightsteelblue\",fontsize=20";
      std::string passLabel = String::format("Pass #%0\n%1", pass.passUID(), pass.passName());

      m_stream << "    Pass" << pass.passUID() << " [" << passStyle << ",label=\"" << passLabel << "\"];\n";
    }

    void
      FrameGraphGraphVizSerializer::writePassEdge(
        PassUID_t const&source,
        PassUID_t const&target)
    {
      static constexpr char const*passEdgeStyle = "tailport=e,headport=w";
      // m_stream << "  Pass" << source << " -> " << "Pass" << target << " [" << passEdgeStyle << "];\n";
    }

    void
      FrameGraphGraphVizSerializer::writeTextureResource(
        FrameGraphResourceId_t const&id,
        FrameGraphResource     const&resource,
        FrameGraphTexture      const&texture)
    {      
      static constexpr char const*textureStyle = "shape=none";
      std::string textureLabel =
        String::format(
          "<<table bgcolor=\"#DAA520\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"0\">"
          "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"16\"><b>Texture #%0</b></font></td></tr>"
          "<tr><td align=\"left\">Name:</td><td align=\"left\">%1</td></tr>"
          "<tr><td align=\"left\">Sizes:</td><td align=\"left\">%2 x %3 x %4</td></tr>"
          "<tr><td align=\"left\">Format:</td><td align=\"left\">%5</td></tr>"
          "<tr><td align=\"left\">Array-Levels:</td><td align=\"left\">%6</td></tr>"
          "<tr><td align=\"left\">Mip-Levels:</td><td align=\"left\">%7</td></tr>"
          "<tr><td align=\"left\">Initial-State:</td><td align=\"left\">%8</td></tr>"
          "</table>>",
          resource.resourceId,
          resource.readableName,
          texture.width, texture.height, texture.depth,
          to_string(texture.format),
          texture.arraySize,
          texture.mipLevels,
          to_string(texture.initialState));
      
      m_stream << "    Texture" << id << " [" << textureStyle << ",label=" << textureLabel << "];\n";

      static constexpr char const*pass2TextureEdgeStyle = "style=dotted,weight=3";
      m_stream << "    Pass" << resource.assignedPassUID << " -> Texture" << id << " [" << pass2TextureEdgeStyle << ",constraint=false];\n";

      m_stream << "{ rank=same; Pass" << resource.assignedPassUID << "; Texture" << id << "}\n";
    }

    void
      FrameGraphGraphVizSerializer::writeTextureResourceView(
        FrameGraphResourceId_t const&id,
        FrameGraphResource     const&parentResource,
        FrameGraphResource     const&resource,
        FrameGraphTextureView  const&view)
    {
      bool parentResourceIsTexture     = (parentResource.type == FrameGraphResourceType::Texture);
      bool parentResourceIsTextureView = (parentResource.type == FrameGraphResourceType::TextureView);
      bool viewIsReadMode              = view.mode.check(FrameGraphViewAccessMode::Read);
      bool viewIsWriteMode             = view.mode.check(FrameGraphViewAccessMode::Write);
      bool passTransition              = (parentResource.assignedPassUID != resource.assignedPassUID);

      std::string passId   = String::format("Pass%0", resource.assignedPassUID);
      std::string viewId   = String::format("TextureView%0", id);
      std::string parentId = "";
      if(parentResourceIsTexture)
        parentId = String::format("Texture%0", parentResource.resourceId);
      else if(parentResourceIsTextureView)
        parentId = String::format("TextureView%0", parentResource.resourceId);

      std::string color = "black";

      static constexpr char const*viewStyle = "shape=none";
      std::string viewLabel = 
        String::format(
          "<<table bgcolor=\"#%0\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"0\">"
          "<tr><td colspan=\"2\"><font point-size=\"16\"><b>TextureView #%1</b></font></td></tr>"
          "<tr><td align=\"left\">SubjacentResourceId:</td><td align=\"left\">%2</td></tr>"
          "<tr><td align=\"left\">Mode:</td><td align=\"left\">%3</td></tr>"
          "<tr><td align=\"left\">Format:</td><td align=\"left\">%4</td></tr>"
          "<tr><td align=\"left\">ArrayRange:</td><td align=\"left\">%5</td></tr>"
          "<tr><td align=\"left\">MipRange:</td><td align=\"left\">%6</td></tr>"
          "</table>>",
          resource.resourceId,
          resource.subjacentResource,
          (viewIsReadMode ? "Read" : "Write"),
          to_string(view.format),
          to_string(view.arraySliceRange),
          to_string(view.mipSliceRange));

      m_stream << "    " << viewId << " [" << viewStyle << ",color=" << color << ",label=" << viewLabel << "];\n";

      if(viewIsReadMode) {
        static constexpr char const*viewRead2PassEdgeStyle = "tailport=e,headport=w,weight=2";
        m_stream << "    " << viewId << " -> " << passId << " [" << viewRead2PassEdgeStyle << ",color=" << color << "];\n";
      }
      else if(viewIsWriteMode) {
        static constexpr char const*pass2ViewWriteEdgeStyle = "tailport=e,headport=w,weight=2";
        m_stream << "    " << passId << " -> " << viewId  << " [" << pass2ViewWriteEdgeStyle << ",color=" << color << "];\n";
      }

      if(parentResourceIsTextureView) {
        static constexpr char const*parent2ViewStyle = "tailport=e,headport=w,weight=1";
        m_stream << "    " << parentId << " -> " << viewId << " [" << parent2ViewStyle << ",color=" << color << "];\n";
      }

      std::string mode = "";
      if(viewIsReadMode) {
        mode = "read";
      }
      else if(viewIsWriteMode) {
        mode = "write";
      }
    }

  }
}