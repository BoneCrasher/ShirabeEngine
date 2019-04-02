#ifndef __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__
#define __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <asset/assetstorage.h>
#include <resources/core/resourcemanagerbase.h>

#include "renderer/irendercontext.h"
#include "renderer/renderertypes.h"
#include "renderer/framegraph/framegraphdata.h"
#include "renderer/framegraph/iframegraphrendercontext.h"

namespace engine
{
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
        using namespace engine::gfxapi;
        using namespace engine::rendering;
        /**
         * Default implementation of IFrameGraphRenderContext.
         */
        class SHIRABE_TEST_EXPORT CFrameGraphRenderContext
                : public IFrameGraphRenderContext
        {
            SHIRABE_DECLARE_LOG_TAG(CFrameGraphRenderContext)
        public:
            /**
             * Creator function to create a new frame graph render context.
             *
             *
             * @param aAssetStorage    AssetStorage for raw data access
             * @param aResourceManager Resourcemanager to manage resource creation and storage
             * @param aRenderer        Render context which maps down to the currently selected graphics API
             * @return                 A pointer to the newly created instance or nullptr on error.
             */
            static CEngineResult<CStdSharedPtr_t<CFrameGraphRenderContext>> create(
                    CStdSharedPtr_t<IAssetStorage>        aAssetStorage,
                    CStdSharedPtr_t<CMaterialLoader>      aMaterialLoader,
                    CStdSharedPtr_t<CResourceManagerBase> aResourceManager,
                    CStdSharedPtr_t<IRenderContext>       aRenderer);

            //
            // IFrameGraphRenderContext implementation
            //

            /**
             * Begin a pass.
             *
             * @return EEngineStatus::Ok, if successful.
             */
            CEngineResult<> beginPass();

            /**
             * End a pass.
             *
             * @return EEngineStatus::Ok, if successful.
             */
            CEngineResult<> endPass();

            /**
             * Copy the content of one image to another.
             *
             * @param aSourceImage
             * @param aTargetImage
             * @return             EEngineStatus::Ok if successful.
             * @return             EEngineStatus::Error otherwise.
             */
            CEngineResult<> copyImage(SFrameGraphTexture const &aSourceTexture,
                                      SFrameGraphTexture const &aTargetTexture);

            /**
             * Copy the content of an image to the backbuffer.
             *
             * @param aImageId
             * @return         EEngineStatus::Ok if successful.
             * @return         EEngineStatus::Error otherwise.
             */
            CEngineResult<> copyImageToBackBuffer(SFrameGraphTexture const &aSourceImageId);

            /**
             * Bind a swapchain, if any, for further framegraph operations.
             *
             * @param  aSwapChainResource The resource handle created for the swapchain in the FG.
             * @return                    EEngineStatus::Ok if successful.
             * @return                    EEngineStatus::Error otherwise.
             */
            CEngineResult<> bindSwapChain(std::string const &aSwapChainId);

            /**
             * Commit all changes and present the rendered content in the backbuffer to screen.
             *
             * @return EEngineStatus::Ok if successful.
             * @return EEngineStatus::Error otherwise.
             */
            CEngineResult<> present();

            /**
             * Put the internal command buffer into recording state.
             *
             * @return EEngineStatus::Ok if successful.
             * @return EEngineStatus::Error otherwise.
             */
            CEngineResult<> beginCommandBuffer();

            /**
             * Put the internal command buffer into non-recording state.
             *
             * @return EEngineStatus::Ok if successful.
             * @return EEngineStatus::Error otherwise.
             */
            CEngineResult<> commitCommandBuffer();

            /**
             * Create a framebuffer and render pass including subpasses for the provided attachment info.
             *
             * @param aFrameBufferId       Unique Id of the frame buffer instance to create.
             * @param aRenderPassId        Unique Id of the render pass instance to create.
             * @param aAttachmentInfo      Attachment information describing all subpasses, their attachments, etc...
             * @param aFrameGraphResources List of frame graph resources affiliated with the attachments
             * @return                     EEngineStatus::Ok if successful.
             * @return                     EEngineStatus::Error otherwise.
             */
            CEngineResult<> createFrameBufferAndRenderPass(
                    std::string                     const &aFrameBufferId,
                    std::string                     const &aRenderPassId,
                    SFrameGraphAttachmentCollection const &aAttachmentInfo,
                    CFrameGraphMutableResources     const &aFrameGraphResources);

            /**
             * Bind the framebuffer and render pass in the command buffer.
             * Has to be invoked after a command buffer has been activated.
             *
             * @param aFrameBufferId Unique Id of the frame buffer instance to bind.
             * @param aRenderPassId  Unique Id of the render pass instance  to bind.
             * @return               EEngineStatus::Ok if successful.
             * @return               EEngineStatus::Error otherwise.
             */
            CEngineResult<> bindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                         std::string const &aRenderPassId);

