#include "Renderer/FrameGraph/FrameGraphData.h"

#include <string.h>

namespace Engine {
  namespace FrameGraph {

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

    FrameGraphTexture::FrameGraphTexture()
      : width(0)
      , height(1)
      , depth(1)
      , format(FrameGraphFormat::Undefined)
      , mipLevels(1)
      , arraySize(1)
      , initialState(FrameGraphResourceInitState::Undefined)
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

    FrameGraphResourcePrivateData::FrameGraphResourcePrivateData()
      : parentResourceId(0)
      , type(FrameGraphResourceType::Undefined)
      , flags()
      , usage(FrameGraphResourceUsage::Undefined)
      , resourceViews()
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