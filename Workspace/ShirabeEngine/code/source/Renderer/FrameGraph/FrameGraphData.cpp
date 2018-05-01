#include "Renderer/FrameGraph/FrameGraphData.h"

#include <string.h>

namespace Engine {
  namespace FrameGraph {

    std::ostream& operator<<(std::ostream &strm, FrameGraphResourceType const&e) {
      switch(e) {
      case FrameGraphResourceType::Undefined:   strm << "FrameGraphResourceType::Undefined";   break;
      case FrameGraphResourceType::Texture:     strm << "FrameGraphResourceType::Texture";     break;
      case FrameGraphResourceType::Buffer:      strm << "FrameGraphResourceType::Buffer";      break;
      case FrameGraphResourceType::TextureView: strm << "FrameGraphResourceType::TextureView"; break;
      case FrameGraphResourceType::BufferView:  strm << "FrameGraphResourceType::BufferView";  break;
      }
      return strm;
    }

    std::ostream& operator<<(std::ostream &strm, FrameGraphFormat const&e) {
      switch(e) {
      case FrameGraphFormat::Undefined:                strm << "FrameGraphFormat::Undefined";                break;
      case FrameGraphFormat::Automatic:                strm << "FrameGraphFormat::Automatic";                break;
      case FrameGraphFormat::Structured:               strm << "FrameGraphFormat::Structured";               break;
      case FrameGraphFormat::R8_TYPELESS:              strm << "FrameGraphFormat::R8_TYPELESS";              break;
      case FrameGraphFormat::R8_SINT:                  strm << "FrameGraphFormat::R8_SINT";                  break;
      case FrameGraphFormat::R8_UINT:                  strm << "FrameGraphFormat::R8_UINT";                  break;
      case FrameGraphFormat::R8_SNORM:                 strm << "FrameGraphFormat::R8_SNORM";                 break;
      case FrameGraphFormat::R8_UNORM:                 strm << "FrameGraphFormat::R8_UNORM";                 break;
      case FrameGraphFormat::R8G8B8A8_TYPELESS:        strm << "FrameGraphFormat::R8G8B8A8_TYPELESS";        break;
      case FrameGraphFormat::R8G8B8A8_SINT:            strm << "FrameGraphFormat::R8G8B8A8_SINT";            break;
      case FrameGraphFormat::R8G8B8A8_UINT:            strm << "FrameGraphFormat::R8G8B8A8_UINT";            break;
      case FrameGraphFormat::R8G8B8A8_SNORM:           strm << "FrameGraphFormat::R8G8B8A8_SNORM";           break;
      case FrameGraphFormat::R8G8B8A8_UNORM:           strm << "FrameGraphFormat::R8G8B8A8_UNORM";           break;
      case FrameGraphFormat::R8G8B8A8_UNORM_SRGB:      strm << "FrameGraphFormat::R8G8B8A8_UNORM_SRGB";      break;
      case FrameGraphFormat::R8G8B8A8_FLOAT:           strm << "FrameGraphFormat::R8G8B8A8_FLOAT";           break;
      case FrameGraphFormat::R16_TYPELESS:             strm << "FrameGraphFormat::R16_TYPELESS";             break;
      case FrameGraphFormat::R16_SINT:                 strm << "FrameGraphFormat::R16_SINT";                 break;
      case FrameGraphFormat::R16_UINT:                 strm << "FrameGraphFormat::R16_UINT";                 break;
      case FrameGraphFormat::R16_SNORM:                strm << "FrameGraphFormat::R16_SNORM";                break;
      case FrameGraphFormat::R16_UNORM:                strm << "FrameGraphFormat::R16_UNORM";                break;
      case FrameGraphFormat::R16_FLOAT:                strm << "FrameGraphFormat::R16_FLOAT";                break;
      case FrameGraphFormat::R16G16B16A16_TYPELESS:    strm << "FrameGraphFormat::R16G16B16A16_TYPELESS";    break;
      case FrameGraphFormat::R16G16B16A16_SINT:        strm << "FrameGraphFormat::R16G16B16A16_SINT";        break;
      case FrameGraphFormat::R16G16B16A16_UINT:        strm << "FrameGraphFormat::R16G16B16A16_UINT";        break;
      case FrameGraphFormat::R16G16B16A16_SNORM:       strm << "FrameGraphFormat::R16G16B16A16_SNORM";       break;
      case FrameGraphFormat::R16G16B16A16_UNORM:       strm << "FrameGraphFormat::R16G16B16A16_UNORM";       break;
      case FrameGraphFormat::R16G16B16A16_FLOAT:       strm << "FrameGraphFormat::R16G16B16A16_FLOAT";       break;
      case FrameGraphFormat::R24_UNORM_X8_TYPELESS:    strm << "FrameGraphFormat::R24_UNORM_X8_TYPELESS";    break;
      case FrameGraphFormat::R32_TYPELESS:             strm << "FrameGraphFormat::R32_TYPELESS";             break;
      case FrameGraphFormat::R32_SINT:                 strm << "FrameGraphFormat::R32_SINT";                 break;
      case FrameGraphFormat::R32_UINT:                 strm << "FrameGraphFormat::R32_UINT";                 break;
      case FrameGraphFormat::R32_SNORM:                strm << "FrameGraphFormat::R32_SNORM";                break;
      case FrameGraphFormat::R32_UNORM:                strm << "FrameGraphFormat::R32_UNORM";                break;
      case FrameGraphFormat::R32_FLOAT:                strm << "FrameGraphFormat::R32_FLOAT";                break;
      case FrameGraphFormat::R32_FLOAT_S8X24_TYPELESS: strm << "FrameGraphFormat::R32_FLOAT_S8X24_TYPELESS"; break;
      case FrameGraphFormat::R32G32B32A32_TYPELESS:    strm << "FrameGraphFormat::R32G32B32A32_TYPELESS";    break;
      case FrameGraphFormat::R32G32B32A32_SINT:        strm << "FrameGraphFormat::R32G32B32A32_SINT";        break;
      case FrameGraphFormat::R32G32B32A32_UINT:        strm << "FrameGraphFormat::R32G32B32A32_UINT";        break;
      case FrameGraphFormat::D24_UNORM_S8_UINT:        strm << "FrameGraphFormat::D24_UNORM_S8_UINT";        break;
      case FrameGraphFormat::D32_FLOAT:                strm << "FrameGraphFormat::D32_FLOAT";                break;
      case FrameGraphFormat::D32_FLOAT_S8X24_UINT:     strm << "FrameGraphFormat::D32_FLOAT_S8X24_UINT";     break;
      }
      return strm;
    }

