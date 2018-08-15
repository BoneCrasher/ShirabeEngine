#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPHDATA_H__

#include <stdint.h>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <variant>
#include <memory>
#include <string.h>

// #include <better-enums/enum.h>

#include <platform/platform.h>
#include <core/bitfield.h>
#include <core/basictypes.h>
#include <graphicsapi/resources/types/definition.h>
#include <graphicsapi/resources/types/texture.h>
#include <graphicsapi/resources/types/textureview.h>
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

        SHIRABE_DECLARE_LIST_OF_TYPE(FrameGraphResourceId_t, FrameGraphResourceId);

        /**
         * The FrameGraphResourceType enum describes Values that represent top-level frame graph resource types
         */
        enum class EFrameGraphResourceType
                : uint8_t
        {
            Undefined = 0,
            Imported  = 1,
            Texture,
            Buffer,
            TextureView,
            BufferView,
            RenderableList,
            RenderableListView // To support renderable filtering...
        };

        /**
         * Values that represent texture and buffer formats
         */
        using FrameGraphFormat_t = engine::resources::EFormat;

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
            Undefined      =  0,
            ImageResource  =  1,
            BufferResource =  2,
            RenderTarget   =  4,
            DepthTarget    =  8,
            BufferTarget   = 16
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
         * The EFrameGraphViewSource enum describes the pipeline data source of a texture view.
         */
        enum class EFrameGraphViewSource
                : uint8_t
        {
            Undefined = 0,
            Color,
            Depth
        };

        /**
         * The FrameGraphViewAccessMode enum describes in which access modes a texture view may operate.
         */
        enum class EFrameGraphViewAccessMode
                : uint8_t
        {
            Undefined = 0,
            Read,
            Write
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
            SHIRABE_INLINE operator FrameGraphResourceId_t()
            {
                return resourceId;
            }

        public_members:
            uint32_t               referenceCount;
            PassUID_t              assignedPassUID;
            FrameGraphResourceId_t resourceId;
            FrameGraphResourceId_t parentResource;
            FrameGraphResourceId_t subjacentResource;
            std::string            readableName;
            EFrameGraphResourceType type;
            bool                   isExternalResource;
        };

        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphResource, SFrameGraphResource);

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
            uint32_t elementSize;
            uint32_t elementCount;
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

        struct SHIRABE_TEST_EXPORT SFrameGraphTexture
                : public SFrameGraphResource
                , public gfxapi::STextureInfo
        {
        public_constructors:
            /**
             * Default-Construt a frame graph texture
             */
            SFrameGraphTexture();

        public_methods:
            /**
             * Copy-Over the texture info data from another framegraph texture.
             *
             * @param aOther The other texture to copy the texture information from.
             */
            void assignTextureParameters(SFrameGraphTexture const &aOther);

            /**
             * Validate the texture data.
             *
             * @return True if valid. False otherwise.
             */
            virtual bool validate() const;

        public_members:
            EFrameGraphResourceInitState        initialState;
            CBitField<EFrameGraphResourceUsage> permittedUsage;
            CBitField<EFrameGraphResourceUsage> requestedUsage;

        };

        SHIRABE_DECLARE_LIST_OF_TYPE(SFrameGraphTexture, SFrameGraphTexture);
        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphTexture, SFrameGraphTexture);

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
            EFrameGraphViewSource                source;
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(SFrameGraphTextureView, SFrameGraphTextureView);
        SHIRABE_DECLARE_MAP_OF_TYPES(FrameGraphResourceId_t, SFrameGraphTextureView, SFrameGraphTextureView);

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
         * Describes flags required for reading textures.
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphReadTextureFlags
                : public SFrameGraphResourceFlags
        {
        public_members:
            EFrameGraphReadSource source;
        };

        /**
         * Describes flags required for writing textures.
         */
        struct SHIRABE_TEST_EXPORT SFrameGraphWriteTextureFlags
                : public SFrameGraphResourceFlags
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

        #define SHIRABE_FRAMEGRAPH_SUPPORTED_RESOURCE_TYPES  \
            SFrameGraphTexture,                              \
            SFrameGraphTextureView,                          \
            SFrameGraphBuffer,                               \
            SFrameGraphBufferView,                           \
            SFrameGraphRenderableList,                       \
            SFrameGraphRenderableListView


        /**
         * Describes a list of framegraph resources.
         */
        using Index_t = Vector<CStdSharedPtr_t<SFrameGraphResource>>;

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
            RefIndex_t const &get() const { return m_index; }

            /**
             * Return the current index state mutably.
             *
             * @return See brief.
             */
            RefIndex_t &getMutable() { return m_index; }

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
            CStdSharedPtr_t<typename std::enable_if_t<std::is_base_of_v<SFrameGraphResource, T>, T>> const
            get(FrameGraphResourceId_t const &aResourceId) const
            {
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(m_resources.size() <= aResourceId)
                    throw std::runtime_error("Resource handle not found.");
#endif

                CStdSharedPtr_t<SFrameGraphResource> resource = m_resources.at(aResourceId);
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
                if(nullptr == resource)
                    throw std::runtime_error("Resource handle is empty.");
#endif

                CStdSharedPtr_t<T> ptr = std::static_pointer_cast<T>(resource);
                return ptr;
            }

            SHIRABE_INLINE Index_t    const &resources()           const { return m_resources; }
            SHIRABE_INLINE RefIndex_t const &textures()            const { return CFrameGraphResourcesRef<SFrameGraphTexture>::get(); }
            SHIRABE_INLINE RefIndex_t const &textureViews()        const { return CFrameGraphResourcesRef<SFrameGraphTextureView>::get(); }
            SHIRABE_INLINE RefIndex_t const &buffers()             const { return CFrameGraphResourcesRef<SFrameGraphBuffer>::get(); }
            SHIRABE_INLINE RefIndex_t const &bufferViews()         const { return CFrameGraphResourcesRef<SFrameGraphBufferView>::get(); }
            SHIRABE_INLINE RefIndex_t const &renderablesLists()    const { return CFrameGraphResourcesRef<SFrameGraphRenderableList>::get(); }
            SHIRABE_INLINE RefIndex_t const &renderableListViews() const { return CFrameGraphResourcesRef<SFrameGraphRenderableListView>::get(); }

        protected_members:
            Index_t m_resources;
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
            typename std::enable_if_t<std::is_base_of_v<SFrameGraphResource, T>, T>&
            spawnResource()
            {
                CStdSharedPtr_t<T> ptr = std::make_shared<T>();
                ptr->resourceId = m_resources.size();

                m_resources.push_back(ptr);

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
            CStdSharedPtr_t<typename std::enable_if_t<std::is_base_of_v<SFrameGraphResource, T>, T>>
            getMutable(FrameGraphResourceId_t const &aResourceId)
            {
                CStdSharedPtr_t<T> ptr = *const_cast<CStdSharedPtr_t<T>*>(&static_cast<CFrameGraphResources*>(this)->get<T>(aResourceId));
                return ptr;
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
    SHIRABE_TEST_EXPORT std::string to_string<framegraph::EFrameGraphResourceType>(framegraph::EFrameGraphResourceType const &aType);
    template <>
    SHIRABE_TEST_EXPORT std::string to_string<framegraph::FrameGraphFormat_t>(framegraph::FrameGraphFormat_t const &aFormat);
    template <>
    SHIRABE_TEST_EXPORT std::string to_string<framegraph::EFrameGraphResourceUsage>(framegraph::EFrameGraphResourceUsage const &aUsage);
    template <>
    SHIRABE_TEST_EXPORT std::string to_string<framegraph::EFrameGraphWriteTarget>(framegraph::EFrameGraphWriteTarget const &aTarget);
    template <>
    SHIRABE_TEST_EXPORT std::string to_string<framegraph::EFrameGraphResourceAccessibility>(framegraph::EFrameGraphResourceAccessibility const &aAccessibility);
    template <>
    SHIRABE_TEST_EXPORT std::string to_string<framegraph::EFrameGraphResourceInitState>(framegraph::EFrameGraphResourceInitState const &aState);
    template <>
    SHIRABE_TEST_EXPORT std::string to_string<framegraph::EFrameGraphViewAccessMode>(framegraph::EFrameGraphViewAccessMode const &aAccessMode);


    template <>
    SHIRABE_TEST_EXPORT std::string to_string<framegraph::SFrameGraphTexture>(framegraph::SFrameGraphTexture const &aTexture);
    template <>
    SHIRABE_TEST_EXPORT std::string to_string<framegraph::SFrameGraphTextureView>(framegraph::SFrameGraphTextureView const &aTextureView);
    template <>
    SHIRABE_TEST_EXPORT std::string to_string<framegraph::SFrameGraphBuffer>(framegraph::SFrameGraphBuffer const &aBuffer);
    template <>
    SHIRABE_TEST_EXPORT  std::string to_string<framegraph::SFrameGraphBufferView>(framegraph::SFrameGraphBufferView const &aBufferView);

}

#endif
