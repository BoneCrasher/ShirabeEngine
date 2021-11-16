#ifndef __SR_SHIRABE_FRAMEGRAPH_SERIALIZATION_H__
#define __SR_SHIRABE_FRAMEGRAPH_SERIALIZATION_H__

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION

#include <sstream>
#include <optional>

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <core/serialization/serialization.h>

#include "renderer/rendergraph/framegraphdata.h"

namespace engine
{
    // Forward declarations in proper namespaces
    namespace rendergraph
    {
        class  CGraph;
        class  CPassBase;
        struct SRenderGraphResource;
    }

    namespace serialization
    {
        using rendergraph::CGraph;
        using rendergraph::CPassBase;
        using rendergraph::SRenderGraphResource;
        using rendergraph::PassUID_t;
        using rendergraph::SRenderGraphTexture;
        using rendergraph::SRenderGraphImageView;
        using rendergraph::SRenderGraphRenderableList;
        using rendergraph::SRenderGraphRenderableListView;
        using rendergraph::RenderGraphResourceId_t;

        /**
         * The IRenderGraphSerializer interface describes the basic requiremets
         * to serialize a rendergraph instance.
         */
        class IRenderGraphSerializer
                : public ISerializer<CGraph>
        {
            SHIRABE_DECLARE_INTERFACE(IRenderGraphSerializer);

        public_api:
            /**
             * Serialize the graph itself. This is the main entry point of serialization.
             * This call will invoke serializePass and serializeResource serveral times.
             *
             * @param aGraph The rendergraph instance to serialize.
             * @return       True, if successful. False otherwise.
             */
            virtual bool serializeGraph(CGraph const &aGraph) = 0;
            /**
             * Serialize a single pass of a graph.
             *
             * @param aPass The pass to serialize.
             * @return      True, if successful. False otherwise.
             */
            virtual bool serializePass(CPassBase const &aPass) = 0;
            /**
             * Serialize a rendergraph resource.
             *
             * @param aResource The resource to serialize.
             * @return          True, if successful. False otherwise.
             */
            virtual bool serializeResource(SRenderGraphResource const &aResource) = 0;
        };

        /**
         * The IRenderGraphDeserializer interface describes the basic requirements
         * to deserialize to a rendergraph instance.
         */
        class IRenderGraphDeserializer
                : public IDeserializer<CGraph>
        {
            SHIRABE_DECLARE_INTERFACE(IRenderGraphDeserializer)

        public_api:
            virtual bool deserializeGraph(CGraph &aOutGraph) = 0;
            virtual bool deserializePass(CPassBase &aOutPass) = 0;
            virtual bool deserializeResource(SRenderGraphResource &aOutResource) = 0;
        };

