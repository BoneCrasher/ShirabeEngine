#ifndef __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__
#define __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <asset/assetstorage.h>
#include <resources/ilogicalresourceobject.h>

#include "renderer/irendercontext.h"
#include "renderer/renderertypes.h"
#include "renderer/framegraph/framegraphdata.h"
#include "renderer/framegraph/iframegraphrendercontext.h"

namespace engine
{
    namespace resources
    {
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

        struct SSampledImageBinding
        {
            SFrameGraphTextureView imageView;
            SFrameGraphTexture     image;
        };

        /**
         * Describes the runtime state of render context operations, used
         * in conjunction with a SFrameGraphRenderContext-instance.
         */
        struct SHIRABE_LIBRARY_EXPORT SFrameGraphRenderContextState
        {
            std::vector<ResourceId_t> mReferencedResources;

        };

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
            std::function<EEngineStatus(SFrameGraphTexture const& /* aSourceTexture */
                                      , SFrameGraphTexture const& /* aTargetTexture */)> copyImage;
            /**
             * Copy aTexture to the currently bound back buffer.
             *
             * @param aTexture Texture to copy from
             * @returns EEngineStatus::Ok    If successful
             * @returns EEngineStatus::Error On error
             */
            std::function<EEngineStatus(SFrameGraphTexture const& /* aTexture */)> copyImageToBackBuffer;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphTexture const& /* aImageHandle  */
                                      , CRange             const& /* aArrayRange   */
                                      , CRange             const& /* aMipRange     */
                                      , VkImageAspectFlags const& /* aAspectFlags  */
                                      , VkImageLayout      const& /* aSourceLayout */
                                      , VkImageLayout      const& /* aTargetLayout */)> performImageLayoutTransfer;

            /**
             *
             */
            std::function<EEngineStatus()>                                    beginGraphicsFrame;
            std::function<EEngineStatus()>                                    endGraphicsFrame;
            std::function<EEngineStatus()>                                    beginPass;
            std::function<EEngineStatus()>                                    endPass;
            std::function<EEngineStatus()>                                    beginFrameCommandBuffers;
            std::function<EEngineStatus()>                                    endFrameCommandBuffers;
            std::function<EEngineStatus(std::string const&, uint32_t const&)> clearAttachments;
            std::function<EEngineStatus()>                                    present;

            /**
             *
             */
            std::function<EEngineStatus(std::string                     const& /* aRenderPassId        */
                                      , std::vector<PassUID_t>          const& /* aPassExecutionOrder  */
                                      , SFrameGraphAttachmentCollection const& /* aAttachmentInfo      */
                                      , CFrameGraphMutableResources     const& /* aFrameGraphResources */)> createRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(std::string                     const& /* aRenderPassId        */
                                      , std::string                     const& /* aFrameBufferId       */
                                      , std::vector<PassUID_t>          const& /* aPassExecutionOrder  */
                                      , SFrameGraphAttachmentCollection const& /* aAttachmentInfo      */
                                      , CFrameGraphMutableResources     const& /* aFrameGraphResources */)> bindRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(std::string const& /* aRenderPassId */
                                      , std::string const& /* aFrameBufferId */)> unbindRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(std::string const& /* aRenderPass */)> destroyRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(std::string const& /* aFrameBufferId */
                                      , std::string const& /* aRenderPassId  */)> createFrameBuffer;

            /**
             *
             */
            std::function<EEngineStatus(std::string const& /* aFrameBufferId */)> destroyFrameBuffer;

            std::function<EEngineStatus(AssetId_t const& /* aAssetUid */)>                 loadTextureAsset;
            std::function<EEngineStatus(AssetId_t const& /* aAssetUid */)>                 unloadTextureAsset;

            std::function<EEngineStatus(SFrameGraphTexture const& /* aTexture */)>         importTexture;
            std::function<EEngineStatus(SFrameGraphTexture const& /* aTexture */)>         createTexture;
            std::function<EEngineStatus(SFrameGraphTexture const& /* aTexture */)>         transferTextureData;
            std::function<EEngineStatus(SFrameGraphTexture const& /* aTexture */)>         destroyTexture;

