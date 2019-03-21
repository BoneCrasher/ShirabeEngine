#ifndef __SHIRABE_FRAMEGRAPH_IFRAMEGRAPH_RENDERCONTEXT_H__
#define __SHIRABE_FRAMEGRAPH_IFRAMEGRAPH_RENDERCONTEXT_H__

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <asset/assetstorage.h>
#include <resources/core/resourcemanagerbase.h>
#include "renderer/renderertypes.h"
#include "renderer/framegraph/framegraphdata.h"

namespace engine
{
    namespace rendering
    {
        class IRenderContext;
    }

    namespace framegraph
    {
        using engine::asset::AssetId_t;
        using engine::asset::IAssetStorage;
        using namespace engine::resources;
        using namespace engine::gfxapi;
        using namespace engine::rendering;

        /**
         * The IFrameGraphRenderContext interface describes the basic requirements for a compatible
         * framegraph render context implementation.
         */
        class IFrameGraphRenderContext
        {
            SHIRABE_DECLARE_INTERFACE(IFrameGraphRenderContext)

        public_api:

            /**
             * Begin a pass.
             *
             * @return EEngineStatus::Ok, if successful.
             */
            virtual CEngineResult<> beginPass() = 0;

            /**
             * End a pass.
             *
             * @return EEngineStatus::Ok, if successful.
             */
            virtual CEngineResult<> endPass() = 0;

            /**
             * Copy the content of one image to another.
             *
             * @param aSourceImage
             * @param aTargetImage
             * @return
             */
            virtual CEngineResult<> copyImage(SFrameGraphTexture const &aSourceImage,
                                              SFrameGraphTexture const &aTargetImage) = 0;

            /**
             * Copy the content of an image to the backbuffer.
             *
             * @param aImageId
             * @return
             */
            virtual CEngineResult<> copyToBackBuffer(SFrameGraphTexture const &aImage) = 0;

            //
            // SwapChain and Presentation
            //

            /**
             * Bind a swapchain, if any, for further framegraph operations.
             *
             * @param  aSwapChainResource The resource handle created for the swapchain in the FG.
             * @return                    EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> bindSwapChain(SFrameGraphResource const &aSwapChainResource) = 0;

            /**
             * Commit all changes and present the rendered content in the backbuffer to screen.
             *
             * @return EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> present() = 0;

            //
            // Command Buffer
            //

            /**
             * @brief bindCommandBuffer
             * @return
             */
            virtual CEngineResult<> beginCommandBuffer() = 0;

            /**
             * @brief commitCommandBuffer
             * @return
             */
            virtual CEngineResult<> commitCommandBuffer() = 0;

            //
            // FrameBuffer & RenderPass
            //
            /**
             * Create a framebuffer and render pass including subpasses for the provided attachment info.
             *
             * @param aFrameBufferId       Unique Id of the frame buffer instance to create.
             * @param aRenderPassId        Unique Id of the render pass instance to create.
             * @param aAttachmentInfo      Attachment information describing all subpasses, their attachments, etc...
             * @param aFrameGraphResources List of frame graph resources affiliated with the attachments
             * @return EEngineStatus::Ok, if successful. An errorcode otherwise.
             */
            virtual CEngineResult<> createFrameBufferAndRenderPass(
                    std::string                     const &aFrameBufferId,
                    std::string                     const &aRenderPassId,
                    SFrameGraphAttachmentCollection const &aAttachmentInfo,
                    CFrameGraphMutableResources     const &aFrameGraphResources) = 0;

            /**
             * Bind the framebuffer and render pass in the command buffer.
             * Has to be invoked after a command buffer has been activated.
             *
             * @param aFrameBufferId       Unique Id of the frame buffer instance to bind.
             * @param aRenderPassId        Unique Id of the render pass instance  to bind.
             * @return
             */
            virtual CEngineResult<> bindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                                 std::string const &aRenderPassId) = 0;


            /**
             * Unbind the framebuffer and render pass in the command buffer.
             * Has to be invoked before a command buffer will be deactivated.
             *
             * @param aFrameBufferId       Unique Id of the frame buffer instance to unbind.
             * @param aRenderPassId        Unique Id of the render pass instance  to unbind.
             * @return
             */
            virtual CEngineResult<> unbindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                                   std::string const &aRenderPassId) = 0;