        /**
         * The RenderGraphGraphVizSerializer class implements rendergraph serialization
         * to the graphviz dot format.
         */
        class SHIRABE_TEST_EXPORT CRenderGraphGraphVizSerializer
                : public IRenderGraphSerializer
        {
            SHIRABE_DECLARE_LOG_TAG(CRenderGraphGraphVizSerializer);

        public_structs:
            /*!
             * The IResult interface of the ISerializer<T> interface declares required
             * signatures for result retrieval from a serialization process.
             */
            class CRenderGraphSerializationResult
                    : public ISerializer<CGraph>::IResult
            {
            public_constructors:
                CRenderGraphSerializationResult(std::string const &aResult);

            public_methods:
                bool asString      (std::string          &aOutString) const;
                bool asBinaryBuffer(std::vector<uint8_t> &aOutBuffer) const;

            private_members:
                std::string const mResult;
            };

        public_methods:
            /**
             * Initialize the serializer and prepare for serialization calls.
             *
             * @return True, if successful. False otherwise.
             */
            bool initialize();

            /**
             * Cleanup and shutdown...
             *
             * @return True, if successful. False otherwise.
             */
            bool deinitialize();            

            /*!
             * Serialize an instance of type CGraph into whichever internal representation and
             * provide it using a pointer to IResult.
             *
             * @param aSource    Input data for serialization.
             * @param aOutResult Result-Instance holding the serialized data, providing access to
             *                   it in various output formats.
             * @return
             */
            bool serialize(CGraph const &aSource, Shared<IResult> &aOutResult);

            /**
             * Serialize the graph itself. This is the main entry point of serialization.
             * This call will invoke serializePass and serializeResource serveral times.
             *
             * @param aGraph The rendergraph instance to serialize.
             * @return       True, if successful. False otherwise.
             */
            bool serializeGraph(CGraph const &aGraph);
            /**
             * Serialize a single pass of a graph.
             *
             * @param aPass The pass to serialize.
             * @return      True, if successful. False otherwise.
             */
            bool serializePass(CPassBase const &aPass);
            /**
             * Serialize a rendergraph resource.
             *
             * @param aResource The resource to serialize.
             * @return          True, if successful. False otherwise.
             */
            bool serializeResource(SRenderGraphResource const &aResource);

        private_methods:
            /**
             * Begin writing the rendergraph, writing out some header information and style data for
             * the dot layout engine.
             */
            void beginGraph();
            /**
             * Finalize the dot graph.
             */
            void endGraph();
            /**
             * Write a pass instance in dot format.
             *
             * @param aPass The pass to write.
             */
            void writePass(CPassBase const &aPass);
            /**
             * Write out a list of renderables in dot format.
             *
             * @param aList The list of renderables to write.
             */
            void writeRenderableList(
                    SRenderGraphRenderableList const &aList);
            /**
             * Write out a renderable list view in dot format.
             *
             * @param aParentResource The parent renderable list of the view.
             * @param aView           The view instance to write out.
             */
            void writeRenderableListView(
                    SRenderGraphResource           const &aParentResource,
                    SRenderGraphRenderableListView const &aView);

            /**
             * Write a renderable list view edge, either outgoing from a pass or ingoing into a pass.
             *
             * @param aParentResource The parent resource of the view.
             * @param aView           The renderable list view to write.
             */
            void writeRenderableResourceViewEdge(
                    SRenderGraphResource           const &aParentResource,
                    SRenderGraphRenderableListView const &aView);

            /**
             * Write out a texture resource in dot format.
             *
             * @param aTexture The texture resource to write.
             */
            void writeTextureResource(
                    SRenderGraphImage const &aTexture);

            /**
             * Write out a texture view in dot format.
             *
             * @param aParentResource The parent resource of the view.
             * @param aView           The resource view to write.
             */
            void writeTextureResourceView(
                    SRenderGraphResource     const &aParentResource,
                    SRenderGraphImageView  const &aView);

            /**
             * Write a graph edge inbetween two passes.
             *
             * @param aSource Source pass UID.
             * @param aTarget Target pass UID.
             */
            void writePassEdge(
                    PassUID_t const &aSource,
                    PassUID_t const &aTarget);
            /**
             * Write out an edge from pass to a texture created in the pass.
             *
             * @param aTexture The texture being created by the pass.
             */
            void writePass2TextureResourceEdge(SRenderGraphImage const &aTexture);

            /**
             * Write an edge connecting a pass and a texture view, either in- or outgoing.
             *
             * @param aPassUID        The pass, to which a resource op is attached.
             * @param aParentResource The parent texture resource of the view.
             * @param aView           The texture view to connect.
             */
            void writeTextureResourceViewEdge(
                    PassUID_t              const &aPassUID,
                    SRenderGraphResource    const &aParentResource,
                    SRenderGraphImageView const &aView);

        private_members:
            std::stringstream mStream;
        };

        /**
         * @brief The RenderGraphGraphVizDeserializer class
         */
        class SHIRABE_TEST_EXPORT CRenderGraphGraphVizDeserializer
                : public IRenderGraphDeserializer
        {
            SHIRABE_DECLARE_LOG_TAG(CRenderGraphGraphVizDeserializer);

        public_methods:
            bool initialize();
            bool deinitialize();

            bool deserializeGraph(CGraph &aOutGraph);
            bool deserializePass(CPassBase &aOutPass);
            bool deserializeResource(SRenderGraphResource &aOutResource);

        private_methods:
            std::stringstream m_stream;
        };
    }
}

#endif
#endif