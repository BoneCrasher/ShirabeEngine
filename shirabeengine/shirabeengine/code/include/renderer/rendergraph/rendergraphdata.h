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

#include "rhi/resource_management/extensibility.h"
#include "renderer/renderertypes.h"

#ifdef None
#undef None
#endif

namespace engine
{
    namespace framegraph
    {
        using core::CBitField;
        using engine::CRange;
        using namespace engine::rendering;

        using RenderGraphResourceId_t = uint64_t;
        using PassUID_t              = uint64_t;
        using RenderPassUID_t        = uint64_t;

#define SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE 0

        SHIRABE_DECLARE_LIST_OF_TYPE(RenderGraphResourceId_t, RenderGraphResourceId)

        /**
         * The RenderGraphResourceType enum describes Values that represent top-level frame graph resource types
         */
        enum class ERenderGraphResourceType
                : uint8_t
        {
            Undefined = 0
            , Image   = 1
            , Buffer
            , ImageView
            , BufferView
            , Mesh
            , Material
            , Pipeline
        };

        /**
         * Values that represent texture and buffer formats
         */
        using RenderGraphFormat_t = graphicsapi::EFormat;

        /**
         * Checks, whether two formats are compatible.
         *
         * @param aBase    Reference format
         * @param aDerived Possibly compatible format.
         * @return         True, if compatible. False otherwise.
         */
        SHIRABE_TEST_EXPORT bool validateFormatCompatibility(
                RenderGraphFormat_t const &aBase,
                RenderGraphFormat_t const &aDerived);

        /**
         * The RenderGraphResourceUsage enum describes how a rendergraph resource should be
         * used within the pipeline.
         */
        enum class ERenderGraphResourceUsage
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
         * The ERenderGraphWriteTarget enum describes how an image resource output should be processed.
         */
        enum class ERenderGraphWriteTarget
                : uint8_t
        {
            Undefined = 0,
            Color,
            Depth
        };

        /**
         * The ERenderGraphForwardTarget enum describes how an image resource forwarding should be processed.
         */
        enum class ERenderGraphForwardTarget
                : uint8_t
        {
            Undefined = 0,
            Color,
            Depth
        };

        /**
         * The ERenderGraphReadSource enum describes how an image resource should be read from.
         */
        enum class ERenderGraphReadSource
                : uint8_t
        {
            Undefined = 0,
            Color,
            Depth
        };

        /**
         * The ERenderGraphResourceAccessibility enum describes read/write access possibilities
         * to a resource.
         */
        enum class ERenderGraphResourceAccessibility
                : uint8_t
        {
            None      = 0,
            Immutable = 1,
            Mutable   = 2
        };

        /**
         * The ERenderGraphResourceInitState enum describes the required initial state of a
         * freshly created resource.
         */
        enum class ERenderGraphResourceInitState
                : uint8_t
        {
            Undefined = 0,
            Clear     = 1  // Resource will be cleared depending on the resource type.
        };

        /**
         * The ERenderGraphViewPurpose enum describes the pipeline data source of a texture view.
         */
        enum class ERenderGraphViewPurpose
                : uint8_t
        {
            Undefined = 0,
            ColorAttachment,
            DepthAttachment,
            InputAttachment,
            ShaderInput
        };

        /**
         * The RenderGraphViewAccessMode enum describes in which access modes a texture view may operate.
         */
        enum class ERenderGraphViewAccessMode
                : uint8_t
        {
            Undefined = 0,
            Forward   = 1,
            Accept    = 2,
            Read      = 4,
            Write     = 8
        };

        /**
         * Permit printing a RenderGraphFormat_t to outstreams.
         *
         * @param strm
         * @param e
         * @return
         */
        SHIRABE_TEST_EXPORT std::ostream& operator<<(std::ostream &strm, RenderGraphFormat_t const&e);
        /**
         * Permit printing a RenderGraphResourceType to outstreams.
         *
         * @param strm
         * @param e
         * @return
         */
        SHIRABE_TEST_EXPORT std::ostream& operator<<(std::ostream &strm, ERenderGraphResourceType const&e);

