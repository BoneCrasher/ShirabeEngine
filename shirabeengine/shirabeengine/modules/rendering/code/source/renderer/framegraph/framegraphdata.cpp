#include "renderer/framegraph/framegraphdata.h"
#include <core/basictypes.h>

namespace engine
{
    using namespace framegraph;

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<ERenderGraphResourceType>(ERenderGraphResourceType const&type)
    {
        switch(type)
        {
        default:
        case ERenderGraphResourceType::Undefined:   return "Undefined";
        case ERenderGraphResourceType::Image:       return "Image";
        case ERenderGraphResourceType::Buffer:      return "Buffer";
        case ERenderGraphResourceType::ImageView:   return "ImageView";
        case ERenderGraphResourceType::BufferView:  return "BufferView";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<RenderGraphFormat_t>(RenderGraphFormat_t const &aFormat)
    {
        switch(aFormat)
        {
        default:
        case RenderGraphFormat_t::Undefined:                return "Undefined";
        case RenderGraphFormat_t::Automatic:                return "Automatic";
        case RenderGraphFormat_t::Structured:               return "Structured";
        case RenderGraphFormat_t::R8_TYPELESS:              return "R8_TYPELESS";
        case RenderGraphFormat_t::R8_SINT:                  return "R8_SINT";
        case RenderGraphFormat_t::R8_UINT:                  return "R8_UINT";
        case RenderGraphFormat_t::R8_SNORM:                 return "R8_SNORM";
        case RenderGraphFormat_t::R8_UNORM:                 return "R8_UNORM";
        case RenderGraphFormat_t::R8G8B8A8_TYPELESS:        return "R8G8B8A8_TYPELESS";
        case RenderGraphFormat_t::R8G8B8A8_SINT:            return "R8G8B8A8_SINT";
        case RenderGraphFormat_t::R8G8B8A8_UINT:            return "R8G8B8A8_UINT";
        case RenderGraphFormat_t::R8G8B8A8_SNORM:           return "R8G8B8A8_SNORM";
        case RenderGraphFormat_t::R8G8B8A8_UNORM:           return "R8G8B8A8_UNORM";
        case RenderGraphFormat_t::R8G8B8A8_UNORM_SRGB:      return "R8G8B8A8_UNORM_SRGB";
        case RenderGraphFormat_t::R8G8B8A8_FLOAT:           return "R8G8B8A8_FLOAT";
        case RenderGraphFormat_t::B8G8R8A8_TYPELESS:        return "B8G8R8A8_TYPELESS";
        case RenderGraphFormat_t::B8G8R8A8_SINT:            return "B8G8R8A8_SINT";
        case RenderGraphFormat_t::B8G8R8A8_UINT:            return "B8G8R8A8_UINT";
        case RenderGraphFormat_t::B8G8R8A8_SNORM:           return "B8G8R8A8_SNORM";
        case RenderGraphFormat_t::B8G8R8A8_UNORM:           return "B8G8R8A8_UNORM";
        case RenderGraphFormat_t::B8G8R8A8_UNORM_SRGB:      return "B8G8R8A8_UNORM_SRGB";
        case RenderGraphFormat_t::B8G8R8A8_FLOAT:           return "B8G8R8A8_FLOAT";
        case RenderGraphFormat_t::R16_TYPELESS:             return "R16_TYPELESS";
        case RenderGraphFormat_t::R16_SINT:                 return "R16_SINT";
        case RenderGraphFormat_t::R16_UINT:                 return "R16_UINT";
        case RenderGraphFormat_t::R16_SNORM:                return "R16_SNORM";
        case RenderGraphFormat_t::R16_UNORM:                return "R16_UNORM";
        case RenderGraphFormat_t::R16_FLOAT:                return "R16_FLOAT";
        case RenderGraphFormat_t::R16G16B16A16_TYPELESS:    return "R16G16B16A16_TYPELESS";
        case RenderGraphFormat_t::R16G16B16A16_SINT:        return "R16G16B16A16_SINT";
        case RenderGraphFormat_t::R16G16B16A16_UINT:        return "R16G16B16A16_UINT";
        case RenderGraphFormat_t::R16G16B16A16_SNORM:       return "R16G16B16A16_SNORM";
        case RenderGraphFormat_t::R16G16B16A16_UNORM:       return "R16G16B16A16_UNORM";
        case RenderGraphFormat_t::R16G16B16A16_FLOAT:       return "R16G16B16A16_FLOAT";
        case RenderGraphFormat_t::R24_UNORM_X8_TYPELESS:    return "R24_UNORM_X8_TYPELESS";
        case RenderGraphFormat_t::R32_TYPELESS:             return "R32_TYPELESS";
        case RenderGraphFormat_t::R32_SINT:                 return "R32_SINT";
        case RenderGraphFormat_t::R32_UINT:                 return "R32_UINT";
        case RenderGraphFormat_t::R32_SNORM:                return "R32_SNORM";
        case RenderGraphFormat_t::R32_UNORM:                return "R32_UNORM";
        case RenderGraphFormat_t::R32_FLOAT:                return "R32_FLOAT";
        case RenderGraphFormat_t::R32_FLOAT_S8X24_TYPELESS: return "R32_FLOAT_S8X24_TYPELESS";
        case RenderGraphFormat_t::R32G32B32A32_TYPELESS:    return "R32G32B32A32_TYPELESS";
        case RenderGraphFormat_t::R32G32B32A32_SINT:        return "R32G32B32A32_SINT";
        case RenderGraphFormat_t::R32G32B32A32_UINT:        return "R32G32B32A32_UINT";
        case RenderGraphFormat_t::D24_UNORM_S8_UINT:        return "D24_UNORM_S8_UINT";
        case RenderGraphFormat_t::D32_FLOAT:                return "D32_FLOAT";
        case RenderGraphFormat_t::D32_FLOAT_S8X24_UINT:     return "D32_FLOAT_S8X24_UINT";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::ERenderGraphResourceUsage>(framegraph::ERenderGraphResourceUsage const &aUsage)
    {
        switch(aUsage)
        {
            case ERenderGraphResourceUsage::Undefined      : return "Undefined";
            case ERenderGraphResourceUsage::InputAttachment: return "InputAttachment";
            case ERenderGraphResourceUsage::ColorAttachment: return "ColorAttachment";
            case ERenderGraphResourceUsage::DepthAttachment: return "DepthAttachment";
            case ERenderGraphResourceUsage::SampledImage   : return "SampledImage";
            case ERenderGraphResourceUsage::StorageImage   : return "StorageImage";
            case ERenderGraphResourceUsage::BufferResource : return "BufferResource";
            case ERenderGraphResourceUsage::BufferTarget   : return "BufferTarget";
            case ERenderGraphResourceUsage::Unused         : return "Unused";
            default                                       : return "Unknown";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::ERenderGraphWriteTarget>(framegraph::ERenderGraphWriteTarget const &aTarget)
    {
        SHIRABE_UNUSED(aTarget);

        return "";
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::ERenderGraphResourceAccessibility>(framegraph::ERenderGraphResourceAccessibility const &aAccessibility)
    {
        SHIRABE_UNUSED(aAccessibility);

        return "";
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::ERenderGraphResourceInitState>(framegraph::ERenderGraphResourceInitState const &aState)
    {
        switch(aState)
        {
            default:
            case ERenderGraphResourceInitState::Undefined: return "Undefined";
            case ERenderGraphResourceInitState::Clear:     return "Clear";
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::ERenderGraphViewAccessMode>(framegraph::ERenderGraphViewAccessMode const &aAccessMode)
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
        std::ostream& operator<<(std::ostream &strm, ERenderGraphResourceType const&e)
        {
            strm << convert_to_string(e).c_str();
            return strm;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::ostream& operator<<(std::ostream &strm, RenderGraphFormat_t const&e)
        {
            strm << convert_to_string(e).c_str();
            return strm;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool validateFormatCompatibility(
                RenderGraphFormat_t const &aBase,
                RenderGraphFormat_t const &aDerived)
        {
            SHIRABE_UNUSED(aBase);
            SHIRABE_UNUSED(aDerived);

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphResource::SRenderGraphResource()
            : referenceCount(0)
            , assignedPassUID(0)
            , resourceId(RenderGraphResourceId_t{ })
            , parentResource(0)
            , subjacentResource(0)
            , readableName("")
            , type(ERenderGraphResourceType::Undefined)
            , isExternalResource(false)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphDynamicBufferDescription::SRenderGraphDynamicBufferDescription()
            : bufferUsage(VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM)
              , sizeInBytes(0)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphPersistentBufferDescription::SRenderGraphPersistentBufferDescription()
            : bufferResourceId()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphBufferDescription::SRenderGraphBufferDescription()
            : isDynamicBuffer(false)
            , dynamicBuffer()
            , persistentBuffer()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphBuffer::SRenderGraphBuffer()
            : SRenderGraphTypedResource<SRenderGraphBufferDescription>()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphBufferViewDescription::SRenderGraphBufferViewDescription()
            : subrange(CRange(0, 0))
              , format(RenderGraphFormat_t::Undefined)
              , mode(ERenderGraphViewAccessMode::Undefined)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphBufferView::SRenderGraphBufferView()
            : SRenderGraphTypedResource<SRenderGraphBufferViewDescription>()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphDynamicImageDescription::SRenderGraphDynamicImageDescription()
            : STextureInfo()
              , initialState(ERenderGraphResourceInitState::Undefined)
              , permittedUsage(ERenderGraphResourceUsage::Undefined)
              , requestedUsage(ERenderGraphResourceUsage::Undefined)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphDynamicImageDescription::SRenderGraphDynamicImageDescription(STextureInfo const &aTextureInfo)
            : STextureInfo(aTextureInfo)
              , initialState(ERenderGraphResourceInitState::Undefined)
              , permittedUsage(ERenderGraphResourceUsage::Undefined)
              , requestedUsage(ERenderGraphResourceUsage::Undefined)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphPersistentImageDescription::SRenderGraphPersistentImageDescription()
            : imageId()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphImageDescription::SRenderGraphImageDescription()
            : isDynamicImage(false)
            , dynamicImage()
            , persistentImage()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool SRenderGraphImage::validate() const
        {
            bool const dimensionsValid = (0 == description.dynamicImage.width || not (0 == description.dynamicImage.width || 0 == description.dynamicImage.height || 0 == description.dynamicImage.depth));
            bool const mipLevelsValid  = (1 <= description.dynamicImage.mipLevels);
            bool const arraySizeValid  = (1 <= description.dynamicImage.arraySize);

            return (dimensionsValid && mipLevelsValid && arraySizeValid);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphImageViewDescription::SRenderGraphImageViewDescription()
            : arraySliceRange(CRange(0, 0))
              , mipSliceRange(CRange(0, 0))
              , format(RenderGraphFormat_t::Undefined)
              , mode(ERenderGraphViewAccessMode::Undefined)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphImageView::SRenderGraphImageView()
            : SRenderGraphTypedResource<SRenderGraphImageViewDescription>()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        uint32_t SRenderGraphAttachmentCollection::addAttachment(
                PassUID_t              const &aPassUID,
                RenderGraphResourceId_t const &aImageResourceID,
                RenderGraphResourceId_t const &aImageViewResourceID)
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
        void SRenderGraphAttachmentCollection::addInputAttachment(
                PassUID_t              const &aPassUID,
                RenderGraphResourceId_t const &aImageResourceID,
                RenderGraphResourceId_t const &aImageViewResourceID)
        {
            uint32_t const imageViewIndex = addAttachment(aPassUID, aImageResourceID, aImageViewResourceID);
            mInputAttachments.push_back(imageViewIndex);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void SRenderGraphAttachmentCollection::addColorAttachment(
                PassUID_t              const &aPassUID,
                RenderGraphResourceId_t const &aImageResourceID,
                RenderGraphResourceId_t const &aImageViewResourceID)
        {
            uint32_t const imageViewIndex = addAttachment(aPassUID, aImageResourceID, aImageViewResourceID);
            mColorAttachments.push_back(imageViewIndex);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void SRenderGraphAttachmentCollection::addDepthAttachment(
                PassUID_t              const &aPassUID,
                RenderGraphResourceId_t const &aImageResourceID,
                RenderGraphResourceId_t const &aImageViewResourceID)
        {
            uint32_t const imageViewIndex = addAttachment(aPassUID, aImageResourceID, aImageViewResourceID);
            mDepthAttachments.push_back(imageViewIndex);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        SRenderGraphResourceFlags::SRenderGraphResourceFlags()
            : requiredFormat(RenderGraphFormat_t::Undefined)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool operator<(
                SRenderGraphResource const &aLHS,
                SRenderGraphResource const &aRHS)
        {
            return (aLHS.resourceId < aRHS.resourceId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool operator!=(
                SRenderGraphResource const &aLHS,
                SRenderGraphResource const &aRHS)
        {
            return (aLHS.resourceId != aRHS.resourceId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CRenderGraphResources::CRenderGraphResources()
        {
            mResources.push_back(std::make_shared<SRenderGraphResource>());
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void appendToVector(std::vector<T>       &aOutTarget,
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
        bool CRenderGraphMutableResources::mergeIn(CRenderGraphResources const &aOther)
        {
#if defined SHIRABE_DEBUG || defined SHIRABE_TEST
            try {
#endif
                mResources = aOther.resources();

                for(RefIndex_t::value_type const&id : aOther.images())
                {
                    CRenderGraphResourcesRef<SRenderGraphImage>::insert(id);
                }

                for(RefIndex_t::value_type const&id : aOther.imageViews())
                {
                    CRenderGraphResourcesRef<SRenderGraphImageView>::insert(id);
                }

                for(RefIndex_t::value_type const&id : aOther.buffers())
                {
                    CRenderGraphResourcesRef<SRenderGraphBuffer>::insert(id);
                }
                for(RefIndex_t::value_type const&id : aOther.bufferViews())
                {
                    CRenderGraphResourcesRef<SRenderGraphBufferView>::insert(id);
                }

                for(RefIndex_t::value_type const &id : aOther.meshes())
                {
                    CRenderGraphResourcesRef<SRenderGraphMesh>::insert(id);
                }

                for(RefIndex_t::value_type const &id : aOther.materials())
                {
                    CRenderGraphResourcesRef<SRenderGraphMaterial>::insert(id);
                }

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
    std::string convert_to_string<framegraph::SRenderGraphImage>(framegraph::SRenderGraphImage const &aTexture)
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
                    "\n    Reference-Count: {}",
                    "Texture",
                    aTexture.resourceId,
                    aTexture.readableName,
                    aTexture.description.dynamicImage.width, aTexture.description.dynamicImage.height, aTexture.description.dynamicImage.depth,
                    convert_to_string(aTexture.description.dynamicImage.format),
                    aTexture.description.dynamicImage.arraySize,
                    aTexture.description.dynamicImage.mipLevels,
                    convert_to_string(aTexture.description.dynamicImage.initialState),
                    aTexture.referenceCount);
        return s;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::SRenderGraphImageView>(framegraph::SRenderGraphImageView const &aTextureView)
    {
        bool viewIsReadMode  = aTextureView.description.mode.check(ERenderGraphViewAccessMode::Read);

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
                    convert_to_string(aTextureView.description.format),
                    convert_to_string(aTextureView.description.arraySliceRange),
                    convert_to_string(aTextureView.description.mipSliceRange),
                    aTextureView.referenceCount);
        return s;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<framegraph::SRenderGraphBuffer>(framegraph::SRenderGraphBuffer const &aBuffer)
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
    std::string convert_to_string<framegraph::SRenderGraphBufferView>(framegraph::SRenderGraphBufferView const &aBufferView)
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