    bool validateFormatCompatibility(
      FrameGraphFormat const&base,
      FrameGraphFormat const&derived)
    {
      return true;
    }

    FrameGraphBuffer::FrameGraphBuffer()
      : elementSize(0)
      , elementCount(0)
    {}

    FrameGraphBufferView::FrameGraphBufferView()
      : subrange(Range(0, 0))
      , format(FrameGraphFormat::Undefined)
      , mode(FrameGraphViewAccessMode::Undefined)
    {}

    uint32_t
      width,  // 0 - Undefined
      height, // At least 1
      depth;  // At least 1
    FrameGraphFormat
      format;
    uint16_t
      mipLevels; // At least 1 (most detailed MIP)
    uint16_t
      arraySize; // At least 1 (basically everything is a vector...)
    FrameGraphResourceInitState
      initialState;
    BitField<FrameGraphResourceUsage>
      permittedUsage,
      requestedUsage;
    FrameGraphTexture::FrameGraphTexture()
      : width(0)
      , height(1)
      , depth(1)
      , format(FrameGraphFormat::Undefined)
      , mipLevels(1)
      , arraySize(1)
      , initialState(FrameGraphResourceInitState::Undefined)
      , permittedUsage(FrameGraphResourceUsage::Undefined)
      , requestedUsage(FrameGraphResourceUsage::Undefined)
    {}

    bool
      FrameGraphTexture::validate() const
    {
      bool dimensionsValid = (width == 0 || !(width == 0 || height == 0 || depth == 0));
      bool mipLevelsValid  = (mipLevels >= 1);
      bool arraySizeValid  = (arraySize >= 1);

      return (dimensionsValid && mipLevelsValid && arraySizeValid);
    }

    FrameGraphTextureView::FrameGraphTextureView()
      : arraySliceRange(Range(0, 0))
      , mipSliceRange(Range(0, 0))
      , format(FrameGraphFormat::Undefined)
      , mode(FrameGraphViewAccessMode::Undefined)
    {}

    FrameGraphResourceFlags::FrameGraphResourceFlags()
      : requiredFormat(FrameGraphFormat::Undefined)
    {}

    FrameGraphResource::FrameGraphResource()
      : resourceId(FrameGraphResourceId_t{})
    {}

    bool
      operator<(FrameGraphResource const&l, FrameGraphResource const&r)
    {
      return (l.resourceId < r.resourceId);
    }

    bool
      operator!=(FrameGraphResource const&l, FrameGraphResource const&r)
    {
      return (l.resourceId == r.resourceId);
    }

  }
}