        /**
         * @brief The SRenderGraphResource struct
         */
        struct SHIRABE_TEST_EXPORT SRenderGraphResource
        {
        public_constructors:
            /**
             * Default-Construct a rendergraph resources.
             */
            SRenderGraphResource();

        public_operators:
            /**
             * Extract the resource id via implicit conversion.
             */
            explicit SHIRABE_INLINE operator RenderGraphResourceId_t()
            {
                return resourceId;
            }

            /**
             * Extract the resource id via implicit conversion in const context.
             */
            SHIRABE_INLINE operator RenderGraphResourceId_t const() const
            {
                return resourceId;
            }

        public_members:
            uint32_t                 referenceCount;
            RenderPassUID_t          assignedRenderpassUID;
            PassUID_t                assignedPassUID;
            RenderGraphResourceId_t  resourceId;
            RenderGraphResourceId_t  parentResource;
            RenderGraphResourceId_t  subjacentResource;
            std::string              readableName;
            ERenderGraphResourceType type;
            bool                     isExternalResource;
        };

        SHIRABE_DECLARE_MAP_OF_TYPES(RenderGraphResourceId_t, SRenderGraphResource, SRenderGraphResource)

        SHIRABE_TEST_EXPORT
        SHIRABE_INLINE
        std::ostream& operator<<(std::ostream &aStream, SRenderGraphResource const &aResource)
        {
            aStream << aResource.resourceId;

            return aStream;
        }

        /**
         * Compare two rendergraph resources for a less equal relation ship.
         *
         * @param aLHS
         * @param aRHS
         * @return     True if aLHS is ordered to the left of aRHS., False otherwise.
         */
        SHIRABE_TEST_EXPORT bool operator<(
                SRenderGraphResource const &aLHS,
                SRenderGraphResource const &aRHS);

        /**
         * Compare two rendergraph resources for inequality.
         *
         * @param aLHS
         * @param aRHS
         * @return
         */
        SHIRABE_TEST_EXPORT bool operator!=(
                SRenderGraphResource const &aLHS,
                SRenderGraphResource const &aRHS);

        SHIRABE_DECLARE_MAP_OF_TYPES(RenderGraphResourceId_t, SRenderGraphResource, SRenderGraphResource)

        template <typename T>
        struct SHIRABE_TEST_EXPORT SRenderGraphTypedResource
            : public SRenderGraphResource
        {
        public_constructors:
        public_members:
            T description;
        };

        struct SHIRABE_TEST_EXPORT SRenderGraphPersistentBufferDescription
        {
        public_constructors:
            SRenderGraphPersistentBufferDescription();

        public_members:
            rhi::ResourceId_t bufferResourceId;
        };

        struct SHIRABE_TEST_EXPORT SRenderGraphDynamicBufferDescription
        {
        public_constructors:
            SRenderGraphDynamicBufferDescription();

        public_members:
            VkBufferUsageFlags      bufferUsage;
            std::size_t             sizeInBytes;
        };

        struct SHIRABE_TEST_EXPORT SRenderGraphBufferDescription
        {
        public_constructors:
            SRenderGraphBufferDescription();

        public_members:
            bool isDynamicBuffer;
            SRenderGraphDynamicBufferDescription    dynamicBuffer;
            SRenderGraphPersistentBufferDescription persistentBuffer;
        };

        /**
         * The SRenderGraphBuffer struct describes any kind of rendergraph buffer resources.
         */
        struct SHIRABE_TEST_EXPORT SRenderGraphBuffer
                : public SRenderGraphTypedResource<SRenderGraphBufferDescription>
        {
        public_constructors:
            /**
             * Default-Construct a rendergraph buffer.
             */
            SRenderGraphBuffer();
        };

        struct SHIRABE_TEST_EXPORT SRenderGraphBufferViewDescription
        {
        public_constructors:
            SRenderGraphBufferViewDescription();

        public_members:
            CRange                                subrange;
            RenderGraphFormat_t                   format;
            CBitField<ERenderGraphViewAccessMode> mode;
        };

