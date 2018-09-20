#include "renderer/framegraph/framegraphdata.h"
#include "renderer/framegraph/framegraph.h"
#include "renderer/framegraph/pass.h"
#include "renderer/framegraph/framegraphserialization.h"

namespace engine
{
    namespace serialization
    {
        using namespace engine::framegraph;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult::CFrameGraphSerializationResult(std::string const &aResult)
            : IResult()
            , mResult(aResult)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult::asString(std::string &aOutString) const
        {
            aOutString = mResult;
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult::asBinaryBuffer(std::vector<uint8_t> &aOutBuffer) const
        {
            aOutBuffer = std::vector<uint8_t>(mResult.begin(), mResult.end());
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CFrameGraphGraphVizSerializer::initialize()
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CFrameGraphGraphVizSerializer::deinitialize()
        {
            mStream.str("");
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        using FilterElementCompareCallbackFunction_t = std::function<bool(SFrameGraphResource const&)>;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        using FilterFunction_t = std::function<FrameGraphResourceIdList(FrameGraphResourceIdList const&, FilterElementCompareCallbackFunction_t const&)>;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        bool CFrameGraphGraphVizSerializer::serialize(
                CGraph                   const &aSource,
                CStdSharedPtr_t<IResult>       &aOutResult)
        {
            CStdSharedPtr_t<CFrameGraphSerializationResult> result = nullptr;

            bool const serialized = serializeGraph(aSource);
            if(serialized)
            {
                result = makeCStdSharedPtr<CFrameGraphSerializationResult>(mStream.str());
                mStream.str(std::string());
            }

            aOutResult = result;

            return serialized;
        }
        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CFrameGraphGraphVizSerializer::serializeGraph(CGraph const &aGraph)
        {
            CStdUniquePtr_t<CGraph::CAccessor> accessor = aGraph.getAccessor(CPassKey<CFrameGraphGraphVizSerializer>());

            CFrameGraphResources const &resources = accessor->resourceData();

            FilterFunction_t const filter = [&](
                    FrameGraphResourceIdList               const &aUIDs,
                    FilterElementCompareCallbackFunction_t const &aCallback) -> FrameGraphResourceIdList
            {
                FrameGraphResourceIdList output{ };
                if(aUIDs.empty())
                    return output;

                std::copy_if(
                            aUIDs.begin(),
                            aUIDs.end(),
                            std::back_inserter(output),
                            [&] (FrameGraphResourceId_t const &aId) -> bool
                {
                    if(resources.resources().size() <= aId)
                        return false;

                    if(aCallback)
                        return aCallback(*resources.get<SFrameGraphResource>(aId));

                    return false;
                });

                return output;
            };

            beginGraph();

            // Write registered resources
            for(RefIndex_t::value_type const &textureRef : resources.textures())
            {
                SFrameGraphTexture const &texture = *resources.get<SFrameGraphTexture>(textureRef);
                writeTextureResource(texture);
            }

            for(RefIndex_t::value_type const &textureViewRef : resources.textureViews())
            {
                SFrameGraphTextureView const &view   = *resources.get<SFrameGraphTextureView>(textureViewRef);
                SFrameGraphResource    const &parent = *resources.get<SFrameGraphResource>(view.parentResource);

                writeTextureResourceView(parent, view);
            }

            for(RefIndex_t::value_type const &renderableListRef : resources.renderablesLists())
            {
                SFrameGraphRenderableList const &list = *resources.get<SFrameGraphRenderableList>(renderableListRef);

                writeRenderableList(list);
            }

            for(RefIndex_t::value_type const &renderableListViewRef : resources.renderableListViews())
            {
                SFrameGraphRenderableListView const &view   = *resources.get<SFrameGraphRenderableListView>(renderableListViewRef);
                SFrameGraphResource           const &parent = *resources.get<SFrameGraphResource>(view.parentResource);
                writeRenderableListView(parent, view);
            }

            // Write passes and adjacent resources
            AdjacencyListMap_t<PassUID_t>                         const &passAdjacency    = accessor->passAdjacency();
            AdjacencyListMap_t<PassUID_t, FrameGraphResourceId_t> const &passResourcesAdj = accessor->passToResourceAdjacency();

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
                    mStream << CString::format("\n  subgraph pass%0 {\n", sourceUID);

                    CStdSharedPtr_t<CPassBase> sourcePass = aGraph.passes().at(sourceUID);

                    sourcePass->acceptSerializer(*this);

                    // Write out the passes' resources
                    if(passResourcesAdj.find(sourceUID) != passResourcesAdj.end())
                    {
                        FrameGraphResourceIdList const &passResources = passResourcesAdj.at(sourceUID);

                        // Create Texture
                        auto textureCreateFilterFn = [] (SFrameGraphResource const &aInput) -> bool
                        {
                                return (aInput.type == EFrameGraphResourceType::Texture /* && aInput.assignedPassUID != 0 */);
                        };
                        std::vector<FrameGraphResourceId_t> const creations = filter(passResources, textureCreateFilterFn);

                        if(!creations.empty())
                        {
                            for(FrameGraphResourceId_t const &id : creations)
                            {
                                SFrameGraphTexture const &texture = *resources.get<SFrameGraphTexture>(id);
                                writePass2TextureResourceEdge(texture);
                            }
                        }

                        // Read/Write Texture
                        auto const textureViewfilterFn = [] (SFrameGraphResource const &aInput) -> bool
                        {
                            return (aInput.type == EFrameGraphResourceType::TextureView);
                        };
                        std::vector<FrameGraphResourceId_t> readViews = filter(passResources, textureViewfilterFn);

                        if(!readViews.empty())
                        {
                            for(FrameGraphResourceId_t const &id : readViews)
                            {
                                SFrameGraphTextureView const &view           = *resources.get<SFrameGraphTextureView>(id);
                                SFrameGraphResource    const &parentResource = *resources.get<SFrameGraphResource>(view.parentResource);

                                writeTextureResourceViewEdge(sourceUID, parentResource, view);
                            }
                        }

                        // Use Renderables
                        auto const renderableFilterFn = [] (SFrameGraphResource const &aInput) -> bool
                        {
                            return (aInput.type == EFrameGraphResourceType::RenderableListView);
                        };
                        std::vector<FrameGraphResourceId_t> renderableListViews = filter(passResources, renderableFilterFn);

                        if(!renderableListViews.empty())
                        {
                            for(FrameGraphResourceId_t const &id : renderableListViews)
                            {
                                SFrameGraphRenderableListView const&view           = *resources.get<SFrameGraphRenderableListView>(id);
                                SFrameGraphResource           const&parentResource = *resources.get<SFrameGraphResource>(view.parentResource);

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
        bool CFrameGraphGraphVizSerializer::serializePass(CPassBase const &aPass)
        {
            writePass(aPass);

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CFrameGraphGraphVizSerializer::serializeResource(SFrameGraphResource const &aResource)
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CFrameGraphGraphVizDeserializer::deserializeGraph(CGraph &aGraph)
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CFrameGraphGraphVizDeserializer::deserializePass(CPassBase &aPass)
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CFrameGraphGraphVizDeserializer::deserializeResource(SFrameGraphResource &aResource)
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CFrameGraphGraphVizSerializer::beginGraph()
        {
            mStream
                    << "strict digraph FrameGraph {\n"
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
        void CFrameGraphGraphVizSerializer::endGraph()
        {
            mStream << "}";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CFrameGraphGraphVizSerializer::writePass(CPassBase const &aPass)
        {
            std::string const passLabel = CString::format(
                        "<<table bgcolor=\"#429692\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
                        "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"18\"><b>Pass (PID: %0)</b></font></td></tr>"
                        "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"16\">%1</font></td></tr>"
                        "</table>>",
                        aPass.passUID(),
                        aPass.passName());

            mStream << "    Pass" << aPass.passUID() << " [shape=none, label=" << passLabel << "];\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CFrameGraphGraphVizSerializer::writePassEdge(
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
        void CFrameGraphGraphVizSerializer::writeRenderableList(
                SFrameGraphRenderableList const &aList)
        {
            static constexpr char const*listStyle = "shape=none";
            std::string const listLabel =
                    CString::format(
                        "<<table bgcolor=\"#edb036\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
                        "<tr><td colspan=\"2\"><font point-size=\"16\">&lt;&lt;%0&gt;&gt;</font></td></tr>"
                        "<tr><td colspan=\"2\" height=\"20\"><font point-size=\"18\"><b>RenderableList (RID: %1)</b></font></td></tr>"
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
        void CFrameGraphGraphVizSerializer::writeRenderableListView(
                SFrameGraphResource           const &aParentResource,
                SFrameGraphRenderableListView const &aView)
        {
            std::string viewId = CString::format("RenderableListView%0", aView.resourceId);

            static constexpr char const*viewStyle = "shape=none";
            std::string viewLabel =
                    CString::format(
                        "<<table bgcolor=\"#%0\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
                        "<tr><td colspan=\"2\"><font point-size=\"16\"><b>RenderableListView (RID: %1)</b></font></td></tr>"
                        "<tr><td align=\"left\">SubjacentResourceId:</td><td align=\"left\">%2</td></tr>"
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
        void CFrameGraphGraphVizSerializer::writeRenderableResourceViewEdge(
                SFrameGraphResource           const &aParentResource,
                SFrameGraphRenderableListView const &aView)
        {
            std::string const passId   = CString::format("Pass%0", aView.assignedPassUID);
            std::string const viewId   = CString::format("RenderableListView%0", aView.resourceId);
            std::string       parentId = "";

            if(aParentResource.type == EFrameGraphResourceType::RenderableList)
            {
                parentId = CString::format("RenderableList%0", aParentResource.resourceId);
            }
            else
            {
                parentId = CString::format("RenderableListView%0", aParentResource.resourceId);
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
        void CFrameGraphGraphVizSerializer::writeTextureResource(SFrameGraphTexture const &aTexture)
        {
            std::string mode = "create";
            if(aTexture.assignedPassUID == 0)
                mode = "import";

            static constexpr char const *textureStyle = "shape=none";
            std::string const textureLabel =
                    CString::format(
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
                        aTexture.resourceId,
                        aTexture.readableName,
                        aTexture.width, aTexture.height, aTexture.depth,
                        to_string(aTexture.format),
                        aTexture.arraySize,
                        aTexture.mipLevels,
                        to_string(aTexture.initialState),
                        aTexture.referenceCount);

            mStream << "    Texture" << aTexture.resourceId << " [" << textureStyle << ",label=" << textureLabel << "];\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CFrameGraphGraphVizSerializer::writePass2TextureResourceEdge(SFrameGraphTexture const &aTexture)
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
        void CFrameGraphGraphVizSerializer::writeTextureResourceView(
                SFrameGraphResource     const &aParentResource,
                SFrameGraphTextureView  const &aView)
        {
            bool const viewIsReadMode  = aView.mode.check(EFrameGraphViewAccessMode::Read);
            bool const viewIsWriteMode = aView.mode.check(EFrameGraphViewAccessMode::Write);
            bool const viewIsFwdMode   = aView.mode.check(EFrameGraphViewAccessMode::Forward);

            std::string viewId   = CString::format("TextureView%0", aView.resourceId);

            static constexpr char const *viewStyle = "shape=none";
            std::string const viewLabel =
                    CString::format(
                        "<<table bgcolor=\"#%0\" style=\"rounded\" border=\"0\" cellspacing=\"1\" cellpadding=\"5\">"
                        "<tr><td colspan=\"2\"><font point-size=\"16\"><b>TextureView (RID: %1)</b></font></td></tr>"
                        "<tr><td align=\"left\">SubjacentResourceId:</td><td align=\"left\">%2</td></tr>"
                        "<tr><td align=\"left\">Mode:</td><td align=\"left\">%3</td></tr>"
                        "<tr><td align=\"left\">Format:</td><td align=\"left\">%4</td></tr>"
                        "<tr><td align=\"left\">ArrayRange:</td><td align=\"left\">%5</td></tr>"
                        "<tr><td align=\"left\">MipRange:</td><td align=\"left\">%6</td></tr>"
                        "<tr><td align=\"left\">Reference-Count:</td><td align=\"left\">%7</td></tr>"
                        "</table>>",
                        (viewIsReadMode
                            ? "68a357"
                            : (viewIsWriteMode
                               ? "c97064"
                               : "e0e0e0" )),
                        aView.resourceId,
                        aView.subjacentResource,
                        (viewIsReadMode
                            ? "Read"
                            : (viewIsWriteMode
                               ? "Write"
                               : "Forward" )),
                        to_string(aView.format),
                        to_string(aView.arraySliceRange),
                        to_string(aView.mipSliceRange),
                        aView.referenceCount);

            mStream << "    " << viewId << " [" << viewStyle << ",label=" << viewLabel << "];\n";
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CFrameGraphGraphVizSerializer::writeTextureResourceViewEdge(
                PassUID_t               const &aPassUID,
                SFrameGraphResource     const &aParentResource,
                SFrameGraphTextureView  const &aView)
        {
            bool parentResourceIsTexture     = (aParentResource.type == EFrameGraphResourceType::Texture);
            bool parentResourceIsTextureView = (aParentResource.type == EFrameGraphResourceType::TextureView);
            bool viewIsReadMode              = aView.mode.check(EFrameGraphViewAccessMode::Read);
            bool viewIsWriteMode             = aView.mode.check(EFrameGraphViewAccessMode::Write);

            std::string const passId   = CString::format("Pass%0", aPassUID);
            std::string const viewId   = CString::format("TextureView%0", aView.resourceId);
            std::string       parentId = "";

            if(parentResourceIsTexture)
            {
                parentId = CString::format("Texture%0", aParentResource.resourceId);

            }
            else if(parentResourceIsTextureView)
            {
                parentId = CString::format("TextureView%0", aParentResource.resourceId);
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
            else
            {
                static constexpr char const*pass2ViewForwardEdgeStyle = "tailport=e,headport=w,weight=2,style=dashed";
                mStream << "    " << passId << " -> " << viewId  << " [" << pass2ViewForwardEdgeStyle << ",color=" << color << "];\n";
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