            /**
             * Destroy the frame buffer and render pass identified by the arguments' UIDs.
             *
             * @param aFrameBufferId       Unique Id of the frame buffer to destroy
             * @param aRenderPassId        Unique Id of the render pass to destroy
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual CEngineResult<> destroyFrameBufferAndRenderPass(
                    std::string                     const &aFrameBufferId,
                    std::string                     const &aRenderPassId) = 0;

            //
            // Texture
            //

            /**
             * Load a texture asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> loadTextureAsset(AssetId_t const &aAssetUID) = 0;

            /**
             * Unload a texture asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> unloadTextureAsset(AssetId_t const &aAssetUID) = 0;

            /**
             * Import a texture for use with framegraph render operations.
             *
             * @param aTexture The texture to import.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> importTexture(SFrameGraphTexture const &aTexture) = 0;

            /**
             * Create a texture in the graphics API for use within render operations.
             *
             * @param aTexture Descriptor of the texture to create.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> createTexture(SFrameGraphTexture const &aTexture) = 0;

            /**
             * Destroy a texture in the system.
             *
             * @param aTexture The texture to destroy.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> destroyTexture(SFrameGraphTexture const &aTexture) = 0;

            /**
             * Create a texture view for a precreated texture for use within render operations.
             *
             * @param aTexture The texture to base the view on.
             * @param aView    Descriptor of the view to create.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> createTextureView(
                    SFrameGraphTexture     const &aTexture,
                    SFrameGraphTextureView const &aView) = 0;

            /**
             * Bind a texture view to the graphics API.
             *
             * @param aView The view to bind.
             * @return      EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> bindTextureView(SFrameGraphTextureView const &aView) = 0;

            /**
             * Unbind a texture view from the graphics API.
             *
             * @param aView The view to Unbind.
             * @return      EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> unbindTextureView(SFrameGraphTextureView const &aView) = 0;

            /**
             * Destroy a texture view in the system.
             *
             * @param aView The texture view to destroy.
             * @return      EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> destroyTextureView(SFrameGraphTextureView const &aView) = 0;

            //
            // Buffer
            //
            virtual CEngineResult<> createBuffer(
                    FrameGraphResourceId_t const &aResourceId,
                    SFrameGraphResource    const &aResource,
                    SFrameGraphBuffer      const &aBuffer) = 0;

            /**
             * Load a buffer asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> loadBufferAsset(AssetId_t const &aAssetUID) = 0;

            /**
             * Unload a texture asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> unloadBufferAsset(AssetId_t const &aAssetUID) = 0;

            /**
             * Destroy a buffer in the system.
             *
             * @param aResourceId The buffer id of the buffer to destroy.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> destroyBuffer(FrameGraphResourceId_t const &aResourceId) = 0;

            virtual CEngineResult<> createBufferView(
                    FrameGraphResourceId_t const &aResourceId,
                    SFrameGraphResource    const &aResource,
                    SFrameGraphBufferView  const &aBufferView) = 0;

            /**
             * Bind a buffer view to the graphics API.
             *
             * @param aResourceId The buffer view to bind.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> bindBufferView(FrameGraphResourceId_t const &aResourceId) = 0;

            /**
             * Unbind a buffer view from the graphics API.
             *
             * @param aResource The view to Unbind.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> unbindBufferView(FrameGraphResourceId_t  const &aResource) = 0;
            /**
             * Destroy a buffer view in the system.
             *
             * @param aResourceId The id of the buffer view to destroy.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> destroyBufferView(FrameGraphResourceId_t const &aResourceId) = 0;

            /**
             * Load a mesh asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> loadMeshAsset(AssetId_t const &aAssetUID) = 0;

            /**
             * Unload a mesh asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> unloadMeshAsset(AssetId_t const &aAssetUID) = 0;

            /**
             * Bind a mesh asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> bindMesh(AssetId_t const&aAssetUID) = 0;
            /**
             * Unbind a mesh asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> unbindMesh(AssetId_t const &aAssetUID) = 0;

            /**
             * Render a renderable entity using the graphicsAPI.
             *
             * @param aRenderable The renderable to process.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            virtual CEngineResult<> render(SRenderable const &aRenderable) = 0;
        };

    }
}

#endif
