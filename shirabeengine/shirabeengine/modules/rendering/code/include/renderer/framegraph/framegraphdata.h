#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__

#include <cstdint>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <variant>
#include <memory>
#include <cstring>
#include <ostream>

// #include <better-enums/enum.h>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <platform/platform.h>
#include <core/bitfield.h>
#include <core/basictypes.h>
#include <core/enginestatus.h>
#include <log/log.h>
#include <math/geometric/rect.h>
#include "renderer/renderertypes.h"

namespace engine
{
    namespace framegraph
    {
        using core::CBitField;
        using engine::CRange;
        using namespace engine::rendering;

        using FrameGraphResourceId_t = uint64_t;
        using PassUID_t              = uint64_t;

#define SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE 0

        SHIRABE_DECLARE_LIST_OF_TYPE(FrameGraphResourceId_t, FrameGraphResourceId);

        /**
         * The FrameGraphResourceType enum describes Values that represent top-level frame graph resource types
         */
        enum class EFrameGraphResourceType
                : uint8_t
        {
            Undefined = 0,
            Texture   = 1,
            Buffer,
            TextureView,
            BufferView,
            Mesh,
            Material,
            RenderableList,
            RenderableListView // To support renderable filtering...
        };

        /**
         * Values that represent texture and buffer formats
         */
        using FrameGraphFormat_t = graphicsapi::EFormat;

        /**
         * Checks, whether two formats are compatible.
         *
         * @param aBase    Reference format
         * @param aDerived Possibly compatible format.
         * @return         True, if compatible. False otherwise.
         */
        SHIRABE_TEST_EXPORT bool validateFormatCompatibility(
                FrameGraphFormat_t const &aBase,
                FrameGraphFormat_t const &aDerived);

        /**
         * The FrameGraphResourceUsage enum describes how a framegraph resource should be
         * used within the pipeline.
         */
        enum class EFrameGraphResourceUsage
                : uint8_t
        {
            Undefined        =   0,
            InputAttachment  =   1,
            ColorAttachment  =   2,
            DepthAttachment  =   4,
            SampledImage     =   8,
            StorageImage     =  16,
            BufferResource   =  32,
            BufferTarget     =  64,
            Unused           = 128
        };

        /**
         * The EFrameGraphWriteTarget enum describes how an image resource output should be processed.
         */
        enum class EFrameGraphWriteTarget
                : uint8_t
        {
            Undefined = 0,
            Color,
            Depth
        };

        /**
         * The EFrameGraphForwardTarget enum describes how an image resource forwarding should be processed.
         */
        enum class EFrameGraphForwardTarget
                : uint8_t
        {
            Undefined = 0,
            Color,
            Depth
        };

        /**
         * The EFrameGraphReadSource enum describes how an image resource should be read from.
         */
        enum class EFrameGraphReadSource
                : uint8_t
        {
            Undefined = 0,
            Color,
            Depth
        };

        /**
         * The EFrameGraphResourceAccessibility enum describes read/write access possibilities
         * to a resource.
         */
        enum class EFrameGraphResourceAccessibility
                : uint8_t
        {
            None      = 0,
            Immutable = 1,
            Mutable   = 2
        };

        /**
         * The EFrameGraphResourceInitState enum describes the required initial state of a
         * freshly created resource.
         */
        enum class EFrameGraphResourceInitState
                : uint8_t
        {
            Undefined = 0,
            Clear     = 1  // Resource will be cleared depending on the resource type.
        };

        /**
         * The EFrameGraphViewPurpose enum describes the pipeline data source of a texture view.
         */
        enum class EFrameGraphViewPurpose
                : uint8_t
        {
            Undefined = 0,
            ColorAttachment,
            DepthAttachment,
            InputAttachment,
            ShaderInput
        };

        /**
         * The FrameGraphViewAccessMode enum describes in which access modes a texture view may operate.
         */
        enum class EFrameGraphViewAccessMode
                : uint8_t
        {
            Undefined = 0,
            Forward   = 1,
            Accept    = 2,
            Read      = 4,
            Write     = 8
        };

