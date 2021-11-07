#ifndef __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__
#define __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__

#include <core/enginestatus.h>
#include <functional>
#include <log/log.h>
#include <core/enginetypehelper.h>
#include <asset/assetstorage.h>

#include "rhi/resource_management/extensibility.h"
#include "renderer/renderertypes.h"
#include "renderer/rendergraph/rendergraphdata.h"

namespace engine
{
    namespace resources
    {
        template <typename... TResources>
        class CRHIResourceManagerBase;
    }

    namespace material
    {
        class CMaterialLoader;
    }

    namespace rendering
    {
        class IRenderContext;
    }

    namespace framegraph
    {
        using namespace core;
        using engine::asset::AssetId_t;
        using engine::asset::IAssetStorage;
        using engine::material::CMaterialLoader;
        using namespace engine::rhi;
        using namespace engine::rendering;


        /**
         * Describes the runtime state of render context operations, used
         * in conjunction with a SRenderGraphRenderContext-instance.
         */
        struct SHIRABE_LIBRARY_EXPORT SRenderGraphRenderContextState
        {
            // std::vector<ResourceId_t> mReferencedResources;
            uint32_t currentSubpassIndex;

            SHIRABE_INLINE
            SRenderGraphRenderContextState()
                : currentSubpassIndex(0)
            {}
        };

        /**
         *
         */
        struct SHIRABE_LIBRARY_EXPORT SRenderGraphRenderContext
        {
            /**
             * Copy one image into another.
             *
             * @param aSourceTexture Texture to copy from
             * @param aTargetTexture Texture to copy to
             * @returns EEngineStatus::Ok    If successful
             * @returns EEngineStatus::Error On error
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState         */
                                      , SRenderGraphImage const        &/* aSourceTexture */
                                      , SRenderGraphImage const        &/* aTargetTexture */)> copyImage;
            /**
             * Copy aTexture to the currently bound back buffer.
             *
             * @param aTexture Texture to copy from
             * @returns EEngineStatus::Ok    If successful
             * @returns EEngineStatus::Error On error
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState  &/* aState */
                                        , SRenderGraphImage const       &/* aTexture */)> copyImageToBackBuffer;

