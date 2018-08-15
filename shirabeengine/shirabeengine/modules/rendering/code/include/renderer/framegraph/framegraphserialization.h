#ifndef __SR_SHIRABE_FRAMEGRAPH_SERIALIZATION_H__
#define __SR_SHIRABE_FRAMEGRAPH_SERIALIZATION_H__

#include <sstream>
#include <optional>

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <core/serialization/graphvizdotserializer.h>

#include "renderer/framegraph/framegraphdata.h"

namespace engine
{
    // Forward declarations in proper namespaces
    namespace framegraph
    {
        class CGraph;
        class CPassBase;
        class SFrameGraphResource;
    }

    namespace serialization
    {
        using framegraph::CGraph;
        using framegraph::CPassBase;
        using framegraph::SFrameGraphResource;
        using framegraph::PassUID_t;
        using framegraph::SFrameGraphTexture;
        using framegraph::SFrameGraphTextureView;
        using framegraph::SFrameGraphRenderableList;
        using framegraph::SFrameGraphRenderableListView;
        using framegraph::FrameGraphResourceId_t;

        /**********************************************************************************************//**
     * \class IObjectSerializer
     *
     * \brief A value tree serializer.
     **************************************************************************************************/
        DeclareInterface(IFrameGraphSerializer);
        virtual bool serializeGraph(Graph const&) = 0;
        virtual bool serializePass(PassBase const&) = 0;
        virtual bool serializeResource(FrameGraphResource const&) = 0;

        DeclareInterfaceEnd(IFrameGraphSerializer);

        /**********************************************************************************************//**
     * \class IObjectDeserializer
     *
     * \brief A value tree deserializer.
     **************************************************************************************************/
        DeclareInterface(IFrameGraphDeserializer);
        virtual bool deserializeGraph(Graph &) = 0;
        virtual bool deserializePass(PassBase &) = 0;
        virtual bool deserializeResource(FrameGraphResource &) = 0;

        DeclareInterfaceEnd(IFrameGraphSerializer);

        /**********************************************************************************************//**
     * \class FrameGraphJSONSerializer
     *
     * \brief A frame graph JSON serializer.
     **************************************************************************************************/
        class SHIRABE_TEST_EXPORT FrameGraphGraphVizSerializer
                : public Serializer<IFrameGraphSerializer, IFrameGraphDeserializer>
                , public ISerializationResult
        {
            SHIRABE_DECLARE_LOG_TAG(FrameGraphGraphVizSerializer);

        public:
            bool initialize();
            bool deinitialize();

            bool serializeGraph(Graph const&);
            bool serializePass(PassBase const&);
            bool serializeResource(FrameGraphResource const&);

            bool deserializeGraph(Graph &);
            bool deserializePass(PassBase &);
            bool deserializeResource(FrameGraphResource &);

            bool writeToFile(std::string const&filename);

            std::string serializeResultToString();

        private:
            void beginGraph();
            void endGraph();
            void writePass(PassBase const&pass);
            void writeRenderableList(
                    FrameGraphRenderableList const&list);
            void writeRenderableListView(
                    FrameGraphResource           const&parentResource,
                    FrameGraphRenderableListView const&view);

            void writeRenderableResourceViewEdge(
                    FrameGraphResource           const&parentResource,
                    FrameGraphRenderableListView const&view);

            void writeTextureResource(
                    FrameGraphTexture      const&texture);
            void writeTextureResourceView(
                    FrameGraphResource     const&parentResource,
                    FrameGraphTextureView  const&view);

            void writePassEdge(PassUID_t const&source, PassUID_t const&target);
            void writePass2TextureResourceEdge(
                    FrameGraphTexture      const&texture);
            void writeTextureResourceViewEdge(
                    PassUID_t              const&passUID,
                    FrameGraphResource     const&parentResource,
                    FrameGraphTextureView const&view);

            std::stringstream m_stream;
        };
    }
}

#endif
