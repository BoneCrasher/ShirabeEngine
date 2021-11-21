#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION

#include "renderer/rendergraph/framegraphdata.h"
#include "renderer/rendergraph/framegraph.h"
#include "renderer/rendergraph/pass.h"
#include "renderer/rendergraph/framegraphserialization.h"

namespace engine
{
    namespace serialization
    {
        using namespace engine::framegraph;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CRenderGraphGraphVizSerializer::CRenderGraphSerializationResult::CRenderGraphSerializationResult(std::string const &aResult)
            : IResult()
            , mResult(aResult)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizSerializer::CRenderGraphSerializationResult::asString(std::string &aOutString) const
        {
            aOutString = mResult;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizSerializer::CRenderGraphSerializationResult::asBinaryBuffer(std::vector<uint8_t> &aOutBuffer) const
        {
            aOutBuffer = std::vector<uint8_t>(mResult.begin(), mResult.end());
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizSerializer::initialize()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizSerializer::deinitialize()
        {
            mStream.str("");
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        using FilterElementCompareCallbackFunction_t = std::function<bool(SRenderGraphResource const&)>;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        using FilterFunction_t = std::function<RenderGraphResourceIdList(RenderGraphResourceIdList const&, FilterElementCompareCallbackFunction_t const&)>;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizSerializer::serialize(
                CGraph                   const &aSource,
                Shared<IResult>       &aOutResult)
        {
            Shared<CRenderGraphSerializationResult> result = nullptr;

            bool const serialized = serializeGraph(aSource);
            if(serialized)
            {
                result = makeShared<CRenderGraphSerializationResult>(mStream.str());
                mStream.str(std::string());
            }

            aOutResult = result;

            return serialized;
        }
        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizSerializer::serializeGraph(CGraph const &aGraph)
        {


            Unique<CGraph::CAccessor> accessor = aGraph.getAccessor(CPassKey<CRenderGraphGraphVizSerializer>());

            CRenderGraphResources const &resources = accessor->resourceData();

            FilterFunction_t const filter = [&](
                    RenderGraphResourceIdList               const &aUIDs,
                    FilterElementCompareCallbackFunction_t const &aCallback) -> RenderGraphResourceIdList
            {
                RenderGraphResourceIdList output{ };
                if(aUIDs.empty())
                    return output;

                std::copy_if(
                            aUIDs.begin(),
                            aUIDs.end(),
                            std::back_inserter(output),
                            [&] (RenderGraphResourceId_t const &aId) -> bool
                {
                    if(resources.resources().size() <= aId)
                        return false;

                    if(aCallback)
                        return aCallback(*resources.get<SRenderGraphResource>(aId));

                    return false;
                });

                return output;
            };

            beginGraph();

            // Write registered resources
            for(RefIndex_t::value_type const &textureRef : resources.textures())
            {
                SRenderGraphDynamicTexture const &texture = *resources.get<SRenderGraphImage>(textureRef);
                writeTextureResource(texture);
            }

            for(RefIndex_t::value_type const &textureViewRef : resources.textureViews())
            {
                SRenderGraphImageView const &view   = *resources.get<SRenderGraphImageView>(textureViewRef);
                SRenderGraphResource    const &parent = *resources.get<SRenderGraphResource>(view.parentResource);

                writeTextureResourceView(parent, view);
            }

            for(RefIndex_t::value_type const &renderableListRef : resources.renderablesLists())
            {
                SRenderGraphRenderableList const &list = *resources.get<SRenderGraphRenderableList>(renderableListRef);

                writeRenderableList(list);
            }

            for(RefIndex_t::value_type const &renderableListViewRef : resources.renderableListViews())
            {
                SRenderGraphRenderableListView const &view   = *resources.get<SRenderGraphRenderableListView>(renderableListViewRef);
                SRenderGraphResource           const &parent = *resources.get<SRenderGraphResource>(view.parentResource);
                writeRenderableListView(parent, view);
            }

            // Write passes and adjacent resources
            AdjacencyListMap_t<PassUID_t>                         const &passAdjacency    = accessor->passAdjacency();
            AdjacencyListMap_t<PassUID_t, RenderGraphResourceId_t> const &passResourcesAdj = accessor->passToResourceAdjacency();

            std::stack<PassUID_t> passOrderCopy = accessor->passExecutionOrder();

            while(!passOrderCopy.empty())
            {
                // Write all passes and their connections
                PassUID_t const sourceUID  = passOrderCopy.top();
                if(sourceUID == 0)
                { // Pseudo-Pass
                }
                else
                {
                    mStream << StaticStringHelpers::format("\n  subgraph pass{} {\n", sourceUID);

                    Shared<CPassBase> sourcePass = aGraph.passes().at(sourceUID);

                    sourcePass->acceptSerializer(*this);

                    // Write out the passes' resources
                    if(passResourcesAdj.find(sourceUID) != passResourcesAdj.end())
                    {
                        RenderGraphResourceIdList const &passResources = passResourcesAdj.at(sourceUID);

                        // Create Texture
                        auto textureCreateFilterFn = [] (SRenderGraphResource const &aInput) -> bool
                        {
                                return (aInput.type == ERenderGraphResourceType::Texture /* && aInput.assignedPassUID != 0 */);
                        };
                        std::vector<RenderGraphResourceId_t> const creations = filter(passResources, textureCreateFilterFn);

                        if(!creations.empty())
                        {
                            for(RenderGraphResourceId_t const &id : creations)
                            {
                                SRenderGraphImage const &texture = *resources.get<SRenderGraphImage>(id);
                                writePass2TextureResourceEdge(texture);
                            }
                        }

                        // Read/Write Texture
                        auto const textureViewfilterFn = [] (SRenderGraphResource const &aInput) -> bool
                        {
                            return (aInput.type == ERenderGraphResourceType::TextureView);
                        };
                        std::vector<RenderGraphResourceId_t> readViews = filter(passResources, textureViewfilterFn);

                        if(!readViews.empty())
                        {
                            for(RenderGraphResourceId_t const &id : readViews)
                            {
                                SRenderGraphImageView const &view           = *resources.get<SRenderGraphImageView>(id);
                                SRenderGraphResource    const &parentResource = *resources.get<SRenderGraphResource>(view.parentResource);

                                writeTextureResourceViewEdge(sourceUID, parentResource, view);
                            }
                        }

                        // Use Renderables
                        auto const renderableFilterFn = [] (SRenderGraphResource const &aInput) -> bool
                        {
                            return (aInput.type == ERenderGraphResourceType::RenderableListView);
                        };
                        std::vector<RenderGraphResourceId_t> renderableListViews = filter(passResources, renderableFilterFn);

                        if(!renderableListViews.empty())
                        {
                            for(RenderGraphResourceId_t const &id : renderableListViews)
                            {
                                SRenderGraphRenderableListView const&view           = *resources.get<SRenderGraphRenderableListView>(id);
                                SRenderGraphResource           const&parentResource = *resources.get<SRenderGraphResource>(view.parentResource);

                                writeRenderableResourceViewEdge(parentResource, view);
                            }
                        }
                    }

                    mStream << "  }\n\n";

                    if(passAdjacency.find(sourceUID) != passAdjacency.end())
                    {
                        for(PassUID_t const&targetUID : passAdjacency.at(sourceUID))
                        {
                            writePassEdge(sourceUID, targetUID);
                        }
                    }
                }

                passOrderCopy.pop();
            }

            endGraph();

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizSerializer::serializePass(CPassBase const &aPass)
        {
            writePass(aPass);

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizSerializer::serializeResource(SRenderGraphResource const &aResource)
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizDeserializer::deserializeGraph(CGraph &aGraph)
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizDeserializer::deserializePass(CPassBase &aPass)
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderGraphGraphVizDeserializer::deserializeResource(SRenderGraphResource &aResource)
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::beginGraph()
        {
            mStream
                    << "strict digraph RenderGraph {\n"
                    << "  rankdir=LR;\n"
                    << "  colorscheme=svg;\n"
                    << "  overlap=false;\n"
                    << "  graph[fontname=\"verdana\"];\n"
                    << "  edge[fontname=\"verdana\"];\n"
                    << "  node[fontname=\"verdana\"];\n"
                       //<< "  graph [nodesep=1.5,ranksep=1.5,clusterrank=local];\n"
                       /*<< "  node [];\n"*/;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::endGraph()
        {
            mStream << "}";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::writePass(CPassBase const &aPass)
        {
            std::string const passLabel = StaticStringHelpers::format(
                        "<<table bgcolor=\"#429692\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
                        "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"18\"><b>Pass (PID: {})</b></font></td></tr>"
                        "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"16\">{}</font></td></tr>"
                        "</table>>",
                        aPass.passUID(),
                        aPass.passName());

            mStream << "    Pass" << aPass.passUID() << " [shape=none, label=" << passLabel << "];\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::writePassEdge(
                PassUID_t const &aSource,
                PassUID_t const &aTarget)
        {
            static constexpr char const*passEdgeStyle = "style=bold,color=\"#d95d39\",tailport=e,headport=w";
            mStream << "  Pass" << aSource << " -> " << "Pass" << aTarget << " [" << passEdgeStyle << "];\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::writeRenderableList(
                SRenderGraphRenderableList const &aList)
        {
            static constexpr char const*listStyle = "shape=none";
            std::string const listLabel =
                    StaticStringHelpers::format(
                        "<<table bgcolor=\"#edb036\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
                        "<tr><td colspan=\"2\"><font point-size=\"16\">&lt;&lt;{}&gt;&gt;</font></td></tr>"
                        "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"18\"><b>RenderableList (RID: {})</b></font></td></tr>"
                        "</table>>",
                        "import",
                        aList.resourceId,
                        aList.readableName);

            mStream << "    RenderableList" << aList.resourceId << " [" << listStyle << ",label=" << listLabel << "];\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::writeRenderableListView(
                SRenderGraphResource           const &aParentResource,
                SRenderGraphRenderableListView const &aView)
        {
            std::string viewId = StaticStringHelpers::format("RenderableListView{}", aView.resourceId);

            static constexpr char const*viewStyle = "shape=none";
            std::string viewLabel =
                    StaticStringHelpers::format(
                        "<<table bgcolor=\"#{}\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
                        "<tr><td colspan=\"2\"><font point-size=\"16\"><b>RenderableListView (RID: {})</b></font></td></tr>"
                        "<tr><td align=\"left\">SubjacentResourceId:</td><td align=\"left\">{}</td></tr>"
                        "</table>>",
                        "f46e5d",
                        aView.resourceId,
                        aView.subjacentResource);

            mStream << "    " << viewId << " [" << viewStyle << ",label=" << viewLabel << "];\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::writeRenderableResourceViewEdge(
                SRenderGraphResource           const &aParentResource,
                SRenderGraphRenderableListView const &aView)
        {
            std::string const passId   = StaticStringHelpers::format("Pass{}", aView.assignedPassUID);
            std::string const viewId   = StaticStringHelpers::format("RenderableListView{}", aView.resourceId);
            std::string       parentId = "";

            if(aParentResource.type == ERenderGraphResourceType::RenderableList)
            {
                parentId = StaticStringHelpers::format("RenderableList{}", aParentResource.resourceId);
            }
            else
            {
                parentId = StaticStringHelpers::format("RenderableListView{}", aParentResource.resourceId);
            }

            static constexpr char const *pass2ViewEdgeStyle = "tailport=e,headport=w,weight=2,style=dashed";
            mStream << "    " << viewId << " -> " << passId  << " [" << pass2ViewEdgeStyle << "];\n";

            static constexpr char const *parent2ViewStyle = "tailport=e,headport=w,weight=1,color=\"#bbbbbb\"";
            mStream << "    " << parentId << " -> " << viewId << " [" << parent2ViewStyle << "];\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::writeTextureResource(SRenderGraphImage const &aTexture)
        {
            std::string mode = "create";
            if(aTexture.isExternalResource)
                mode = "import";

            static constexpr char const *textureStyle = "shape=none";
            std::string const textureLabel =
                    StaticStringHelpers::format(
                        "<<table bgcolor=\"#e0b867\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
                        "<tr><td colspan=\"2\"><font point-size=\"16\">&lt;&lt;{}&gt;&gt;</font></td></tr>"
                        "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"18\"><b>Texture (RID: {})</b></font></td></tr>"
                        "<tr><td align=\"left\">Name:</td><td align=\"left\">{}</td></tr>"
                        "<tr><td align=\"left\">Sizes:</td><td align=\"left\">{} x {} x {}</td></tr>"
                        "<tr><td align=\"left\">Format:</td><td align=\"left\">{}</td></tr>"
                        "<tr><td align=\"left\">Array-Levels:</td><td align=\"left\">{}</td></tr>"
                        "<tr><td align=\"left\">Mip-Levels:</td><td align=\"left\">{}</td></tr>"
                        "<tr><td align=\"left\">Initial-State:</td><td align=\"left\">{}</td></tr>"
                        "<tr><td align=\"left\">Reference-Count:</td><td align=\"left\">{}0</td></tr>"
                        "</table>>",
                        mode,
                        aTexture.resourceId,
                        aTexture.readableName,
                        aTexture.width, aTexture.height, aTexture.depth,
                        convert_to_string(aTexture.format),
                        aTexture.arraySize,
                        aTexture.mipLevels,
                        convert_to_string(aTexture.initialState),
                        aTexture.referenceCount);

            mStream << "    Texture" << aTexture.resourceId << " [" << textureStyle << ",label=" << textureLabel << "];\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::writePass2TextureResourceEdge(SRenderGraphImage const &aTexture)
        {
            bool const isImported = aTexture.isExternalResource;

            if(isImported)
            {
                static constexpr char const *pass2TextureEdgeStyle = "style=dotted,weight=2";
                mStream << "    Texture" << aTexture.resourceId << " -> Pass" << aTexture.assignedPassUID << " [" << pass2TextureEdgeStyle << ",constraint=false];\n";
            }
            else
            {
                static constexpr char const *pass2TextureEdgeStyle = "style=dotted,weight=2";
                mStream << "    Pass" << aTexture.assignedPassUID << " -> Texture" << aTexture.resourceId << " [" << pass2TextureEdgeStyle << ",constraint=false];\n";
            }

            mStream << "    { rank=same; Pass" << aTexture.assignedPassUID << "; Texture" << aTexture.resourceId << "}\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::writeTextureResourceView(
                SRenderGraphResource     const &aParentResource,
                SRenderGraphImageView  const &aView)
        {
            bool const viewIsReadMode  = aView.mode.check(ERenderGraphViewAccessMode::Read);
            bool const viewIsWriteMode = aView.mode.check(ERenderGraphViewAccessMode::Write);
            bool const viewIsFwdMode   = aView.mode.check(ERenderGraphViewAccessMode::Forward);
            bool const viewIsAckMode   = aView.mode.check(ERenderGraphViewAccessMode::Accept);

            std::string viewId   = StaticStringHelpers::format("TextureView{}", aView.resourceId);

            static constexpr char const *viewStyle = "shape=none";
            std::string const viewLabel =
                    StaticStringHelpers::format(
                        "<<table bgcolor=\"#{}\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
                        "<tr><td colspan=\"2\"><font point-size=\"16\"><b>TextureView (RID: {})</b></font></td></tr>"
                        "<tr><td align=\"left\">SubjacentResourceId:</td><td align=\"left\">{}</td></tr>"
                        "<tr><td align=\"left\">Mode:</td><td align=\"left\">{}</td></tr>"
                        "<tr><td align=\"left\">Format:</td><td align=\"left\">{}</td></tr>"
                        "<tr><td align=\"left\">ArrayRange:</td><td align=\"left\">{}</td></tr>"
                        "<tr><td align=\"left\">MipRange:</td><td align=\"left\">{}</td></tr>"
                        "<tr><td align=\"left\">Reference-Count:</td><td align=\"left\">{}</td></tr>"
                        "</table>>",
                        (viewIsReadMode
                            ? "68a357"
                            : (viewIsWriteMode
                               ? "c97064"
                               : (viewIsFwdMode
                                  ? "d0d0d0"
                                  : "e0e0e0"))),
                        aView.resourceId,
                        aView.subjacentResource,
                        (viewIsReadMode
                            ? "Read"
                            : (viewIsWriteMode
                               ? "Write"
                               : (viewIsFwdMode
                                  ? "Forward"
                                  : "Accept"))),
                        convert_to_string(aView.format),
                        convert_to_string(aView.arraySliceRange),
                        convert_to_string(aView.mipSliceRange),
                        aView.referenceCount);

            mStream << "    " << viewId << " [" << viewStyle << ",label=" << viewLabel << "];\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CRenderGraphGraphVizSerializer::writeTextureResourceViewEdge(
                PassUID_t               const &aPassUID,
                SRenderGraphResource     const &aParentResource,
                SRenderGraphImageView  const &aView)
        {
            bool parentResourceIsTexture     = (aParentResource.type == ERenderGraphResourceType::Texture);
            bool parentResourceIsTextureView = (aParentResource.type == ERenderGraphResourceType::TextureView);
            bool viewIsReadMode              = aView.mode.check(ERenderGraphViewAccessMode::Read);
            bool viewIsWriteMode             = aView.mode.check(ERenderGraphViewAccessMode::Write);
            bool viewIsFwdMode               = aView.mode.check(ERenderGraphViewAccessMode::Forward);
            bool viewIsAckMode               = aView.mode.check(ERenderGraphViewAccessMode::Accept);

            std::string const passId   = StaticStringHelpers::format("Pass{}", aPassUID);
            std::string const viewId   = StaticStringHelpers::format("TextureView{}", aView.resourceId);
            std::string       parentId = "";

            if(parentResourceIsTexture)
            {
                parentId = StaticStringHelpers::format("Texture{}", aParentResource.resourceId);

            }
            else if(parentResourceIsTextureView)
            {
                parentId = StaticStringHelpers::format("TextureView{}", aParentResource.resourceId);
            }

            std::string color = "black";

            if(viewIsReadMode)
            {
                static constexpr char const*viewRead2PassEdgeStyle = "tailport=e,headport=w,weight=2,style=dashed";
                mStream << "    " << viewId << " -> " << passId << " [" << viewRead2PassEdgeStyle << ",color=" << color << "];\n";
            }
            else if(viewIsWriteMode)
            {
                static constexpr char const*pass2ViewWriteEdgeStyle = "tailport=e,headport=w,weight=2,style=dashed";
                mStream << "    " << passId << " -> " << viewId  << " [" << pass2ViewWriteEdgeStyle << ",color=" << color << "];\n";
            }
            else if(viewIsFwdMode)
            {
                static constexpr char const*pass2ViewForwardEdgeStyle = "tailport=e,headport=w,weight=2,style=dashed";
                mStream << "    " << passId << " -> " << viewId  << " [" << pass2ViewForwardEdgeStyle << ",color=" << color << "];\n";
            }
            else
            {
                static constexpr char const*viewAccept2PassEdgeStyle = "tailport=e,headport=w,weight=2,style=dashed";
                mStream << "    " << viewId << " -> " << passId << " [" << viewAccept2PassEdgeStyle << ",color=" << color << "];\n";
            }

            if(parentResourceIsTextureView)
            {
                static constexpr char const*parent2ViewStyle = "tailport=e,headport=w,weight=1,style=dashed";
                mStream << "    " << parentId << " -> " << viewId << " [" << parent2ViewStyle << ",color=" << color << "];\n";
            }
            else
            {
                static constexpr char const*parent2ViewStyle = "tailport=e,headport=w,weight=1,color=\"#bbbbbb\"";
                mStream << "    " << parentId << " -> " << viewId << " [" << parent2ViewStyle << "];\n";
            }
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
