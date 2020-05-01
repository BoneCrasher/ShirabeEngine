#include "renderer/framegraph/framegraphdata.h"
#include <core/basictypes.h>

namespace engine
{
    using namespace framegraph;

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<EFrameGraphResourceType>(EFrameGraphResourceType const&type)
    {
        switch(type)
        {
        default:
        case EFrameGraphResourceType::Undefined:   return "Undefined";
        case EFrameGraphResourceType::Texture:     return "Texture";
        case EFrameGraphResourceType::Buffer:      return "Buffer";
        case EFrameGraphResourceType::TextureView: return "TextureView";
        case EFrameGraphResourceType::BufferView:  return "BufferView";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<FrameGraphFormat_t>(FrameGraphFormat_t const &aFormat)
    {
        switch(aFormat)
        {
        default:
        case FrameGraphFormat_t::Undefined:                return "Undefined";
        case FrameGraphFormat_t::Automatic:                return "Automatic";
        case FrameGraphFormat_t::Structured:               return "Structured";
        case FrameGraphFormat_t::R8_TYPELESS:              return "R8_TYPELESS";
        case FrameGraphFormat_t::R8_SINT:                  return "R8_SINT";
        case FrameGraphFormat_t::R8_UINT:                  return "R8_UINT";
        case FrameGraphFormat_t::R8_SNORM:                 return "R8_SNORM";
        case FrameGraphFormat_t::R8_UNORM:                 return "R8_UNORM";
        case FrameGraphFormat_t::R8G8B8A8_TYPELESS:        return "R8G8B8A8_TYPELESS";
        case FrameGraphFormat_t::R8G8B8A8_SINT:            return "R8G8B8A8_SINT";
        case FrameGraphFormat_t::R8G8B8A8_UINT:            return "R8G8B8A8_UINT";
        case FrameGraphFormat_t::R8G8B8A8_SNORM:           return "R8G8B8A8_SNORM";
        case FrameGraphFormat_t::R8G8B8A8_UNORM:           return "R8G8B8A8_UNORM";
        case FrameGraphFormat_t::R8G8B8A8_UNORM_SRGB:      return "R8G8B8A8_UNORM_SRGB";
        case FrameGraphFormat_t::R8G8B8A8_FLOAT:           return "R8G8B8A8_FLOAT";
        case FrameGraphFormat_t::B8G8R8A8_TYPELESS:        return "B8G8R8A8_TYPELESS";
        case FrameGraphFormat_t::B8G8R8A8_SINT:            return "B8G8R8A8_SINT";
        case FrameGraphFormat_t::B8G8R8A8_UINT:            return "B8G8R8A8_UINT";
        case FrameGraphFormat_t::B8G8R8A8_SNORM:           return "B8G8R8A8_SNORM";
        case FrameGraphFormat_t::B8G8R8A8_UNORM:           return "B8G8R8A8_UNORM";
        case FrameGraphFormat_t::B8G8R8A8_UNORM_SRGB:      return "B8G8R8A8_UNORM_SRGB";
        case FrameGraphFormat_t::B8G8R8A8_FLOAT:           return "B8G8R8A8_FLOAT";
        case FrameGraphFormat_t::R16_TYPELESS:             return "R16_TYPELESS";
        case FrameGraphFormat_t::R16_SINT:                 return "R16_SINT";
        case FrameGraphFormat_t::R16_UINT:                 return "R16_UINT";
        case FrameGraphFormat_t::R16_SNORM:                return "R16_SNORM";
        case FrameGraphFormat_t::R16_UNORM:                return "R16_UNORM";
        case FrameGraphFormat_t::R16_FLOAT:                return "R16_FLOAT";
        case FrameGraphFormat_t::R16G16B16A16_TYPELESS:    return "R16G16B16A16_TYPELESS";
        case FrameGraphFormat_t::R16G16B16A16_SINT:        return "R16G16B16A16_SINT";
        case FrameGraphFormat_t::R16G16B16A16_UINT:        return "R16G16B16A16_UINT";
        case FrameGraphFormat_t::R16G16B16A16_SNORM:       return "R16G16B16A16_SNORM";
        case FrameGraphFormat_t::R16G16B16A16_UNORM:       return "R16G16B16A16_UNORM";
        case FrameGraphFormat_t::R16G16B16A16_FLOAT:       return "R16G16B16A16_FLOAT";
        case FrameGraphFormat_t::R24_UNORM_X8_TYPELESS:    return "R24_UNORM_X8_TYPELESS";
        case FrameGraphFormat_t::R32_TYPELESS:             return "R32_TYPELESS";
        case FrameGraphFormat_t::R32_SINT:                 return "R32_SINT";
        case FrameGraphFormat_t::R32_UINT:                 return "R32_UINT";
        case FrameGraphFormat_t::R32_SNORM:                return "R32_SNORM";
        case FrameGraphFormat_t::R32_UNORM:                return "R32_UNORM";
        case FrameGraphFormat_t::R32_FLOAT:                return "R32_FLOAT";
        case FrameGraphFormat_t::R32_FLOAT_S8X24_TYPELESS: return "R32_FLOAT_S8X24_TYPELESS";
        case FrameGraphFormat_t::R32G32B32A32_TYPELESS:    return "R32G32B32A32_TYPELESS";
        case FrameGraphFormat_t::R32G32B32A32_SINT:        return "R32G32B32A32_SINT";
        case FrameGraphFormat_t::R32G32B32A32_UINT:        return "R32G32B32A32_UINT";
        case FrameGraphFormat_t::D24_UNORM_S8_UINT:        return "D24_UNORM_S8_UINT";
        case FrameGraphFormat_t::D32_FLOAT:                return "D32_FLOAT";
        case FrameGraphFormat_t::D32_FLOAT_S8X24_UINT:     return "D32_FLOAT_S8X24_UINT";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::EFrameGraphResourceUsage>(framegraph::EFrameGraphResourceUsage const &aUsage)
    {
        switch(aUsage)
        {
            case EFrameGraphResourceUsage::Undefined      : return "Undefined";
            case EFrameGraphResourceUsage::InputAttachment: return "InputAttachment";
            case EFrameGraphResourceUsage::ColorAttachment: return "ColorAttachment";
            case EFrameGraphResourceUsage::DepthAttachment: return "DepthAttachment";
            case EFrameGraphResourceUsage::SampledImage   : return "SampledImage";
            case EFrameGraphResourceUsage::StorageImage   : return "StorageImage";
            case EFrameGraphResourceUsage::BufferResource : return "BufferResource";
            case EFrameGraphResourceUsage::BufferTarget   : return "BufferTarget";
            case EFrameGraphResourceUsage::Unused         : return "Unused";
            default                                       : return "Unknown";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::EFrameGraphWriteTarget>(framegraph::EFrameGraphWriteTarget const &aTarget)
    {
        SHIRABE_UNUSED(aTarget);

        return "";
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::EFrameGraphResourceAccessibility>(framegraph::EFrameGraphResourceAccessibility const &aAccessibility)
    {
        SHIRABE_UNUSED(aAccessibility);

        return "";
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::EFrameGraphResourceInitState>(framegraph::EFrameGraphResourceInitState const &aState)
    {
        switch(aState)
        {
            default:
            case EFrameGraphResourceInitState::Undefined: return "Undefined";
            case EFrameGraphResourceInitState::Clear:     return "Clear";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::EFrameGraphViewAccessMode>(framegraph::EFrameGraphViewAccessMode const &aAccessMode)
    {
        SHIRABE_UNUSED(aAccessMode);

        return "";
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        std::ostream& operator<<(std::ostream &strm, EFrameGraphResourceType const&e)
        {
            strm << convert_to_string(e).c_str();
            return strm;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::ostream& operator<<(std::ostream &strm, FrameGraphFormat_t const&e)
        {
            strm << convert_to_string(e).c_str();
            return strm;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool validateFormatCompatibility(
                FrameGraphFormat_t const &aBase,
                FrameGraphFormat_t const &aDerived)
        {
            SHIRABE_UNUSED(aBase);
            SHIRABE_UNUSED(aDerived);

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SFrameGraphResource::SFrameGraphResource()
            : referenceCount(0)
            , assignedPassUID(0)
            , resourceId(FrameGraphResourceId_t{ })
            , parentResource(0)
            , subjacentResource(0)
            , readableName("")
            , type(EFrameGraphResourceType::Undefined)
            , isExternalResource(false)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SFrameGraphBuffer::SFrameGraphBuffer()
            : SFrameGraphResource()
            , bufferUsage(VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM)
            , sizeInBytes(0)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SFrameGraphBufferView::SFrameGraphBufferView()
            : SFrameGraphResource()
            , subrange(CRange(0, 0))
            , format(FrameGraphFormat_t::Undefined)
            , mode(EFrameGraphViewAccessMode::Undefined)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SFrameGraphTexture::SFrameGraphTexture()
            : SFrameGraphResource()
            , STextureInfo()
            , initialState(EFrameGraphResourceInitState::Undefined)
            , permittedUsage(EFrameGraphResourceUsage::Undefined)
            , requestedUsage(EFrameGraphResourceUsage::Undefined)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void SFrameGraphTexture::assignTextureParameters(SFrameGraphTexture const &aOther)
        {
            graphicsapi::STextureInfo::operator=(aOther); // Use implicitly defined assignment op

            initialState   = aOther.initialState;
            permittedUsage = aOther.permittedUsage;
            requestedUsage = aOther.requestedUsage;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool SFrameGraphTexture::validate() const
        {
            bool const dimensionsValid = (0 == width || not (0 == width || 0 == height || 0 == depth));
            bool const mipLevelsValid  = (1 <= mipLevels);
            bool const arraySizeValid  = (1 <= arraySize);

            return (dimensionsValid && mipLevelsValid && arraySizeValid);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SFrameGraphTextureView::SFrameGraphTextureView()
            : SFrameGraphResource()
            , arraySliceRange(CRange(0, 0))
            , mipSliceRange(CRange(0, 0))
            , format(FrameGraphFormat_t::Undefined)
            , mode(EFrameGraphViewAccessMode::Undefined)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        uint32_t SFrameGraphAttachmentCollection::addAttachment(
                PassUID_t              const &aPassUID,
                FrameGraphResourceId_t const &aImageResourceID,
                FrameGraphResourceId_t const &aImageViewResourceID)
        {
            uint64_t imageViewIndex = 0;

            auto const it2 = std::find(  mAttachmentImageViewResourceIds.begin()
                                       , mAttachmentImageViewResourceIds.end()
                                       , aImageViewResourceID);
            if(mAttachmentImageViewResourceIds.end() == it2)
            {
                mAttachmentImageViewResourceIds.push_back(aImageViewResourceID);
                mAttachmentImageResourceIds    .push_back(aImageResourceID);

                mViewToImageAssignment[aImageViewResourceID] = (mAttachmentImageResourceIds.size() - 1);

                imageViewIndex = (mAttachmentImageViewResourceIds.size() - 1);
            }
            else
            {
                imageViewIndex = std::distance(mAttachmentImageViewResourceIds.begin(), it2);
            }

            mAttachmentPassAssignment[aPassUID].push_back(imageViewIndex);

            return imageViewIndex;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        void SFrameGraphAttachmentCollection::addInputAttachment(
                PassUID_t              const &aPassUID,
                FrameGraphResourceId_t const &aImageResourceID,
                FrameGraphResourceId_t const &aImageViewResourceID)
        {
            uint32_t const imageViewIndex = addAttachment(aPassUID, aImageResourceID, aImageViewResourceID);
            mInputAttachments.push_back(imageViewIndex);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void SFrameGraphAttachmentCollection::addColorAttachment(
                PassUID_t              const &aPassUID,
                FrameGraphResourceId_t const &aImageResourceID,
                FrameGraphResourceId_t const &aImageViewResourceID)
        {
            uint32_t const imageViewIndex = addAttachment(aPassUID, aImageResourceID, aImageViewResourceID);
            mColorAttachments.push_back(imageViewIndex);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void SFrameGraphAttachmentCollection::addDepthAttachment(
                PassUID_t              const &aPassUID,
                FrameGraphResourceId_t const &aImageResourceID,
                FrameGraphResourceId_t const &aImageViewResourceID)
        {
            uint32_t const imageViewIndex = addAttachment(aPassUID, aImageResourceID, aImageViewResourceID);
            mDepthAttachments.push_back(imageViewIndex);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SFrameGraphResourceFlags::SFrameGraphResourceFlags()
            : requiredFormat(FrameGraphFormat_t::Undefined)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool operator<(
                SFrameGraphResource const &aLHS,
                SFrameGraphResource const &aRHS)
        {
            return (aLHS.resourceId < aRHS.resourceId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool operator!=(
                SFrameGraphResource const &aLHS,
                SFrameGraphResource const &aRHS)
        {
            return (aLHS.resourceId != aRHS.resourceId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CFrameGraphResources::CFrameGraphResources()
        {
            mResources.push_back(std::make_shared<SFrameGraphResource>());
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void appendToVector(
                std::vector<T>       &aOutTarget,
                std::vector<T> const &aSource)
        {
            aSource.resize(aOutTarget.size() + aSource.size());

            for(T const &s : aSource)
            {
                aOutTarget.push_back(s);
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CFrameGraphMutableResources::mergeIn(CFrameGraphResources const &aOther)
        {
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
            try {
#endif
                mResources = aOther.resources();

                for(RefIndex_t::value_type const&id : aOther.textures())
                {
                    CFrameGraphResourcesRef<SFrameGraphTexture>::insert(id);
                }

                for(RefIndex_t::value_type const&id : aOther.textureViews())
                {
                    CFrameGraphResourcesRef<SFrameGraphTextureView>::insert(id);
                }

                for(RefIndex_t::value_type const&id : aOther.buffers())
                {
                    CFrameGraphResourcesRef<SFrameGraphBuffer>::insert(id);
                }

                for(RefIndex_t::value_type const&id : aOther.bufferViews())
                {
                    CFrameGraphResourcesRef<SFrameGraphBufferView>::insert(id);
                }

                for(RefIndex_t::value_type const&id : aOther.renderablesLists())
                {
                    CFrameGraphResourcesRef<SFrameGraphRenderableList>::insert(id);
                }

                for(RefIndex_t::value_type const&id : aOther.renderableListViews())
                {
                    CFrameGraphResourcesRef<SFrameGraphRenderableListView>::insert(id);
                }

                for(RefIndex_t::value_type const &id : aOther.meshes())
                {
                    CFrameGraphResourcesRef<SFrameGraphMesh>::insert(id);
                }

                for(RefIndex_t::value_type const &id : aOther.materials())
                {
                    CFrameGraphResourcesRef<SFrameGraphMaterial>::insert(id);
                }

                mAttachements = aOther.attachements();

                return true;

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
            }
            catch(std::runtime_error const &aRTE)
            {
                SHIRABE_UNUSED(aRTE);
                return false;
            }
            catch(...)
            {
                return false;
            }
#endif
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::SFrameGraphTexture>(framegraph::SFrameGraphTexture const &aTexture)
    {
        std::string s =
                CString::format(
                    "  {} (RID: {})"
                    "\n    Name:            {}"
                    "\n    Sizes:           {} x {} x {}"
                    "\n    Format:          {}"
                    "\n    Array-Levels:    {}"
                    "\n    Mip-Levels:      {}"
                    "\n    Initial-State:   {}"
                    "\n    Reference-Count: {}0",
                    "Texture",
                    aTexture.resourceId,
                    aTexture.readableName,
                    aTexture.width, aTexture.height, aTexture.depth,
                    convert_to_string(aTexture.format),
                    aTexture.arraySize,
                    aTexture.mipLevels,
                    convert_to_string(aTexture.initialState),
                    aTexture.referenceCount);
        return s;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::SFrameGraphTextureView>(framegraph::SFrameGraphTextureView const &aTextureView)
    {
        bool viewIsReadMode  = aTextureView.mode.check(EFrameGraphViewAccessMode::Read);

        std::string s =
                CString::format(
                    "  {} (RID: {})"
                    "\n    SubjacentResourceId: {}"
                    "\n    Mode:                {}"
                    "\n    Format:              {}"
                    "\n    ArrayRange:          {}"
                    "\n    MipRange:            {}"
                    "\n    Reference-Count:     {}",
                    "TextureView",
                    aTextureView.resourceId,
                    aTextureView.subjacentResource,
                    (viewIsReadMode ? "Read" : "Write"),
                    convert_to_string(aTextureView.format),
                    convert_to_string(aTextureView.arraySliceRange),
                    convert_to_string(aTextureView.mipSliceRange),
                    aTextureView.referenceCount);
        return s;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::SFrameGraphBuffer>(framegraph::SFrameGraphBuffer const &aBuffer)
    {
        std::string s =
                CString::format(
                    "  {} (RID: {})",
                    "Buffer",
                    aBuffer.resourceId);
        return s;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::SFrameGraphBufferView>(framegraph::SFrameGraphBufferView const &aBufferView)
    {
        std::string s =
                CString::format(
                    "  {} (RID: {})",
                    "BufferView",
                    aBufferView.resourceId);
        return s;
    }
    //<-----------------------------------------------------------------------------
}
