#ifndef __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__
#define __SHIRABE_FRAMEGRAPH_RENDERCONTEXT_H__

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
            SHIRABE_DECLARE_INTERFACE(IFrameGraphRenderContext);

        public_api:

            /**
             * @brief bindCommandBuffer
             * @return
             */
            virtual EEngineStatus bindCommandBuffer() = 0;

            /**
             * @brief commitCommandBuffer
             * @return
             */
            virtual EEngineStatus commitCommandBuffer() = 0;

            /**
             * Bind a swapchain, if any, for further framegraph operations.
             *
             * @param  aSwapChainResource The resource handle created for the swapchain in the FG.
             * @return                    EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus bindSwapChain(SFrameGraphResource const &aSwapChainResource) = 0;

            /**
             * Commit all changes and present the rendered content in the backbuffer to screen.
             *
             * @return EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus present() = 0;

            /**
             * Import a texture for use with framegraph render operations.
             *
             * @param aTexture The texture to import.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus importTexture(SFrameGraphTexture const &aTexture) = 0;

            /**
             * Create a texture in the graphics API for use within render operations.
             *
             * @param aTexture Descriptor of the texture to create.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus createTexture(SFrameGraphTexture const &aTexture) = 0;

            /**
             * Create a texture view for a precreated texture for use within render operations.
             *
             * @param aTexture The texture to base the view on.
             * @param aView    Descriptor of the view to create.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus createTextureView(
                    SFrameGraphTexture     const &aTexture,
                    SFrameGraphTextureView const &aView) = 0;

            virtual EEngineStatus createBuffer(
                    FrameGraphResourceId_t const &aResourceId,
                    SFrameGraphResource    const &aResource,
                    SFrameGraphBuffer      const &aBuffer) = 0;

            virtual EEngineStatus createBufferView(
                    FrameGraphResourceId_t const &aResourceId,
                    SFrameGraphResource    const &aResource,
                    SFrameGraphBufferView  const &aBufferView) = 0;

            /**
             * Load a texture asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus loadTextureAsset(AssetId_t const &aAssetUID) = 0;
            /**
             * Load a buffer asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus loadBufferAsset(AssetId_t const &aAssetUID) = 0;
            /**
             * Load a mesh asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus loadMeshAsset(AssetId_t const &aAssetUID) = 0;

            /**
             * Bind a texture view to the graphics API.
             *
             * @param aView The view to bind.
             * @return      EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus bindTextureView(SFrameGraphTextureView const &aView) = 0;
            /**
             * Bind a buffer view to the graphics API.
             *
             * @param aResourceId The buffer view to bind.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus bindBufferView(FrameGraphResourceId_t const &aResourceId) = 0;
            /**
             * Bind a mesh asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus bindMesh(AssetId_t const&aAssetUID) = 0;

            /**
             * Unbind a texture view from the graphics API.
             *
             * @param aView The view to Unbind.
             * @return      EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus unbindTextureView(SFrameGraphTextureView const &aView) = 0;
            /**
             * Unbind a buffer view from the graphics API.
             *
             * @param aResource The view to Unbind.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus unbindBufferView(FrameGraphResourceId_t  const &aResource) = 0;
            /**
             * Unbind a mesh asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus unbindMesh(AssetId_t const &aAssetUID) = 0;

            /**
             * Unload a texture asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus unloadTextureAsset(AssetId_t const &aAssetUID) = 0;
            /**
             * Unload a texture asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus unloadBufferAsset(AssetId_t const &aAssetUID) = 0;
            /**
             * Unload a mesh asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus unloadMeshAsset(AssetId_t const &aAssetUID) = 0;

            /**
             * Destroy a texture in the system.
             *
             * @param aTexture The texture to destroy.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus destroyTexture(SFrameGraphTexture const &aTexture) = 0;
            /**
             * Destroy a texture view in the system.
             *
             * @param aView The texture view to destroy.
             * @return      EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus destroyTextureView(SFrameGraphTextureView const &aView) = 0;
            /**
             * Destroy a buffer in the system.
             *
             * @param aResourceId The buffer id of the buffer to destroy.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus destroyBuffer(FrameGraphResourceId_t const &aResourceId) = 0;
            /**
             * Destroy a buffer view in the system.
             *
             * @param aResourceId The id of the buffer view to destroy.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus destroyBufferView(FrameGraphResourceId_t const &aResourceId) = 0;

            /**
             * Render a renderable entity using the graphicsAPI.
             *
             * @param aRenderable The renderable to process.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus render(SRenderable const &aRenderable) = 0;
        };

        /**
         * Default implementation of IFrameGraphRenderContext.
         */
        class SHIRABE_TEST_EXPORT CFrameGraphRenderContext
                : public IFrameGraphRenderContext
        {
            SHIRABE_DECLARE_LOG_TAG(CFrameGraphRenderContext);
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
            static CStdSharedPtr_t<CFrameGraphRenderContext> create(
                    CStdSharedPtr_t<IAssetStorage>        aAssetStorage,
                    CStdSharedPtr_t<CResourceManagerBase> aResourceManager,
                    CStdSharedPtr_t<IRenderContext>       aRenderer);

            EEngineStatus bindCommandBuffer();

            EEngineStatus commitCommandBuffer();

            /**
             * Bind a swapchain, if any, for further framegraph operations.
             *
             * @param  aSwapChainResource The resource handle created for the swapchain in the FG.
             * @return                    EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus bindSwapChain(SFrameGraphResource const &aSwapChainResource);

            /**
             * Commit all changes and present the rendered content in the backbuffer to screen.
             *
             * @return EEngineStatus::Ok if successful. False otherwise.
             */

            EEngineStatus present();

            /**
             * Import a texture for use with framegraph render operations.
             *
             * @param aTexture The texture to import.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus importTexture(SFrameGraphTexture const &aTexture);

            /**
             * Create a texture in the graphics API for use within render operations.
             *
             * @param texture Descriptor of the texture to create.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus createTexture(SFrameGraphTexture const&texture);

            /**
             * Create a texture view for a precreated texture for use within render operations.
             *
             * @param aTexture The texture to base the view on.
             * @param aView    Descriptor of the view to create.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus createTextureView(
                    SFrameGraphTexture     const &aTexture,
                    SFrameGraphTextureView const &aView);

            EEngineStatus createBuffer(
                    FrameGraphResourceId_t const&aResourceId,
                    SFrameGraphResource    const&aResource,
                    SFrameGraphBuffer      const&aBuffer);
            EEngineStatus createBufferView(
                    FrameGraphResourceId_t const&aResourceId,
                    SFrameGraphResource    const&aResource,
                    SFrameGraphBufferView  const&aBufferView);

            /**
             * Load a texture asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus loadTextureAsset(AssetId_t const &aAssetUID);
            /**
             * Load a buffer asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus loadBufferAsset(AssetId_t  const &aAssetUID);
            /**
             * Load a mesh asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus loadMeshAsset(AssetId_t const &aAssetUID);

            /**
             * Bind a texture view to the graphics API.
             *
             * @param aView The view to bind.
             * @return      EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus bindTextureView(SFrameGraphTextureView  const&aView);
            /**
             * Bind a buffer view to the graphics API.
             *
             * @param aResourceId The buffer view to bind.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus bindBufferView(FrameGraphResourceId_t const &aResourceId);
            /**
             * Bind a mesh asset using the asset manager into the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus bindMesh(AssetId_t const &aAssetUID);
            /**
             * Unbind a texture view from the graphics API.
             *
             * @param aView The view to Unbind.
             * @return      EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus unbindTextureView( SFrameGraphTextureView const &aView);
            /**
             * Unbind a buffer view from the graphics API.
             *
             * @param aResource The view to Unbind.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus unbindBufferView(FrameGraphResourceId_t const &aResourceId);
            /**
             * Unbind a mesh asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to load.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus unbindMesh(AssetId_t const &aAssetUID);


            /**
             * Unload a texture asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus unloadTextureAsset(AssetId_t const &aAssetUID);
            /**
             * Unload a texture asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus unloadBufferAsset(AssetId_t const &aAssetUID);
            /**
             * Unload a mesh asset from the graphics API.
             *
             * @param aAssetUID The UID of the asset to Unload.
             * @return          EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus unloadMeshAsset(AssetId_t const &aAssetUID);

            /**
             * Destroy a texture in the system.
             *
             * @param aTexture The texture to destroy.
             * @return         EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus destroyTexture(SFrameGraphTexture const &aTexture);
            /**
             * Destroy a texture view in the system.
             *
             * @param aView The texture view to destroy.
             * @return      EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus destroyTextureView(SFrameGraphTextureView const &aView);
            /**
             * Destroy a buffer in the system.
             *
             * @param aResourceId The buffer id of the buffer to destroy.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus destroyBuffer(FrameGraphResourceId_t const &aResourceId);
            /**
             * Destroy a buffer view in the system.
             *
             * @param aResourceId The id of the buffer view to destroy.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus destroyBufferView(FrameGraphResourceId_t const &aResourceId);

            /**
             * Render a renderable entity using the graphicsAPI.
             *
             * @param aRenderable The renderable to process.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus render(SRenderable const &aRenderable);

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
            void mapFrameGraphToInternalResource(
                    std::string        const &aName,
                    PublicResourceId_t const &aResourceId);

            /**
             * Get the resource manager id by frame graph resource name.
             *
             * @param aName Name of the resource.
             * @return      A list of resource manager resource id's for the given framegraph resource.
             */
            Vector<PublicResourceId_t> getMappedInternalResourceIds(std::string const &aName) const;

            /**
             * Remove all resource manager resource ids for a provided frame graph resource name.
             *
             * @param aName Name of the resource.
             */
            void removeMappedInternalResourceIds(std::string const &aName);

        private_members:
            CStdSharedPtr_t<IAssetStorage>        mAssetStorage;
            CStdSharedPtr_t<CResourceManagerBase> mResourceManager;
            CStdSharedPtr_t<IRenderContext>       mGraphicsAPIRenderContext;

            Map<std::string, Vector<PublicResourceId_t>> mResourceMap;
        };

    }
}

#endif
