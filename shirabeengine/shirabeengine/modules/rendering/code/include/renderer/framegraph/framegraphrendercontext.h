#ifndef __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__
#define __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <asset/assetstorage.h>

#include "renderer/renderertypes.h"
#include "renderer/framegraph/framegraphdata.h"

namespace engine
{
    namespace resources
    {
        template <typename... TResources>
        class CResourceManager;
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
        using engine::asset::AssetId_t;
        using engine::asset::IAssetStorage;
        using engine::material::CMaterialLoader;
        using namespace engine::resources;
        using namespace engine::rendering;


        /**
         * Describes the runtime state of render context operations, used
         * in conjunction with a SFrameGraphRenderContext-instance.
         */
        struct SHIRABE_LIBRARY_EXPORT SFrameGraphRenderContextState
        {
            // std::vector<ResourceId_t> mReferencedResources;
            uint32_t currentSubpassIndex;

            SHIRABE_INLINE
            SFrameGraphRenderContextState()
                : currentSubpassIndex(0)
            {}
        };

        /**
         *
         */
        struct SHIRABE_LIBRARY_EXPORT SFrameGraphRenderContext
        {
            /**
             * Copy one image into another.
             *
             * @param aSourceTexture Texture to copy from
             * @param aTargetTexture Texture to copy to
             * @returns EEngineStatus::Ok    If successful
             * @returns EEngineStatus::Error On error
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState  & /* aState         */
                                      , SFrameGraphDynamicTexture const& /* aSourceTexture */
                                      , SFrameGraphDynamicTexture const& /* aTargetTexture */)> copyImage;
            /**
             * Copy aTexture to the currently bound back buffer.
             *
             * @param aTexture Texture to copy from
             * @returns EEngineStatus::Ok    If successful
             * @returns EEngineStatus::Error On error
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &  /* aState */
                                      , SFrameGraphDynamicTexture const& /* aTexture */)> copyImageToBackBuffer;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState      & /* aState        */
                                      , SFrameGraphDynamicTexture     const& /* aImageHandle  */
                                      , CRange                        const& /* aArrayRange   */
                                      , CRange                        const& /* aMipRange     */
                                      , VkImageAspectFlags            const& /* aAspectFlags  */
                                      , VkImageLayout                 const& /* aSourceLayout */
                                      , VkImageLayout                 const& /* aTargetLayout */)> performImageLayoutTransfer;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState       & /* aState */)> beginGraphicsFrame;
            std::function<EEngineStatus(SFrameGraphRenderContextState       & /* aState */)> endGraphicsFrame;
            std::function<EEngineStatus(SFrameGraphRenderContextState       & /* aState */)> beginPass;
            std::function<EEngineStatus(SFrameGraphRenderContextState       & /* aState */)> endPass;
            std::function<EEngineStatus(SFrameGraphRenderContextState       & /* aState */)> beginFrameCommandBuffers;
            std::function<EEngineStatus(SFrameGraphRenderContextState       & /* aState */)> endFrameCommandBuffers;
            std::function<EEngineStatus(SFrameGraphRenderContextState       & /* aState */
                                      , std::string                   const &)>              clearAttachments;
            std::function<EEngineStatus(SFrameGraphRenderContextState       & /* aState */)> present;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState    & /* aState               */
                                      , std::string                 const& /* aRenderPassId        */
                                      , std::string                 const& /* aFrameBufferId       */
                                      , CFrameGraphMutableResources const& /* aFrameGraphResources */)> bindRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState         */
                                      , std::string              const& /* aRenderPassId  */
                                      , std::string              const& /* aFrameBufferId */)> unbindRenderPass;

            std::function<EEngineStatus(SFrameGraphRenderContextState& /* aState */, SFrameGraphMesh const& /* aMesh */)> bindMesh;
            std::function<EEngineStatus(SFrameGraphRenderContextState& /* aState */, SFrameGraphMesh const& /* aMesh */)> unbindMesh;

            std::function<EEngineStatus(SFrameGraphRenderContextState &   /* aState */
                                      , SFrameGraphBuffer const &         /* aBuffer */
                                      , std::vector<VkDeviceSize> const & /* aOffsets */)> bindVertexBuffer;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */
                                      , SFrameGraphBuffer const &       /* aBuffer */
                                      , VkDeviceSize const &            /* aIndexCount */)> bindIndexBuffer;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphPipeline const& /* aPipelineUID */)> bindPipeline;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphPipeline const& /* aPipelineUID */)> unbindPipeline;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, VkDeviceSize const& /* aIndexCount */)> drawIndexed;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */)> drawQuad;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMaterial const& /* aMaterial */)>  drawFullscreenQuadWithMaterial;
        };

    }
}

#endif
