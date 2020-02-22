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
            std::function<EEngineStatus(SFrameGraphRenderContextState       & /* aState         */
                                      , SFrameGraphDynamicTexture             const& /* aSourceTexture */
                                      , SFrameGraphDynamicTexture             const& /* aTargetTexture */)>                             copyImage;
            /**
             * Copy aTexture to the currently bound back buffer.
             *
             * @param aTexture Texture to copy from
             * @returns EEngineStatus::Ok    If successful
             * @returns EEngineStatus::Error On error
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphDynamicTexture const& /* aTexture */)> copyImageToBackBuffer;

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
                                      , std::string                   const &
                                      , uint32_t                      const &)>              clearAttachments;
            std::function<EEngineStatus(SFrameGraphRenderContextState       & /* aState */)> present;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState        & /* aState               */
                                      , std::string                     const& /* aRenderPassId        */
                                      , std::vector<PassUID_t>          const& /* aPassExecutionOrder  */
                                      , SFrameGraphAttachmentCollection const& /* aAttachmentInfo      */
                                      , CFrameGraphMutableResources     const& /* aFrameGraphResources */)> createRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState        & /* aState               */
                                      , std::string                     const& /* aRenderPassId        */
                                      , std::string                     const& /* aFrameBufferId       */
                                      , std::vector<PassUID_t>          const& /* aPassExecutionOrder  */
                                      , SFrameGraphAttachmentCollection const& /* aAttachmentInfo      */
                                      , CFrameGraphMutableResources     const& /* aFrameGraphResources */)> bindRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState      & /* aState         */
                                      , std::string                   const& /* aRenderPassId  */
                                      , std::string                   const& /* aFrameBufferId */)> unbindRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, std::string const& /* aRenderPass */)> destroyRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState      & /* aState         */
                                      , std::string                   const& /* aFrameBufferId */
                                      , std::string                   const& /* aRenderPassId  */)> createFrameBuffer;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState      & /* aState         */
                                      , std::string                   const& /* aFrameBufferId */)> destroyFrameBuffer;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphAssetTexture const& /* aTexture */)> readAssetTexture;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphAssetTexture const& /* aTexture */)> initializeAssetTexture;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphAssetTexture const& /* aTexture */)> transferAssetTexture;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphAssetTexture const& /* aTexture */)> deinitializeAssetTexture;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphAssetTexture const& /* aTexture */)> unloadAssetTexture;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphDynamicTexture const& /* aTexture */)> importDynamicTexture;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphDynamicTexture const& /* aTexture */)> createDynamicTexture;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphDynamicTexture const& /* aTexture */)> destroyDynamicTexture;

            std::function<EEngineStatus(SFrameGraphRenderContextState      & /* aState       */
                                      , SFrameGraphDynamicTexture     const& /* aTexture     */
                                      , SFrameGraphTextureView        const& /* aTextureView */)>                                        createTextureView;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphTextureView const& /* aTextureView */)> bindTextureView;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphTextureView const& /* aTextureView */)> unbindTextureView;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphTextureView const& /* aTextureView */)> destroyTextureView;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, AssetId_t const& /* aAssetId */)>                  loadBufferAsset;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, AssetId_t const& /* aAssetId */)>                  unloadBufferAsset;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphBuffer const& /* aBuffer */)>           createBuffer;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphBuffer const& /* aBuffer */)>           transferBufferData;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphBuffer const& /* aBuffer */)>           destroyBuffer;

            std::function<EEngineStatus(SFrameGraphRenderContextState      & /* aState      */
                                      , SFrameGraphBuffer             const& /* aBuffer     */
                                      , SFrameGraphBufferView         const& /* aBufferView */)>   createBufferView;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphBufferView const& /* aBufferView */)>   bindBufferView;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphBufferView const& /* aBufferView */)>   unbindBufferView;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphBufferView const& /* aBufferView */)>   destroyBufferView;

            // Mesh related function
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMesh const& /* aMesh */)>               readMeshAsset;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMesh const& /* aMesh */)>               initializeMesh;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMesh const& /* aMesh */)>               transferMesh;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMesh const& /* aMesh */)>               bindMesh;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMesh const& /* aMesh */)>               unbindMesh;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMesh const& /* aMesh */)>               deinitializeMesh;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMesh const& /* aMesh */)>               unloadMeshAsset;

            // Material related functions...
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMaterial const& /* aMaterial */)>       readMaterialAsset;
            std::function<EEngineStatus(SFrameGraphRenderContextState      & /* aState */
                                      , SFrameGraphMaterial           const& /* aMaterial */
                                      , ResourceId_t                  const& /* aRenderPassId */)>                                       initializeMaterial;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMaterial const& /* aMaterial */)>       transferMaterial;
            std::function<EEngineStatus(SFrameGraphRenderContextState      & /* aState */
                                      , SFrameGraphMaterial           const& /* aMaterial */)>                                            bindMaterial;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMaterial const& /* aMaterial */)>        unbindMaterial;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMaterial const& /* aMaterial */)>        deinitializeMaterial;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMaterial const& /* aMaterial */)>        unloadMaterialAsset;

            std::function<EEngineStatus(SFrameGraphRenderContextState      & /* aState             */
                                      , std::string                   const& /* aAttributeBufferId */
                                      , std::string                   const& /* aIndexBufferId     */
                                      , Vector<VkDeviceSize>                 /* aOffsets           */)> bindAttributeAndIndexBuffers;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, ResourceId_t const& /* aPipelineUID */)> bindPipeline;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, ResourceId_t const& /* aPipelineUID */)> unbindPipeline;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, ResourceId_t const& /* aId */)> bindResource;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, ResourceId_t const& /* aId */)> unbindResource;

            std::function<EEngineStatus(SFrameGraphRenderContextState      & /* aState */
                                      , SFrameGraphMesh               const& /* aMesh */
                                      , SFrameGraphMaterial           const& /* aMaterial */)> render;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, uint32_t const& /* aIndexCount */)> drawIndexed;
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */)>                                    drawQuad;

            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState */, SFrameGraphMaterial const& /* aMaterial */)>  drawFullscreenQuadWithMaterial;
        };

    }
}

#endif