            /**
             * Transfers an image from one layout into another.
             * Important: If this call is made within a renderpass, a subpass-self-dependency is required!
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState        */
                                      , SRenderGraphImage const        &/* aImageHandle  */
                                      , CRange const                   &/* aArrayRange   */
                                      , CRange const                   &/* aMipRange     */
                                      , VkImageAspectFlags const       &/* aAspectFlags  */
                                      , VkImageLayout const            &/* aSourceLayout */
                                      , VkImageLayout const            &/* aTargetLayout */)> performImageLayoutTransfer;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */)> beginGraphicsFrame;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */)> endGraphicsFrame;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */)> beginSubpass;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */)> nextSubpass;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */)> beginFrameCommandBuffers;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */)> endFrameCommandBuffers;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */
                                      , std::string             const &)> clearAttachments;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */)> present;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState     &/* aState               */
                                      , ResourceId_t                const &/* aRenderPassId        */
                                      , ResourceId_t                const &/* aFrameBufferId       */)> bindRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState & /* aState         */
                                      , std::string              const& /* aRenderPassId  */
                                      , std::string              const& /* aFrameBufferId */)> unbindRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */
                                      , SRenderGraphMesh          const&/* aMesh */)> useMesh;
            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */
                                      , SRenderGraphMaterial     const &/* aMaterial */
                                      , SRenderGraphPipeline     const &/* aPipeline */)> useMaterialWithPipeline;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */
                                      , SRenderGraphPipeline     const &/* aPipelineUID */)> bindPipeline;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */
                                      , VkDeviceSize            const &/* aIndexCount */)> drawIndexed;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */)> drawQuad;

            /**
             *
             */
            std::function<EEngineStatus(SRenderGraphRenderContextState &/* aState */, SRenderGraphMaterial const& /* aMaterial */)>  drawFullscreenQuadWithMaterial;
        };

        struct SHIRABE_LIBRARY_EXPORT SRenderGraphResourceContext
        {
            //---------------------------------------------------------------------------------------------------------------
            // RenderPass
            //---------------------------------------------------------------------------------------------------------------
            /**
             *
             */
            std::function<EEngineStatus(ResourceId_t                      const& /* aRenderPassId        */
                                        , std::vector<PassUID_t>          const& /* aPassExecutionOrder  */
                                        , SRenderGraphAttachmentCollection const& /* aAttachmentInfo      */
                                        , CRenderGraphMutableResources     const& /* aRenderGraphResources */)> createRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(ResourceId_t const& /* aRenderPass */)> destroyRenderPass;

            //---------------------------------------------------------------------------------------------------------------
            // FrameBuffer
            //---------------------------------------------------------------------------------------------------------------
            /**
             *
             */
            std::function<EEngineStatus(ResourceId_t   const& /* aFrameBufferId */
                                        , ResourceId_t const& /* aRenderPassId  */)> createFrameBuffer;

            /**
             *
             */
            std::function<EEngineStatus(std::string const& /* aFrameBufferId */)> destroyFrameBuffer;

            //---------------------------------------------------------------------------------------------------------------
            // Textures
            //---------------------------------------------------------------------------------------------------------------
            std::function<EEngineStatus(SRenderGraphImage const& /* aTexture */)> createTransientTexture;
            std::function<EEngineStatus(SRenderGraphImage const& /* aTexture */)> destroyTransientTexture;

            std::function<EEngineStatus(SRenderGraphImage const &/* aTexture */)> initializePersistentImage;
            std::function<EEngineStatus(SRenderGraphImage const &/* aTexture */)> updatePersistentTexture;
            std::function<EEngineStatus(SRenderGraphImage const &/* aTexture */)> deinitializePersistentTexture;

            std::function<EEngineStatus(SRenderGraphImage      const &/* aTextureName */
                                        , SRenderGraphImageView const& /* aTextureView */)> createTextureView;
            std::function<EEngineStatus(SRenderGraphImageView const& /* aTextureView */)>   destroyTextureView;

            //---------------------------------------------------------------------------------------------------------------
            // Buffers
            //---------------------------------------------------------------------------------------------------------------
            std::function<EEngineStatus(SRenderGraphBuffer const& /* aBuffer */)> createTransientBuffer;
            std::function<EEngineStatus(SRenderGraphBuffer const& /* aBuffer */)> destroyTransientBuffer;

            std::function<EEngineStatus(SRenderGraphBuffer const& /* aBuffer */)> initializePersistentBuffer;
            std::function<EEngineStatus(SRenderGraphBuffer const& /* aBuffer */)> updatePersistentBuffer;
            std::function<EEngineStatus(SRenderGraphBuffer const& /* aBuffer */)> deinitializePersistentBuffer;

            std::function<EEngineStatus(SRenderGraphBuffer        const& /* aBuffer     */
                                        , SRenderGraphBufferView  const& /* aBufferView */)> createBufferView;
            std::function<EEngineStatus(SRenderGraphBufferView const& /* aBufferView */)> destroyBufferView;

            //---------------------------------------------------------------------------------------------------------------
            // Meshes
            //---------------------------------------------------------------------------------------------------------------
            std::function<EEngineStatus(SRenderGraphMesh const &/* aMesh */)> initializeMesh;
            std::function<EEngineStatus(SRenderGraphMesh const &/* aMesh */)> updateMesh;
            std::function<EEngineStatus(SRenderGraphMesh const &/* aMesh */)> deinitializeMesh;

            //---------------------------------------------------------------------------------------------------------------
            // Materials
            //---------------------------------------------------------------------------------------------------------------
            std::function<EEngineStatus(SRenderGraphMaterial const &/* aMaterial */)> initializeMaterial;
            std::function<EEngineStatus(SRenderGraphMaterial const &/* aMaterial */)> updateMaterial;
            std::function<EEngineStatus(SRenderGraphMaterial const &/* aMaterial */)> deinitializeMaterial;

            //---------------------------------------------------------------------------------------------------------------
            // Pipelines
            //---------------------------------------------------------------------------------------------------------------
            std::function<EEngineStatus(SRenderGraphPipeline const     &/* aPipeline */
                                       , resources::ResourceId_t const &/* aRenderPassResourceId */
                                       , uint32_t const                &/* aSubpassIndex*/)> createPipeline;
            std::function<EEngineStatus(SRenderGraphPipeline const &/* aPipeline */)> destroyPipeline;
        };

    }
}


#endif
