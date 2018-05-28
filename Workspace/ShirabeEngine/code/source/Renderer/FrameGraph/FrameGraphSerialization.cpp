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

    using FilterElementCompareCallbackFunction_t =
      std::function<bool(FrameGraphResource const&)>;

    using
      FilterFunction_t =
      std::function<FrameGraphResourceIdList(FrameGraphResourceIdList const&, FilterElementCompareCallbackFunction_t const&)>;

    bool
      FrameGraphGraphVizSerializer::serializeGraph(Graph const&graph)
    {
      UniquePtr<Graph::Accessor> accessor = graph.getAccessor(PassKey<FrameGraphGraphVizSerializer>());

      FrameGraphResources const&resources = accessor->resourceData();

      FilterFunction_t filter =[&](
        FrameGraphResourceIdList               const&ids,
        FilterElementCompareCallbackFunction_t const&cb) -> FrameGraphResourceIdList
      {
        FrameGraphResourceIdList output{ };
        if(ids.empty())
          return output;

        std::copy_if(
          ids.begin(),
          ids.end(),
          std::back_inserter(output),
          [&] (FrameGraphResourceId_t const&id) -> bool
        {
          if(resources.resources().size() <= id)
            return false;

          if(cb)
            return cb(*resources.get<FrameGraphResource>(id));
        });

        return output;
      };

      beginGraph();

      // Write registered resources
      for(RefIndex::value_type const&textureRef : resources.textures()) {
        FrameGraphTexture const&texture = *resources.get<FrameGraphTexture>(textureRef);
        writeTextureResource(texture);
      }

      for(RefIndex::value_type const&textureViewRef : resources.textureViews()) {
        FrameGraphTextureView const&view   = *resources.get<FrameGraphTextureView>(textureViewRef);
        FrameGraphResource    const&parent = *resources.get<FrameGraphResource>(view.parentResource);

        writeTextureResourceView(parent, view);
      }

      for(RefIndex::value_type const&renderableListRef : resources.renderablesLists()) {
        FrameGraphRenderableList const&list = *resources.get<FrameGraphRenderableList>(renderableListRef);
        writeRenderableList(list);
      }

      for(RefIndex::value_type const&renderableListViewRef : resources.renderableListViews()) {
        FrameGraphRenderableListView const&view   = *resources.get<FrameGraphRenderableListView>(renderableListViewRef);
        FrameGraphResource           const&parent = *resources.get<FrameGraphResource>(view.parentResource);
        writeRenderableListView(parent, view);
      }

      // Write passes and adjacent resources
      AdjacencyListMap<PassUID_t>
        const&passAdjacency = accessor->passAdjacency();
      AdjacencyListMap<PassUID_t, FrameGraphResourceId_t>
        const&passResourcesAdj = accessor->passToResourceAdjacency();

      std::stack<PassUID_t>
        passOrderCopy = accessor->passExecutionOrder();

      while(!passOrderCopy.empty()) {
        // Write all passes and their connections
        PassUID_t sourceUID  = passOrderCopy.top();
        if(sourceUID == 0) { // Pseudo-Pass
        }
        else {
          m_stream
            << String::format("\n  subgraph pass%0 {\n", sourceUID);

          Ptr<PassBase> sourcePass = graph.passes().at(sourceUID);
          sourcePass->acceptSerializer(GetNonDeletingSelfPtrType(this));

          // Write out the passes' resources
          if(passResourcesAdj.find(sourceUID) != passResourcesAdj.end()) {
            PublicResourceIdList const&passResources = passResourcesAdj.at(sourceUID);
            // Create Texture
            std::vector<FrameGraphResourceId_t> creations = filter(passResources, [] (FrameGraphResource const&r) -> bool {
              return (r.type == FrameGraphResourceType::Texture && r.assignedPassUID != 0);
            });
            if(!creations.empty())
              for(FrameGraphResourceId_t const&id : creations) {
                FrameGraphTexture const&texture = *resources.get<FrameGraphTexture>(id);
                writePass2TextureResourceEdge(texture);
              }
            // Read/Write Texture
            std::vector<FrameGraphResourceId_t> readViews = filter(passResources, [] (FrameGraphResource const&r) -> bool {
              return (r.type == FrameGraphResourceType::TextureView);
            });
            if(!readViews.empty()) {
              for(FrameGraphResourceId_t const&id : readViews) {
                FrameGraphTextureView const&view           = *resources.get<FrameGraphTextureView>(id);
                FrameGraphResource    const&parentResource = *resources.get<FrameGraphResource>(view.parentResource);
                writeTextureResourceViewEdge(parentResource, view);
              }
            }
            // Use Renderables
            std::vector<FrameGraphResourceId_t> renderableListViews = filter(passResources, [] (FrameGraphResource const&r) -> bool {
              return (r.type == FrameGraphResourceType::RenderableListView);
            });
            if(!renderableListViews.empty()) {
              for(FrameGraphResourceId_t const&id : renderableListViews) {
                FrameGraphRenderableListView const&view           = *resources.get<FrameGraphRenderableListView>(id);
                FrameGraphResource           const&parentResource = *resources.get<FrameGraphResource>(view.parentResource);

                writeRenderableResourceViewEdge(parentResource, view);
              }
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
        << "  overlap=false;\n"
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
      std::string passLabel = String::format(
        "<<table bgcolor=\"#429692\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
        "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"18\"><b>Pass (PID: %0)</b></font></td></tr>"
        "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"16\">%1</font></td></tr>"
        "</table>>",
        pass.passUID(),
        pass.passName());

      m_stream << "    Pass" << pass.passUID() << " [shape=none, label=" << passLabel << "];\n";
    }

    void
      FrameGraphGraphVizSerializer::writePassEdge(
        PassUID_t const&source,
        PassUID_t const&target)
    {
      static constexpr char const*passEdgeStyle = "style=bold,color=\"#d95d39\",tailport=e,headport=w";
      m_stream << "  Pass" << source << " -> " << "Pass" << target << " [" << passEdgeStyle << "];\n";
    }

    void
      FrameGraphGraphVizSerializer::writeRenderableList(
        FrameGraphRenderableList const&list)
    {
      static constexpr char const*listStyle = "shape=none";
      std::string listLabel =
        String::format(
          "<<table bgcolor=\"#edb036\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
          "<tr><td colspan=\"2\"><font point-size=\"16\">&lt;&lt;%0&gt;&gt;</font></td></tr>"
          "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"18\"><b>RenderableList (RID: %1)</b></font></td></tr>"
          "</table>>",
          "import",
          list.resourceId,
          list.readableName);

      m_stream << "    RenderableList" << list.resourceId << " [" << listStyle << ",label=" << listLabel << "];\n";
    }

    void
      FrameGraphGraphVizSerializer::writeRenderableListView(
        FrameGraphResource           const&parentResource,
        FrameGraphRenderableListView const&view)
    {
      std::string viewId   = String::format("RenderableListView%0", view.resourceId);

      static constexpr char const*viewStyle = "shape=none";
      std::string viewLabel =
        String::format(
          "<<table bgcolor=\"#%0\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
          "<tr><td colspan=\"2\"><font point-size=\"16\"><b>RenderableListView (RID: %1)</b></font></td></tr>"
          "<tr><td align=\"left\">SubjacentResourceId:</td><td align=\"left\">%2</td></tr>"
          "</table>>",
          "f46e5d",
          view.resourceId,
          view.subjacentResource);

      m_stream << "    " << viewId << " [" << viewStyle << ",label=" << viewLabel << "];\n";
    }

    void
      FrameGraphGraphVizSerializer::writeRenderableResourceViewEdge(
        FrameGraphResource           const&parentResource,
        FrameGraphRenderableListView const&view)
    {
      std::string passId   = String::format("Pass%0", view.assignedPassUID);
      std::string viewId   = String::format("RenderableListView%0", view.resourceId); std::string parentId = "";
      if(parentResource.type == FrameGraphResourceType::RenderableList)
        parentId = String::format("RenderableList%0", parentResource.resourceId);
      else
        parentId = String::format("RenderableListView%0", parentResource.resourceId);

      static constexpr char const*pass2ViewEdgeStyle = "tailport=e,headport=w,weight=2,style=dashed";
      m_stream << "    " << viewId << " -> " << passId  << " [" << pass2ViewEdgeStyle << "];\n";

      static constexpr char const*parent2ViewStyle = "tailport=e,headport=w,weight=1,color=\"#bbbbbb\"";
      m_stream << "    " << parentId << " -> " << viewId << " [" << parent2ViewStyle << "];\n";
    }

    void
      FrameGraphGraphVizSerializer::writeTextureResource(
        FrameGraphTexture const&texture)
    {
      std::string mode = "create";
      if(texture.assignedPassUID == 0)
        mode = "import";

      static constexpr char const*textureStyle = "shape=none";
      std::string textureLabel =
        String::format(
          "<<table bgcolor=\"#e0b867\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
          "<tr><td colspan=\"2\"><font point-size=\"16\">&lt;&lt;%0&gt;&gt;</font></td></tr>"
          "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"18\"><b>Texture (RID: %1)</b></font></td></tr>"
          "<tr><td align=\"left\">Name:</td><td align=\"left\">%2</td></tr>"
          "<tr><td align=\"left\">Sizes:</td><td align=\"left\">%3 x %4 x %5</td></tr>"
          "<tr><td align=\"left\">Format:</td><td align=\"left\">%6</td></tr>"
          "<tr><td align=\"left\">Array-Levels:</td><td align=\"left\">%7</td></tr>"
          "<tr><td align=\"left\">Mip-Levels:</td><td align=\"left\">%8</td></tr>"
          "<tr><td align=\"left\">Initial-State:</td><td align=\"left\">%9</td></tr>"
          "<tr><td align=\"left\">Reference-Count:</td><td align=\"left\">%10</td></tr>"
          "</table>>",
          mode,
          texture.resourceId,
          texture.readableName,
          texture.width, texture.height, texture.depth,
          to_string(texture.format),
          texture.arraySize,
          texture.mipLevels,
          to_string(texture.initialState),
          texture.referenceCount);

      m_stream << "    Texture" << texture.resourceId << " [" << textureStyle << ",label=" << textureLabel << "];\n";
    }

    void
      FrameGraphGraphVizSerializer::writePass2TextureResourceEdge(
        FrameGraphTexture      const&texture)
    {
      static constexpr char const*pass2TextureEdgeStyle = "style=dotted,weight=2";
      m_stream << "    Pass" << texture.assignedPassUID << " -> Texture" << texture.resourceId << " [" << pass2TextureEdgeStyle << ",constraint=false];\n";
      m_stream << "    { rank=same; Pass" << texture.assignedPassUID << "; Texture" << texture.resourceId << "}\n";
    }

    void
      FrameGraphGraphVizSerializer::writeTextureResourceView(
        FrameGraphResource     const&parentResource,
        FrameGraphTextureView  const&view)
    {
      bool viewIsReadMode  = view.mode.check(FrameGraphViewAccessMode::Read);
      bool viewIsWriteMode = view.mode.check(FrameGraphViewAccessMode::Write);

      std::string viewId   = String::format("TextureView%0", view.resourceId);

      static constexpr char const*viewStyle = "shape=none";
      std::string viewLabel =
        String::format(
          "<<table bgcolor=\"#%0\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
          "<tr><td colspan=\"2\"><font point-size=\"16\"><b>TextureView (RID: %1)</b></font></td></tr>"
          "<tr><td align=\"left\">SubjacentResourceId:</td><td align=\"left\">%2</td></tr>"
          "<tr><td align=\"left\">Mode:</td><td align=\"left\">%3</td></tr>"
          "<tr><td align=\"left\">Format:</td><td align=\"left\">%4</td></tr>"
          "<tr><td align=\"left\">ArrayRange:</td><td align=\"left\">%5</td></tr>"
          "<tr><td align=\"left\">MipRange:</td><td align=\"left\">%6</td></tr>"
          "<tr><td align=\"left\">Reference-Count:</td><td align=\"left\">%7</td></tr>"
          "</table>>",
          (viewIsReadMode ? "68a357" : "c97064"),
          view.resourceId,
          view.subjacentResource,
          (viewIsReadMode ? "Read" : "Write"),
          to_string(view.format),
          to_string(view.arraySliceRange),
          to_string(view.mipSliceRange),
          view.referenceCount);

      m_stream << "    " << viewId << " [" << viewStyle << ",label=" << viewLabel << "];\n";
    }

    void
      FrameGraphGraphVizSerializer::writeTextureResourceViewEdge(
        FrameGraphResource     const&parentResource,
        FrameGraphTextureView  const&view)
    {
      bool parentResourceIsTexture     = (parentResource.type == FrameGraphResourceType::Texture);
      bool parentResourceIsTextureView = (parentResource.type == FrameGraphResourceType::TextureView);
      bool viewIsReadMode              = view.mode.check(FrameGraphViewAccessMode::Read);
      bool viewIsWriteMode             = view.mode.check(FrameGraphViewAccessMode::Write);

      std::string passId   = String::format("Pass%0", view.assignedPassUID);
      std::string viewId   = String::format("TextureView%0", view.resourceId);
      std::string parentId = "";
      if(parentResourceIsTexture)
        parentId = String::format("Texture%0", parentResource.resourceId);
      else if(parentResourceIsTextureView)
        parentId = String::format("TextureView%0", parentResource.resourceId);

      std::string color = "black";

      if(viewIsReadMode) {
        static constexpr char const*viewRead2PassEdgeStyle = "tailport=e,headport=w,weight=2,style=dashed";
        m_stream << "    " << viewId << " -> " << passId << " [" << viewRead2PassEdgeStyle << ",color=" << color << "];\n";
      }
      else if(viewIsWriteMode) {
        static constexpr char const*pass2ViewWriteEdgeStyle = "tailport=e,headport=w,weight=2,style=dashed";
        m_stream << "    " << passId << " -> " << viewId  << " [" << pass2ViewWriteEdgeStyle << ",color=" << color << "];\n";
      }

      if(parentResourceIsTextureView) {
        static constexpr char const*parent2ViewStyle = "tailport=e,headport=w,weight=1,style=dashed";
        m_stream << "    " << parentId << " -> " << viewId << " [" << parent2ViewStyle << ",color=" << color << "];\n";
      }
      else {
        static constexpr char const*parent2ViewStyle = "tailport=e,headport=w,weight=1,color=\"#bbbbbb\"";
        m_stream << "    " << parentId << " -> " << viewId << " [" << parent2ViewStyle << "];\n";
      }
    }

  }
}