            std::function<EEngineStatus(SFrameGraphTexture     const& /* aTexture */
                                      , SFrameGraphTextureView const& /* aTextureView */)> createTextureView;
            std::function<EEngineStatus(SFrameGraphTextureView const& /* aTextureView */)> bindTextureView;
            std::function<EEngineStatus(SFrameGraphTextureView const& /* aTextureView */)> unbindTextureView;
            std::function<EEngineStatus(SFrameGraphTextureView const& /* aTextureView */)> destroyTextureView;

            std::function<EEngineStatus(AssetId_t const& /* aAssetId */)>                  loadBufferAsset;
            std::function<EEngineStatus(AssetId_t const& /* aAssetId */)>                  unloadBufferAsset;
            std::function<EEngineStatus(SFrameGraphBuffer const& /* aBuffer */)>           createBuffer;
            std::function<EEngineStatus(SFrameGraphBuffer const& /* aBuffer */)>           transferBufferData;
            std::function<EEngineStatus(SFrameGraphBuffer const& /* aBuffer */)>           destroyBuffer;

            std::function<EEngineStatus(SFrameGraphBuffer     const& /* aBuffer */
                                      , SFrameGraphBufferView const& /* aBufferView */)>   createBufferView;
            std::function<EEngineStatus(SFrameGraphBufferView const& /* aBufferView */)>   bindBufferView;
            std::function<EEngineStatus(SFrameGraphBufferView const& /* aBufferView */)>   unbindBufferView;
            std::function<EEngineStatus(SFrameGraphBufferView const& /* aBufferView */)>   destroyBufferView;

            std::function<EEngineStatus(SFrameGraphMesh const& /* aMesh */)>               readMeshAsset;
            std::function<EEngineStatus(SFrameGraphMesh const& /* aMesh */)>               unloadMeshAsset;
            std::function<EEngineStatus(SFrameGraphMesh const& /* aMesh */)>               bindMesh;
            std::function<EEngineStatus(SFrameGraphMesh const& /* aMesh */)>               unbindMesh;

            std::function<EEngineStatus(SFrameGraphMaterial const& /* aMesh */)>           readMaterialAsset;
            std::function<EEngineStatus(SFrameGraphMaterial const& /* aMesh */)>           unloadMaterialAsset;
            std::function<EEngineStatus(SFrameGraphMaterial const& /* aMesh */)>           bindMaterial;
            std::function<EEngineStatus(SFrameGraphMaterial const& /* aMesh */)>           unbindMaterial;
            std::function<EEngineStatus(  SFrameGraphMaterial                 const&
                                        , std::vector<SFrameGraphBuffer>      const&
                                        , std::vector<SFrameGraphTextureView> const&
                                        , std::vector<SSampledImageBinding>   const&)>     updateMaterial;

            std::function<EEngineStatus(std::string const&   /* aAttributeBufferId */
                                      , std::string const&   /* aIndexBufferId     */
                                      , Vector<VkDeviceSize> /* aOffsets           */)> bindAttributeAndIndexBuffers;

            std::function<EEngineStatus(ResourceId_t const& /* aPipelineUID */)> bindPipeline;
            std::function<EEngineStatus(ResourceId_t const& /* aPipelineUID */)> unbindPipeline;

            std::function<EEngineStatus(ResourceId_t const& /* aId */)> bindResource;
            std::function<EEngineStatus(ResourceId_t const& /* aId */)> unbindResource;

            std::function<EEngineStatus(SFrameGraphMesh     const& /* aMesh */
                                      , SFrameGraphMaterial const& /* aMaterial */)>       render;

            std::function<EEngineStatus(uint32_t const& /* aIndexCount */)>           drawIndexed;
            std::function<EEngineStatus()>                                            drawQuad;

            std::function<EEngineStatus(SFrameGraphMaterial const& /* aMaterial */)>  drawFullscreenQuadWithMaterial;
        };

    }
}

#endif