        /**
         * The SRenderGraphBufferView struct describes any kind of buffer view resources in the rendergraph.
         */
        struct SHIRABE_TEST_EXPORT SRenderGraphBufferView
                : public SRenderGraphTypedResource<SRenderGraphBufferViewDescription>
        {
        public_constructors:
            /**
             * Default-Construct a rendergraph buffer view.
             */
            SRenderGraphBufferView();

        public_members:
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(SRenderGraphBufferView, SRenderGraphBufferView)
        SHIRABE_DECLARE_MAP_OF_TYPES(RenderGraphResourceId_t, SRenderGraphBufferView, SRenderGraphBufferView)

        struct SHIRABE_TEST_EXPORT SRenderGraphDynamicImageDescription
            : public graphicsapi::STextureInfo
        {
        public_constructors:
            SRenderGraphDynamicImageDescription();
            SRenderGraphDynamicImageDescription(SRenderGraphDynamicImageDescription const &aOther) = default;
            explicit SRenderGraphDynamicImageDescription(graphicsapi::STextureInfo const &aTextureInfo);

        public_members:
            ERenderGraphResourceInitState        initialState;
            CBitField<ERenderGraphResourceUsage> permittedUsage;
            CBitField<ERenderGraphResourceUsage> requestedUsage;
        };

        struct SHIRABE_TEST_EXPORT SRenderGraphPersistentImageDescription
        {
        public_constructors:
            SRenderGraphPersistentImageDescription();

        public_members:
            rhi::ResourceId_t imageId;
        };

        struct SHIRABE_TEST_EXPORT SRenderGraphImageDescription
        {
        public_constructors:
            SRenderGraphImageDescription();

        public_members:
            bool isDynamicImage;
            SRenderGraphDynamicImageDescription    dynamicImage;
            SRenderGraphPersistentImageDescription persistentImage;
        };

        /**
         * @brief The SRenderGraphImage struct
         */
        struct SHIRABE_TEST_EXPORT SRenderGraphImage
                : public SRenderGraphTypedResource<SRenderGraphImageDescription>
        {
        public_constructors:
            /**
             * Default-Construct a frame graph texture
             */
            SRenderGraphImage();

        public_destructors:
            virtual ~SRenderGraphImage() = default;

        public_methods:

            /**
             * Validate the texture data.
             *
             * @return True if valid. False otherwise.
             */
             [[nodiscard]]
            virtual bool validate() const;
        };

        struct SHIRABE_TEST_EXPORT SRenderGraphRenderTarget
            : public SRenderGraphImage
        {};

        struct SHIRABE_TEST_EXPORT SRenderGraphImageViewDescription
        {
        public_constructors:
            SRenderGraphImageViewDescription();

        public_members:
            CRange                               arraySliceRange;
            CRange                               mipSliceRange;
            RenderGraphFormat_t                   format;
            CBitField<ERenderGraphViewAccessMode> mode;
            ERenderGraphViewPurpose               source;
        };

        /**
         * The SRenderGraphImageView struct describes any kind of frame graph texture view in the rendergraph
         */
        struct SHIRABE_TEST_EXPORT SRenderGraphImageView
                : public SRenderGraphTypedResource<SRenderGraphImageViewDescription>
        {
        public_constructors:
            /**
             * Default-Construct a frame graph texture view.
             */
            SRenderGraphImageView();
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(SRenderGraphImageView, SRenderGraphImageView)
        SHIRABE_DECLARE_MAP_OF_TYPES(RenderGraphResourceId_t, SRenderGraphImageView, SRenderGraphImageView)

        /**
         * @brief The SRenderGraphAttachmentCollection struct
         */
        class SRenderGraphAttachmentCollection
        {
        public_constructors:
            SRenderGraphAttachmentCollection() = default;

            explicit SHIRABE_INLINE SRenderGraphAttachmentCollection(SRenderGraphAttachmentCollection const &aOther) = default;

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
                    RenderGraphResourceId_t const &aImageResourceID,
                    RenderGraphResourceId_t const &aImageViewResourceID);

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
                    RenderGraphResourceId_t const &aImageResourceID,
                    RenderGraphResourceId_t const &aImageViewResourceID);

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
                    RenderGraphResourceId_t const &aImageResourceID,
                    RenderGraphResourceId_t const &aImageViewResourceID);