        /**
         * Permit printing a FrameGraphFormat_t to outstreams.
         *
         * @param strm
         * @param e
         * @return
         */
        SHIRABE_TEST_EXPORT std::ostream& operator<<(std::ostream &strm, FrameGraphFormat_t const&e);
        /**
         * Permit printing a FrameGraphResourceType to outstreams.
         *
         * @param strm
         * @param e
         * @return
         */
        SHIRABE_TEST_EXPORT std::ostream& operator<<(std::ostream &strm, EFrameGraphResourceType const&e);

        /**
         * @brief The SFrameGraphResource struct
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphResource
        {
        public_constructors:
            /**
             * Default-Construct a framegraph resources.
             */
            SFrameGraphResource();

        public_operators:
            /**
             * Extract the resource id via implicit conversion.
             */
            explicit SHIRABE_INLINE operator FrameGraphResourceId_t()
            {
                return resourceId;
            }

            /**
             * Extract the resource id via implicit conversion in const context.
             */
            SHIRABE_INLINE operator FrameGraphResourceId_t const() const
            {
                return resourceId;
            }

        public_members:
            uint32_t                       referenceCount;
            PassUID_t                      assignedPassUID;
            FrameGraphResourceId_t         resourceId;
            FrameGraphResourceId_t         parentResource;
            FrameGraphResourceId_t         subjacentResource;
            std::string                    readableName;
            EFrameGraphResourceType        type;
            bool                           isExternalResource;
        };

        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphResource, SFrameGraphResource);

        SHIRABE_TEST_EXPORT
        SHIRABE_INLINE
        std::ostream& operator<<(std::ostream &aStream, SFrameGraphResource const &aResource)
        {
            aStream << aResource.resourceId;

            return aStream;
        }

        /**
         * Compare two framegraph resources for a less equal relation ship.
         *
         * @param aLHS
         * @param aRHS
         * @return     True if aLHS is ordered to the left of aRHS., False otherwise.
         */
        SHIRABE_TEST_EXPORT bool operator<(
                SFrameGraphResource const &aLHS,
                SFrameGraphResource const &aRHS);

        /**
         * Compare two framegraph resources for inequality.
         *
         * @param aLHS
         * @param aRHS
         * @return
         */
        SHIRABE_TEST_EXPORT bool operator!=(
                SFrameGraphResource const &aLHS,
                SFrameGraphResource const &aRHS);

        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphResource, SFrameGraphResource);

        /**
         * The SFrameGraphBuffer struct describes any kind of framegraph buffer resources.
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphBuffer
                : public SFrameGraphResource
        {
        public_constructors:
            /**
             * Default-Construct a framegraph buffer.
             */
            SFrameGraphBuffer();

        public_members:
            VkBufferUsageFlags bufferUsage;
            std::size_t        sizeInBytes;
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(SFrameGraphBuffer, SFrameGraphBuffer);
        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphBuffer, SFrameGraphBuffer);

        /**
         * The SFrameGraphBufferView struct describes any kind of buffer view resources in the framegraph.
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphBufferView
                : public SFrameGraphResource
        {
        public_constructors:
            /**
             * Default-Construct a framegraph buffer view.
             */
            SFrameGraphBufferView();

        public_members:
            CRange                               subrange;
            FrameGraphFormat_t                   format;
            CBitField<EFrameGraphViewAccessMode> mode;
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(SFrameGraphBufferView, SFrameGraphBufferView);
        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphBufferView, SFrameGraphBufferView);

        struct SHIRABE_TEST_EXPORT SFrameGraphAssetTexture
                : public SFrameGraphResource
        {
            asset::AssetId_t assetId;
        };

        /**
         * @brief The SFrameGraphDynamicTexture struct
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphDynamicTexture
                : public SFrameGraphResource
                , public graphicsapi::STextureInfo
        {
        public_constructors:
            /**
             * Default-Construt a frame graph texture
             */
            SFrameGraphDynamicTexture();

        public_destructors:
            virtual ~SFrameGraphDynamicTexture() = default;

        public_methods:
            /**
             * Copy-Over the texture info data from another framegraph texture.
             *
             * @param aOther The other texture to copy the texture information from.
             */
            void assignTextureParameters(SFrameGraphDynamicTexture const &aOther);

            /**
             * Validate the texture data.
             *
             * @return True if valid. False otherwise.
             */
             [[nodiscard]]
            virtual bool validate() const;

        public_members:
            EFrameGraphResourceInitState        initialState;
            CBitField<EFrameGraphResourceUsage> permittedUsage;
            CBitField<EFrameGraphResourceUsage> requestedUsage;
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(SFrameGraphDynamicTexture, SFrameGraphDynamicTexture);
        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphDynamicTexture, SFrameGraphDynamicTexture);

        /**
         * The SFrameGraphTextureView struct describes any kind of frame graph texture view in the framegraph
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphTextureView
                : public SFrameGraphResource
        {
        public_constructors:
            /**
             * Default-Construct a frame graph texture view.
             */
            SFrameGraphTextureView();

        public_members:
            CRange                               arraySliceRange;
            CRange                               mipSliceRange;
            FrameGraphFormat_t                   format;
            CBitField<EFrameGraphViewAccessMode> mode;
            EFrameGraphViewPurpose               source;
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(SFrameGraphTextureView, SFrameGraphTextureView);
        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphTextureView, SFrameGraphTextureView);


        /**
         * @brief The SFrameGraphAttachmentCollection struct
         */
        class SFrameGraphAttachmentCollection
        {
        public_constructors:
            SFrameGraphAttachmentCollection() = default;

            explicit SHIRABE_INLINE SFrameGraphAttachmentCollection(SFrameGraphAttachmentCollection const &aOther) = default;

        public_methods:
            /**
             * Add an input attachment to the collection and register it
             * for the given pass.
             *
             * @param aPassUID    The pass UID of the pass, which will access the attachment.
             * @param aResourceID The resourceUID of the texture view, which will serve as an
             *                    attachment.
             */
            void addInputAttachment(
                    PassUID_t              const &aPassUID,
                    FrameGraphResourceId_t const &aImageResourceID,
                    FrameGraphResourceId_t const &aImageViewResourceID);

            /**
             * Add a color attachment to the collection and register it
             * for the given pass.
             *
             * @param aPassUID    The pass UID of the pass, which will access the attachment.
             * @param aResourceID The resourceUID of the texture view, which will serve as an
             *                    attachment.
             */
            void addColorAttachment(
                    PassUID_t              const &aPassUID,
                    FrameGraphResourceId_t const &aImageResourceID,
                    FrameGraphResourceId_t const &aImageViewResourceID);

            /**
             * Add a depth attachment to the collection and register it
             * for the given pass.
             *
             * @param aPassUID    The pass UID of the pass, which will access the attachment.
             * @param aResourceID The resourceUID of the texture view, which will serve as an
             *                    attachment.
             */
            void addDepthAttachment(
                    PassUID_t              const &aPassUID,
                    FrameGraphResourceId_t const &aImageResourceID,
                    FrameGraphResourceId_t const &aImageViewResourceID);

            /**
             * Return the list of attachment resource ids.
             *
             * @return See brief.
             */
            Vector<FrameGraphResourceId_t>        const &getAttachementImageResourceIds()     const { return mAttachmentImageResourceIds;     }
            Vector<FrameGraphResourceId_t>        const &getAttachementImageViewResourceIds() const { return mAttachmentImageViewResourceIds; }
            Vector<uint64_t>                      const &getColorAttachments()                const { return mColorAttachments;               }
            Vector<uint64_t>                      const &getDepthAttachments()                const { return mDepthAttachments;               }
            Vector<uint64_t>                      const &getInputAttachments()                const { return mInputAttachments;               }
            Map<PassUID_t, Vector<uint64_t>>      const &getAttachmentPassToViewAssignment()  const { return mAttachmentPassAssignment;       }
            Map<FrameGraphResourceId_t, uint64_t> const &getAttachmentViewToImageAssignment() const { return mViewToImageAssignment;          }

        private_methods:

            /**
             * Add an input attachment to the collection and register it
             * for the given pass.
             *
             * @param aPassUID    The pass UID of the pass, which will access the attachment.
             * @param aResourceID The resourceUID of the texture view, which will serve as an
             *                    attachment.
             */
            uint32_t addAttachment(
                    PassUID_t              const &aPassUID,
                    FrameGraphResourceId_t const &aImageResourceID,
                    FrameGraphResourceId_t const &aImageViewResourceID);

        private_members:
            Vector<FrameGraphResourceId_t>
                mAttachmentImageResourceIds,
                mAttachmentImageViewResourceIds;
            Vector<uint64_t>
                mColorAttachments,
                mDepthAttachments,
                mInputAttachments;
            Map<PassUID_t, Vector<uint64_t>>
                mAttachmentPassAssignment;
            Map<FrameGraphResourceId_t, uint64_t>
                mViewToImageAssignment;
        };

        /**
         * Describes common resource flags for read/write operations.
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphResourceFlags
        {
        public_constructors:
            /**
             * Default-Construct resource flags.
             */
            SFrameGraphResourceFlags();

        public_members:
            FrameGraphFormat_t requiredFormat;
        };

        /**
         * Describes specialized constraints for texture resouces.
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphTextureResourceFlags
                : public SFrameGraphResourceFlags
        {
        public_members:
            CRange arraySliceRange;
            CRange mipSliceRange;
        };

        /**
         * Describes flags required for reading textures.
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphReadTextureFlags
                : public SFrameGraphTextureResourceFlags
        {
        public_members:
            EFrameGraphReadSource readSource;
        };

        /**
         * Describes flags required for writing textures.
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphWriteTextureFlags
                : public SFrameGraphTextureResourceFlags
        {
        public_members:
            EFrameGraphWriteTarget writeTarget;
        };

        /**
         * Describes a list of renderables as a resource bound to a framegraph.
         */
        struct SFrameGraphRenderableList
                : public SFrameGraphResource
        {
        public_members:
            rendering::RenderableList renderableList;
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(SFrameGraphRenderableList, SFrameGraphRenderableList);
        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphRenderableList, SFrameGraphRenderableList);
        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, rendering::RenderableList, RenderableList);

        /**
         * Describes a view of a list of renderables as a subset range of the full list of renderables available.
         */
        struct SFrameGraphRenderableListView
                : public SFrameGraphResource
        {
        public_members:
            std::vector<uint64_t> renderableRefIndices;
        };

        using FrameGraphRenderable_t = rendering::SRenderable;

        SHIRABE_DECLARE_LIST_OF_TYPE(SFrameGraphRenderableListView, SFrameGraphRenderableListView);
        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphRenderableListView, SFrameGraphRenderableListView);


        /**
         * Describes an adjacency list collection, mapping from a source id to a list of target ids.
         * Basically a list of lists of graph edges.
         */
        template <
                typename TUnderlyingIDFrom,
                typename TUnderlyingIDTo = TUnderlyingIDFrom
                >
        using AdjacencyListMap_t = std::unordered_map<TUnderlyingIDFrom, std::vector<TUnderlyingIDTo>>;

        /**
         * The frame graph pipeline struct encapsulates information on specific fixed function
         * pipeline configurations and references.
         */
        struct SFrameGraphShaderModule
                : SFrameGraphResource
        {
        public_members:
        };

        /**
         * The frame graph pipeline struct encapsulates information on specific fixed function
         * pipeline configurations and references.
         */
        struct SFrameGraphPipeline
            : SFrameGraphResource
        {
        public_members:
            SFrameGraphShaderModule shaderModuleResource;
        };

        /**
         * The frame graph material struct encapsulates information on
         * the material used for rendering.
         */
        struct SFrameGraphMaterial
            : SFrameGraphResource
        {
        public_members:
            std::vector<SFrameGraphBuffer>       uniformBuffers;
            std::vector<SFrameGraphAssetTexture> textures;
            SFrameGraphPipeline                  pipeline;
        };

        struct SFrameGraphMesh
            : SFrameGraphResource
        {
        public_members:
            SFrameGraphBuffer         attributeBuffer;
            SFrameGraphBuffer         indexBuffer;
            std::vector<VkDeviceSize> attributeOffsets;
            VkDeviceSize              indexCount;
        };

        struct SFrameGraphRenderableResources
        {
            SFrameGraphMesh                      meshResource;
            SFrameGraphMaterial                  materialResource;
            std::vector<SFrameGraphBuffer>       bufferResources;
            std::vector<SFrameGraphAssetTexture> textureResources;
        };

        struct SFrameGraphRenderableFetchFilter
        {};

        struct SFrameGraphDataSource
        {
            std::function<std::vector<SFrameGraphRenderableResources>(SFrameGraphRenderableFetchFilter)> fetchRenderables;
        };

        #define SHIRABE_FRAMEGRAPH_SUPPORTED_RESOURCE_TYPES  \
            SFrameGraphDynamicTexture,                       \
            SFrameGraphTextureView,                          \
            SFrameGraphBuffer,                               \
            SFrameGraphBufferView,                           \
            SFrameGraphMaterial,                             \
            SFrameGraphMesh,                                 \
            SFrameGraphRenderableList,                       \
            SFrameGraphRenderableListView


        /**
         * Describes a list of framegraph resources.
         */
        using Index_t = Vector<Shared<SFrameGraphResource>>;

        /**
         * Describes a list of resource ids to index into a list of resources.
         */
        using RefIndex_t = Vector<FrameGraphResourceId_t>;

        /**
         * Wraps a RefIndex_t type for a specific resource type.
         *
         * @tparam T The internal resource type of the wrapper.
         */
        template <typename T>
        class CFrameGraphResourcesRef
        {
        protected:
            /**
             * Insert a new resource reference.
             *
             * @param aRef
             */
            void insert(FrameGraphResourceId_t const &aRef)
            {
                m_index.push_back(aRef);
            }

            /**
             * Return the current index state immutably.
             *
             * @return See brief.
             */
            RefIndex_t const &get() const
            {
                return m_index;
            }

            /**
             * Return the current index state mutably.
             *
             * @return See brief.
             */
            RefIndex_t &getMutable()
            {
                return m_index;
            }

        private:
            RefIndex_t m_index;
        };

        /**
         * Variadic parameter pack unpacking helper to derive a FrameGraphResourcesRef<T>
         * for each type provided in TTypes
         *
         * @tparam TTypes List of types to implement FrameGraphResourcesRef<T> for.
         */
        template <typename... TTypes>
        class CFrameGraphResourcesRefContainer
                : public CFrameGraphResourcesRef<TTypes>...
        {};

        /**
         * Collection-type to store a list of resources and various resource-ref collections
         * for each supported type.
         */
        class CFrameGraphResources
                : public CFrameGraphResourcesRefContainer<SHIRABE_FRAMEGRAPH_SUPPORTED_RESOURCE_TYPES>
        {
            SHIRABE_DECLARE_LOG_TAG(CFrameGraphResources)

        public_constructors:
            CFrameGraphResources();

        public_methods:
            /**
             * Immutably fetch a stored resource pointer by resource id.
             *
             * @tparam T          The type of the resource to fetch.
             * @param aResourceId The id of the resource to fetch.
             * @return            A pointer to the desired result.
             * @throw             Throws std::runtime_error on error.
             */
            template <typename T>
            CEngineResult<Shared<T>> const
            getResource(FrameGraphResourceId_t const &aResourceId) const
            {
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(mResources.size() <= aResourceId)
                {
                    CLog::Error(logTag(), CString::format("Resource w/ resource id {} not found."));
                    return { EEngineStatus::ResourceError_NotFound };
                }
#endif

                Shared<SFrameGraphResource> resource = mResources.at(aResourceId);
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(nullptr == resource)
                {
                    CLog::Error(logTag(), CString::format("Resource w/ resource id {}: Handle is empty. Invalid resource."));
                    return { EEngineStatus::ResourceError_ResourceInvalid };
                }
#endif

                Shared<T> ptr = std::static_pointer_cast<T>(resource);
                return { EEngineStatus::Ok, ptr };
            }

            SHIRABE_INLINE Index_t                         const &resources()           const { return mResources;                                                    }
            SHIRABE_INLINE SFrameGraphAttachmentCollection const &attachements()        const { return mAttachements;                                                 }
            SHIRABE_INLINE RefIndex_t                      const &textures()            const { return CFrameGraphResourcesRef<SFrameGraphDynamicTexture>::get();     }
            SHIRABE_INLINE RefIndex_t                      const &textureViews()        const { return CFrameGraphResourcesRef<SFrameGraphTextureView>::get();        }
            SHIRABE_INLINE RefIndex_t                      const &buffers()             const { return CFrameGraphResourcesRef<SFrameGraphBuffer>::get();             }
            SHIRABE_INLINE RefIndex_t                      const &bufferViews()         const { return CFrameGraphResourcesRef<SFrameGraphBufferView>::get();         }
            SHIRABE_INLINE RefIndex_t                      const &renderablesLists()    const { return CFrameGraphResourcesRef<SFrameGraphRenderableList>::get();     }
            SHIRABE_INLINE RefIndex_t                      const &renderableListViews() const { return CFrameGraphResourcesRef<SFrameGraphRenderableListView>::get(); }
            SHIRABE_INLINE RefIndex_t                      const &meshes()              const { return CFrameGraphResourcesRef<SFrameGraphMesh>::get();               }
            SHIRABE_INLINE RefIndex_t                      const &materials()           const { return CFrameGraphResourcesRef<SFrameGraphMaterial>::get();           }

        protected_members:
            Index_t                         mResources;
            SFrameGraphAttachmentCollection mAttachements;
        };
        
        /**
         * Extends CFrameGraphResources so that mutable resource operations become possible. Also permits the creation of a new resource.
         */
        class CFrameGraphMutableResources
                : public CFrameGraphResources
        {
        public:
            /**
             * Spawn a new resource of type T inside this collection.
             *
             * @tparam T The type of resource to spawn.
             * @return   Returns a mutable reference to the newly created instance.
             */
            template <typename T> // with T : FrameGraphResource
            typename std::enable_if_t<std::is_base_of_v<SFrameGraphResource, T>, T> &spawnResource()
            {
                Shared<T> ptr = std::make_shared<T>();
                ptr->resourceId = mResources.size();

                mResources.push_back(ptr);

                CFrameGraphResourcesRef<T>::insert(ptr->resourceId);

                return (*ptr);
            }

            /**
             * Mutably fetch a stored resource pointer by resource id.
             *
             * @tparam T          The type of the resource to fetch.
             * @param aResourceId The id of the resource to fetch.
             * @return            A pointer to the desired result.
             * @throw             Throws std::runtime_error on error.
             */
            template <typename T> // with T : FrameGraphResource
            CEngineResult<Shared<T>>
            getResourceMutable(FrameGraphResourceId_t const &aResourceId)
            {
                CEngineResult<Shared<T>> const fetch  = static_cast<CFrameGraphResources *>(this)->getResource<T>(aResourceId);
                CEngineResult<Shared<T>>       result = { EEngineStatus::Ok };
                if(not fetch.resultEquals(EEngineStatus::ResourceError_NotFound))
                {
                   Shared<T> ptr = std::const_pointer_cast<T>(fetch.data());
                   result = CEngineResult<Shared<T>>(EEngineStatus::Ok, ptr);
                }

                return result;
            }

            SHIRABE_INLINE SFrameGraphAttachmentCollection &getAttachments()
            {
                return mAttachements;
            }


            /**
             * Merge two sets of framegraph resources together.
             *
             * @param aOther Another set of resources to be merged with this one.
             * @return       True, if aOther was successfully merged into this instance. False otherwise.
             */
            bool mergeIn(CFrameGraphResources const &aOther);
        };

    }

    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::EFrameGraphResourceType>(framegraph::EFrameGraphResourceType const &aType);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::FrameGraphFormat_t>(framegraph::FrameGraphFormat_t const &aFormat);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::EFrameGraphResourceUsage>(framegraph::EFrameGraphResourceUsage const &aUsage);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::EFrameGraphWriteTarget>(framegraph::EFrameGraphWriteTarget const &aTarget);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::EFrameGraphResourceAccessibility>(framegraph::EFrameGraphResourceAccessibility const &aAccessibility);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::EFrameGraphResourceInitState>(framegraph::EFrameGraphResourceInitState const &aState);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::EFrameGraphViewAccessMode>(framegraph::EFrameGraphViewAccessMode const &aAccessMode);


    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::SFrameGraphDynamicTexture>(framegraph::SFrameGraphDynamicTexture const &aTexture);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::SFrameGraphTextureView>(framegraph::SFrameGraphTextureView const &aTextureView);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::SFrameGraphBuffer>(framegraph::SFrameGraphBuffer const &aBuffer);
    template <>
    SHIRABE_TEST_EXPORT  std::string convert_to_string<framegraph::SFrameGraphBufferView>(framegraph::SFrameGraphBufferView const &aBufferView);

}

#endif