            /**
             * Unbind the framebuffer and render pass in the command buffer.
             * Has to be invoked before a command buffer will be deactivated.
             *
             * @param aFrameBufferId Unique Id of the frame buffer instance to unbind.
             * @param aRenderPassId  Unique Id of the render pass instance  to unbind.
             * @return               EEngineStatus::Ok if successful.
             * @return               EEngineStatus::Error otherwise.
             */
            CEngineResult<> unbindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                           std::string const &aRenderPassId);

            /**
             * Destroy the frame buffer and render pass identified by the arguments' UIDs.
             *
             * @param aFrameBufferId Unique Id of the frame buffer to destroy
             * @param aRenderPassId  Unique Id of the render pass to destroy
             * @return               EEngineStatus::Ok if successful.
             * @return               EEngineStatus::Error otherwise.
             */
            CEngineResult<> destroyFrameBufferAndRenderPass(
                    std::string                     const &aFrameBufferId,
                    std::string                     const &aRenderPassId);

            /**
             * Load a texture asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful.
             * @return          EEngineStatus::Error otherwise.
             */
            CEngineResult<> loadTextureAsset(AssetId_t const &aAssetUID);

            /**
             * Unload a texture asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful.
             * @return          EEngineStatus::Error otherwise.
             */
            CEngineResult<> unloadTextureAsset(AssetId_t const &aAssetUID);

            /**
             * Import a texture for use with framegraph render operations.
             *
             * @param aTexture The texture to import.
             * @return         EEngineStatus::Ok if successful.
             * @return         EEngineStatus::Error otherwise.
             */
            CEngineResult<> importTexture(SFrameGraphTexture const &aTexture);

            /**
             * Create a texture in the graphics API for use within render operations.
             *
             * @param aTexture Descriptor of the texture to create.
             * @return         EEngineStatus::Ok if successful.
             * @return         EEngineStatus::Error otherwise.
             */
            CEngineResult<> createTexture(SFrameGraphTexture const &texture);

            /**
             * Destroy a texture in the system.
             *
             * @param aTexture The texture to destroy.
             * @return         EEngineStatus::Ok if successful.
             * @return         EEngineStatus::Error otherwise.
             */
            CEngineResult<> destroyTexture(SFrameGraphTexture const &aTexture);

            /**
             * Create a texture view for a precreated texture for use within render operations.
             *
             * @param aTexture The texture to base the view on.
             * @param aView    Descriptor of the view to create.
             * @return         EEngineStatus::Ok if successful.
             * @return         EEngineStatus::Error otherwise.
             */
            CEngineResult<> createTextureView(
                    SFrameGraphTexture     const &aTexture,
                    SFrameGraphTextureView const &aView);

            /**
             * Bind a texture view to the graphics API.
             *
             * @param aView The view to bind.
             * @return      EEngineStatus::Ok if successful.
             * @return      EEngineStatus::Error otherwise.
             */
            CEngineResult<> bindTextureView(SFrameGraphTextureView  const&aView);

            /**
             * Unbind a texture view from the graphics API.
             *
             * @param aView The view to Unbind.
             * @return      EEngineStatus::Ok if successful.
             * @return      EEngineStatus::Error otherwise.
             */
            CEngineResult<> unbindTextureView( SFrameGraphTextureView const &aView);

            /**
             * Destroy a texture view in the system.
             *
             * @param aView The texture view to destroy.
             * @return      EEngineStatus::Ok if successful.
             * @return      EEngineStatus::Error otherwise.
             */
            CEngineResult<> destroyTextureView(SFrameGraphTextureView const &aView);

            /**
             * Create a buffer in the system.
             *
             * @param aResourceId
             * @param aResource
             * @param aBuffer
             * @return            EEngineStatus::Ok if successful.
             * @return            EEngineStatus::Error otherwise.
             */
            CEngineResult<> createBuffer(
                    FrameGraphResourceId_t const&aResourceId,
                    SFrameGraphResource    const&aResource,
                    SFrameGraphBuffer      const&aBuffer);

            /**
             * Load a buffer asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful.
             * @return          EEngineStatus::Error otherwise.
             */
            CEngineResult<> loadBufferAsset(AssetId_t  const &aAssetUID);

            /**
             * Unload a texture asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful.
             * @return          EEngineStatus::Error otherwise.
             */
            CEngineResult<> unloadBufferAsset(AssetId_t const &aAssetUID);

            /**
             * Destroy a buffer in the system.
             *
             * @param aResourceId The buffer id of the buffer to destroy.
             * @return            EEngineStatus::Ok if successful.
             * @return            EEngineStatus::Error otherwise.
             */
            CEngineResult<> destroyBuffer(FrameGraphResourceId_t const &aResourceId);

            /**
             * Create a buffer view in the system.
             *
             * @param aResourceId
             * @param aResource
             * @param aBufferView
             * @return            EEngineStatus::Ok if successful.
             * @return            EEngineStatus::Error otherwise.
             */
            CEngineResult<> createBufferView(
                    FrameGraphResourceId_t const&aResourceId,
                    SFrameGraphResource    const&aResource,
                    SFrameGraphBufferView  const&aBufferView);

            /**
             * Bind a buffer view to the graphics API.
             *
             * @param aResourceId The buffer view to bind.
             * @return            EEngineStatus::Ok if successful.
             * @return            EEngineStatus::Error otherwise.
             */
            CEngineResult<> bindBufferView(FrameGraphResourceId_t const &aResourceId);

            /**
             * Unbind a buffer view from the graphics API.
             *
             * @param aResource The view to Unbind.
             * @return          EEngineStatus::Ok if successful.
             * @return          EEngineStatus::Error otherwise.
             */
            CEngineResult<> unbindBufferView(FrameGraphResourceId_t const &aResourceId);

            /**
             * Destroy a buffer view in the system.
             *
             * @param aResourceId The id of the buffer view to destroy.
             * @return            EEngineStatus::Ok if successful.
             * @return            EEngineStatus::Error otherwise.
             */
            CEngineResult<> destroyBufferView(FrameGraphResourceId_t const &aResourceId);

            /**
             * Load a mesh asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful.
             * @return          EEngineStatus::Error otherwise.
             */
            CEngineResult<> loadMeshAsset(AssetId_t const &aAssetUID);

            /**
             * Unload a mesh asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful.
             * @return          EEngineStatus::Error otherwise.
             */
            CEngineResult<> unloadMeshAsset(AssetId_t const &aAssetUID);

            /**
             * Bind a mesh asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful.
             * @return          EEngineStatus::Error otherwise.
             */
            CEngineResult<> bindMesh(AssetId_t const &aAssetUID);

            /**
             * Unbind a mesh asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful.
             * @return          EEngineStatus::Error otherwise.
             */
            CEngineResult<> unbindMesh(AssetId_t const &aAssetUID);

            /**
             * Render a renderable entity using the graphicsAPI.
             *
             * @param aRenderable The renderable to process.
             * @return            EEngineStatus::Ok if successful.
             * @return            EEngineStatus::Error otherwise.
             */
            CEngineResult<> render(SRenderable const &aRenderable);

        private_constructors:
            /**
             * Create a new framegraph render context.
             *
             * @param aAssetStorage    AssetStorage for raw data access
             * @param aResourceManager Resourcemanager to manage resource creation and storage
             * @param aRenderer        Render context which maps down to the currently selected graphics API
             */
            CFrameGraphRenderContext(
                    CStdSharedPtr_t<IAssetStorage>        aAssetStorage,
                    CStdSharedPtr_t<CMaterialLoader>      aMaterialLoader,
                    CStdSharedPtr_t<CResourceManagerBase> aResourceManager,
                    CStdSharedPtr_t<IRenderContext>       aRenderer);

        private_methods:
            /**
             * Append a mapping from the public resource handles in the framegraph to the
             * resource handles created by the resource manager.
             *
             * @param aName       Name of the resource.
             * @param aResourceId The resource manager resource id for the resource.
             */
            CEngineResult<> mapFrameGraphToInternalResource(
                    std::string        const &aName,
                    PublicResourceId_t const &aResourceId);


            /**
             * Get the resource manager id by frame graph resource name.
             *
             * @param aName Name of the resource.
             * @return      A list of resource manager resource id's for the given framegraph resource.
             */
            CEngineResult<Vector<PublicResourceId_t>> getMappedInternalResourceIds(std::string const &aName) const;


            /**
             * Remove all resource manager resource ids for a provided frame graph resource name.
             *
             * @param aName Name of the resource.
             */
            CEngineResult<> removeMappedInternalResourceIds(std::string const &aName);

        private_members:
            CStdSharedPtr_t<IAssetStorage>        mAssetStorage;
            CStdSharedPtr_t<CMaterialLoader>      mMaterialLoader;
            CStdSharedPtr_t<CResourceManagerBase> mResourceManager;
            CStdSharedPtr_t<IRenderContext>       mGraphicsAPIRenderContext;

            Map<std::string, Vector<PublicResourceId_t>> mResourceMap;
        };

    }
}

#endif