            /**
             * Return the list of attachment resource ids.
             *
             * @return See brief.
             */
            Vector<RenderGraphResourceId_t>        const &getAttachmentImageResourceIds()     const { return mAttachmentImageResourceIds;     }
            Vector<RenderGraphResourceId_t>        const &getAttachmentImageViewResourceIds() const { return mAttachmentImageViewResourceIds; }
            Vector<uint64_t>                      const &getColorAttachments()                const { return mColorAttachments;               }
            Vector<uint64_t>                      const &getDepthAttachments()                const { return mDepthAttachments;               }
            Vector<uint64_t>                      const &getInputAttachments()                const { return mInputAttachments;               }
            Map<PassUID_t, Vector<uint64_t>>      const &getAttachmentPassToViewAssignment()  const { return mAttachmentPassAssignment;       }
            Map<RenderGraphResourceId_t, uint64_t> const &getAttachmentViewToImageAssignment() const { return mViewToImageAssignment;          }

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
                    RenderGraphResourceId_t const &aImageResourceID,
                    RenderGraphResourceId_t const &aImageViewResourceID);

        private_members:
            Vector<RenderGraphResourceId_t>
                mAttachmentImageResourceIds,
                mAttachmentImageViewResourceIds;
            Vector<uint64_t>
                mColorAttachments,
                mDepthAttachments,
                mInputAttachments;
            Map<PassUID_t, Vector<uint64_t>>
                mAttachmentPassAssignment;
            Map<RenderGraphResourceId_t, uint64_t>
                mViewToImageAssignment;
        };

        /**
         * Describes common resource flags for read/write operations.
         */
        struct SHIRABE_TEST_EXPORT SRenderGraphResourceFlags
        {
        public_constructors:
            /**
             * Default-Construct resource flags.
             */
            SRenderGraphResourceFlags();

        public_members:
            RenderGraphFormat_t requiredFormat;
        };

        /**
         * Describes specialized constraints for texture resouces.
         */
        struct SHIRABE_TEST_EXPORT SRenderGraphTextureResourceFlags
                : public SRenderGraphResourceFlags
        {
        public_members:
            CRange arraySliceRange;
            CRange mipSliceRange;
        };

        /**
         * Describes flags required for reading textures.
         */
        struct SHIRABE_TEST_EXPORT SRenderGraphReadTextureFlags
                : public SRenderGraphTextureResourceFlags
        {
        public_members:
            ERenderGraphReadSource readSource;
        };

        /**
         * Describes flags required for writing textures.
         */
        struct SHIRABE_TEST_EXPORT SRenderGraphWriteTextureFlags
                : public SRenderGraphTextureResourceFlags
        {
        public_members:
            ERenderGraphWriteTarget writeTarget;
        };

        /**
         * Describes an adjacency list collection, mapping from a source id to a list of target ids.
         * Basically a list of lists of graph edges.
         */
        template <
                typename TUnderlyingIDFrom,
                typename TUnderlyingIDTo = TUnderlyingIDFrom
                >
        using AdjacencyListMap_t = std::unordered_map<TUnderlyingIDFrom, std::vector<TUnderlyingIDTo>>;

        struct SRenderGraphPipelineConfig
        {};

        //<-----------------------------------------------------------------------------
        // Renderable structures
        //<-----------------------------------------------------------------------------
        struct SRenderGraphPipeline
            : SRenderGraphResource
        {
        public_members:
            rhi::ResourceId_t          pipelineResourceId;
            SRenderGraphPipelineConfig pipelineConfiguration;
        };

        struct SRenderGraphMaterialDescription
        {
        public_members:
            rhi::ResourceId_t materialResourceId;
            rhi::ResourceId_t sharedMaterialResourceId;
            std::vector<SRenderGraphPersistentBufferDescription> buffers;
            std::vector<SRenderGraphPersistentImageDescription>  images;
        };

        struct SRenderGraphMaterial
            : SRenderGraphTypedResource<SRenderGraphMaterialDescription>
        {
            std::vector<SRenderGraphBufferView> buffers;
            std::vector<SRenderGraphImageView>  images;
        };

        struct SRenderGraphMeshInfo
        {
            uint32_t indexCount;
        };

        struct SRenderGraphMeshDescription
        {
            rhi::ResourceId_t meshResourceId;
        };

        struct SRenderGraphMesh
            : SRenderGraphTypedResource<SRenderGraphMeshDescription>
        {
            SRenderGraphMeshInfo meshInfo;
        };

        struct SRenderGraphRenderable
        {
            SRenderGraphMeshDescription                    mesh;
            std::array<SRenderGraphMaterialDescription, 4> materials;
        };

        struct SRenderGraphRenderObjectMaterial
        {
            SRenderGraphMaterial material;
            SRenderGraphPipeline pipeline;
        };

        struct SRenderGraphRenderObject
        {
            SRenderGraphMesh mesh;
            std::vector<SRenderGraphRenderObjectMaterial> materials;
        };

        #define SHIRABE_FRAMEGRAPH_SUPPORTED_RESOURCE_TYPES \
            SRenderGraphImage                               \
            , SRenderGraphRenderTarget                      \
            , SRenderGraphImageView                         \
            , SRenderGraphBuffer                            \
            , SRenderGraphBufferView                        \
            , SRenderGraphMesh                              \
            , SRenderGraphMaterial                          \
            , SRenderGraphPipeline


        /**
         * Describes a list of rendergraph resources.
         */
        using Index_t = Vector<Shared<SRenderGraphResource>>;

        /**
         * Describes a list of resource ids to index into a list of resources.
         */
        using RefIndex_t = Vector<RenderGraphResourceId_t>;

        /**
         * Wraps a RefIndex_t type for a specific resource type.
         *
         * @tparam T The internal resource type of the wrapper.
         */
        template <typename T>
        class CRenderGraphResourcesRef
        {
        protected:
            /**
             * Insert a new resource reference.
             *
             * @param aRef
             */
            void insert(RenderGraphResourceId_t const &aRef)
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
         * Variadic parameter pack unpacking helper to adapt a RenderGraphResourcesRef<T>
         * for each type provided in TTypes
         *
         * @tparam TTypes List of types to implement RenderGraphResourcesRef<T> for.
         */
        template <typename... TTypes>
        class CRenderGraphResourcesRefContainer
                : public CRenderGraphResourcesRef<TTypes>...
        {
        public_methods:
            SHIRABE_INLINE RefIndex_t const &images()      const { return CRenderGraphResourcesRef<SRenderGraphImage>::get();      }
            SHIRABE_INLINE RefIndex_t const &imageViews()  const { return CRenderGraphResourcesRef<SRenderGraphImageView>::get();  }
            SHIRABE_INLINE RefIndex_t const &buffers()     const { return CRenderGraphResourcesRef<SRenderGraphBuffer>::get();     }
            SHIRABE_INLINE RefIndex_t const &bufferViews() const { return CRenderGraphResourcesRef<SRenderGraphBufferView>::get(); }
            SHIRABE_INLINE RefIndex_t const &meshes()      const { return CRenderGraphResourcesRef<SRenderGraphMesh>::get();       }
            SHIRABE_INLINE RefIndex_t const &materials()   const { return CRenderGraphResourcesRef<SRenderGraphMaterial>::get();   }
            SHIRABE_INLINE RefIndex_t const &pipelines()   const { return CRenderGraphResourcesRef<SRenderGraphPipeline>::get();   }
        };

        using CRenderGraphResourceReferences_t = CRenderGraphResourcesRefContainer<SHIRABE_FRAMEGRAPH_SUPPORTED_RESOURCE_TYPES>;

        /**
         * Collection-type to store a list of resources and various resource-ref collections
         * for each supported type.
         */
        class CRenderGraphResources
                : public CRenderGraphResourceReferences_t
        {
            SHIRABE_DECLARE_LOG_TAG(CRenderGraphResources)

        public_constructors:
            CRenderGraphResources();

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
                getResource(RenderGraphResourceId_t const &aResourceId) const
            {
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(mResources.size() <= aResourceId)
                {
                    CLog::Error(logTag(), StaticStringHelpers::format("Resource w/ resource id {} not found."));
                    return { EEngineStatus::ResourceError_NotFound };
                }
#endif

                Shared<SRenderGraphResource> resource = mResources.at(aResourceId);
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(nullptr == resource)
                {
                    CLog::Error(logTag(), StaticStringHelpers::format("Resource w/ resource id {}: Handle is empty. Invalid resource."));
                    return { EEngineStatus::ResourceError_ResourceInvalid };
                }
#endif

                Shared<T> ptr = std::static_pointer_cast<T>(resource);
                return { EEngineStatus::Ok, ptr };
            }

            SHIRABE_INLINE Index_t const &resources() const { return mResources; }

        protected_members:
            Index_t mResources;
        };

        /**
         * Extends CRenderGraphResources so that mutable resource operations become possible. Also permits the creation of a new resource.
         */
        class CRenderGraphMutableResources
                : public CRenderGraphResources
        {
        public:
            /**
             * Spawn a new resource of type T inside this collection.
             *
             * @tparam T The type of resource to spawn.
             * @return   Returns a mutable reference to the newly created instance.
             */
            template <typename T> // with T : RenderGraphResource
            typename std::enable_if_t<std::is_base_of_v<SRenderGraphResource, T>, T> &spawnResource()
            {
                Shared<T> ptr = std::make_shared<T>();
                ptr->resourceId = mResources.size();

                mResources.push_back(ptr);

                static_cast<CRenderGraphResourcesRef<T> *>(this)->insert(ptr->resourceId);

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
            template <typename T> // with T : RenderGraphResource
            CEngineResult<Shared<T>>
            getResourceMutable(RenderGraphResourceId_t const &aResourceId)
            {
                CEngineResult<Shared<T>> const fetch  = static_cast<CRenderGraphResources *>(this)->getResource<T>(aResourceId);
                CEngineResult<Shared<T>>       result = { EEngineStatus::Ok };
                if(not fetch.resultEquals(EEngineStatus::ResourceError_NotFound))
                {
                   Shared<T> ptr = std::const_pointer_cast<T>(fetch.data());
                   result = CEngineResult<Shared<T>>(EEngineStatus::Ok, ptr);
                }

                return result;
            }

            /**
             * Merge two sets of rendergraph resources together.
             *
             * @param aOther Another set of resources to be merged with this one.
             * @return       True, if aOther was successfully merged into this instance. False otherwise.
             */
            bool mergeIn(CRenderGraphResources const &aOther);
        };

    }

    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::ERenderGraphResourceType>(framegraph::ERenderGraphResourceType const &aType);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::RenderGraphFormat_t>(framegraph::RenderGraphFormat_t const &aFormat);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::ERenderGraphResourceUsage>(framegraph::ERenderGraphResourceUsage const &aUsage);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::ERenderGraphWriteTarget>(framegraph::ERenderGraphWriteTarget const &aTarget);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::ERenderGraphResourceAccessibility>(framegraph::ERenderGraphResourceAccessibility const &aAccessibility);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::ERenderGraphResourceInitState>(framegraph::ERenderGraphResourceInitState const &aState);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::ERenderGraphViewAccessMode>(framegraph::ERenderGraphViewAccessMode const &aAccessMode);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::SRenderGraphImage>(framegraph::SRenderGraphImage const &aTexture);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::SRenderGraphImageView>(framegraph::SRenderGraphImageView const &aTextureView);
    template <>
    SHIRABE_TEST_EXPORT std::string convert_to_string<framegraph::SRenderGraphBuffer>(framegraph::SRenderGraphBuffer const &aBuffer);
    template <>
    SHIRABE_TEST_EXPORT  std::string convert_to_string<framegraph::SRenderGraphBufferView>(framegraph::SRenderGraphBufferView const &aBufferView);

}

#endif
