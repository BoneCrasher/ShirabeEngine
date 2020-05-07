#ifndef __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__
#define __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__

#include <core/enginestatus.h>
#include <functional>
#include <log/log.h>
#include <core/enginetypehelper.h>
#include <asset/assetstorage.h>
#include <resources/extensibility.h>

#include "renderer/renderertypes.h"
#include "renderer/framegraph/framegraphdata.h"

namespace engine
{
    namespace resources
    {
        template <typename... TResources>
        class CResourceManagerBase;
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
            std::function<EEngineStatus(SFrameGraphRenderContextState  &/* aState         */
                                      , SFrameGraphTexture const&/* aSourceTexture */
                                      , SFrameGraphTexture const&/* aTargetTexture */)>       copyImage;
            /**
             * Copy aTexture to the currently bound back buffer.
             *
             * @param aTexture Texture to copy from
             * @returns EEngineStatus::Ok    If successful
             * @returns EEngineStatus::Error On error
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState  &/* aState */
                                      , SFrameGraphTexture const&/* aTexture */)>             copyImageToBackBuffer;

            /**
             * Transfers an image from one layout into another.
             * Important: If this call is made within a renderpass, a subpass-self-dependency is required!
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState  &/* aState        */
                                      , SFrameGraphTexture const&/* aImageHandle  */
                                      , CRange                    const&/* aArrayRange   */
                                      , CRange                    const&/* aMipRange     */
                                      , VkImageAspectFlags        const&/* aAspectFlags  */
                                      , VkImageLayout             const&/* aSourceLayout */
                                      , VkImageLayout             const&/* aTargetLayout */)> performImageLayoutTransfer;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */)> beginGraphicsFrame;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */)> endGraphicsFrame;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */)> beginSubpass;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */)> nextSubpass;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */)> beginFrameCommandBuffers;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */)> endFrameCommandBuffers;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */
                                      , std::string             const &)> clearAttachments;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */)> present;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState     &/* aState               */
                                      , ResourceId_t                const &/* aRenderPassId        */
                                      , ResourceId_t                const &/* aFrameBufferId       */)> bindRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState & /* aState         */
                                      , std::string              const& /* aRenderPassId  */
                                      , std::string              const& /* aFrameBufferId */)> unbindRenderPass;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */
                                      , SFrameGraphMesh          const&/* aMesh */)> useMesh;
            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */
                                      , SFrameGraphMaterial     const &/* aMaterial */
                                      , SFrameGraphPipeline     const &/* aPipeline */)> useMaterialWithPipeline;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */
                                      , SFrameGraphPipeline     const &/* aPipelineUID */)> bindPipeline;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */
                                      , VkDeviceSize            const &/* aIndexCount */)> drawIndexed;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */)> drawQuad;

            /**
             *
             */
            std::function<EEngineStatus(SFrameGraphRenderContextState &/* aState */, SFrameGraphMaterial const& /* aMaterial */)>  drawFullscreenQuadWithMaterial;
        };

        struct SHIRABE_LIBRARY_EXPORT SFrameGraphResourceContext
        {
            //---------------------------------------------------------------------------------------------------------------
            // RenderPass
            //---------------------------------------------------------------------------------------------------------------
            /**
             *
             */
            std::function<EEngineStatus(ResourceId_t                      const& /* aRenderPassId        */
                                        , std::vector<PassUID_t>          const& /* aPassExecutionOrder  */
                                        , SFrameGraphAttachmentCollection const& /* aAttachmentInfo      */
                                        , CFrameGraphMutableResources     const& /* aFrameGraphResources */)> createRenderPass;

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
            std::function<EEngineStatus(SFrameGraphTransientTexture const& /* aTexture */)> createTransientTexture;
            std::function<EEngineStatus(SFrameGraphTransientTexture const& /* aTexture */)> destroyTransientTexture;

            std::function<EEngineStatus(SFrameGraphPersistentTexture const &/* aTexture */)> initializePersistentTexture;
            std::function<EEngineStatus(SFrameGraphPersistentTexture const &/* aTexture */)> updatePersistentTexture;
            std::function<EEngineStatus(SFrameGraphPersistentTexture const &/* aTexture */)> deinitializePersistentTexture;

            std::function<EEngineStatus(SFrameGraphTexture     const& /* aTexture     */
                                      , SFrameGraphTextureView const& /* aTextureView */)> createTextureView;
            std::function<EEngineStatus(SFrameGraphTextureView const& /* aTextureView */)> destroyTextureView;

            //---------------------------------------------------------------------------------------------------------------
            // Buffers
            //---------------------------------------------------------------------------------------------------------------
            std::function<EEngineStatus(SFrameGraphTransientBuffer const& /* aBuffer */)> createTransientBuffer;
            std::function<EEngineStatus(SFrameGraphTransientBuffer const& /* aBuffer */)> destroyTransientBuffer;

            std::function<EEngineStatus(SFrameGraphPersistentBuffer const& /* aBuffer */)> initializePersistentBuffer;
            std::function<EEngineStatus(SFrameGraphPersistentBuffer const& /* aBuffer */)> updatePersistentBuffer;
            std::function<EEngineStatus(SFrameGraphPersistentBuffer const& /* aBuffer */)> deinitializePersistentBuffer;

            std::function<EEngineStatus(SFrameGraphBuffer     const& /* aBuffer     */
                                      , SFrameGraphBufferView const& /* aBufferView */)> createBufferView;
            std::function<EEngineStatus(SFrameGraphBufferView const& /* aBufferView */)> destroyBufferView;

            //---------------------------------------------------------------------------------------------------------------
            // Meshes
            //---------------------------------------------------------------------------------------------------------------
            std::function<EEngineStatus(SFrameGraphMesh const &/* aMesh */)> initializeMesh;
            std::function<EEngineStatus(SFrameGraphMesh const &/* aMesh */)> updateMesh;
            std::function<EEngineStatus(SFrameGraphMesh const &/* aMesh */)> deinitializeMesh;

            //---------------------------------------------------------------------------------------------------------------
            // Materials
            //---------------------------------------------------------------------------------------------------------------
            std::function<EEngineStatus(SFrameGraphMaterial const &/* aMaterial */)> initializeMaterial;
            std::function<EEngineStatus(SFrameGraphMaterial const &/* aMaterial */)> updateMaterial;
            std::function<EEngineStatus(SFrameGraphMaterial const &/* aMaterial */)> deinitializeMaterial;

            //---------------------------------------------------------------------------------------------------------------
            // Pipelines
            //---------------------------------------------------------------------------------------------------------------
            std::function<EEngineStatus(SFrameGraphPipeline const &/* aPipeline */)> createPipeline;
            std::function<EEngineStatus(SFrameGraphPipeline const &/* aPipeline */)> destroyPipeline;
        };

    }
}


#endif
