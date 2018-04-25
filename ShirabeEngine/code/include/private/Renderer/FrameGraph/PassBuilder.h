#ifndef __SHIRABE_RENDERER_PASSBUILDER_H__
#define __SHIRABE_RENDERER_PASSBUILDER_H__

#include <string>
#include <functional>

#include "Core/BasicTypes.h"
#include "Core/EngineTypeHelper.h"
#include "Core/Random.h"
#include "Resources/Core/ResourceDTO.h"

#include "FrameGraphData.h"

namespace Engine {
  namespace FrameGraph {

    using namespace Engine::Resources;

    /**********************************************************************************************//**
     * \class PassBuilder
     *
     * \brief A pass builder.
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     **************************************************************************************************/
    class PassBuilder {
      DeclareMapType(FrameGraphResourceId_t, FrameGraphResourcePrivateData, ResourcePrivateData);
      DeclareMapType(FrameGraphResourceId_t, FrameGraphTexture,     FrameGraphTexture);
      DeclareMapType(FrameGraphResourceId_t, FrameGraphTextureView, FrameGraphTextureView);
      DeclareMapType(FrameGraphResourceId_t, FrameGraphBuffer,      FrameGraphBuffer);
      DeclareMapType(FrameGraphResourceId_t, FrameGraphBufferView,  FrameGraphBufferView);

    public:
      PassBuilder(PassUID_t const&passUID, Ptr<Random::RandomState>);

      PassUID_t const&assignedPassUID() const { return m_passUID; }

      FrameGraphResource
        createTexture(
          FrameGraphTexture const&desc);

      // createBuffer
      // create~?

      FrameGraphResource
        writeTexture(
          FrameGraphResource          const&subjacentTargetResourceId,
          FrameGraphWriteTextureFlags const&flags,
          Range                       const&arraySliceRange = Range(0, 1),
          Range                       const&mipSliceRange   = Range(0, 1));

      FrameGraphResource
        readTexture(
          FrameGraphResource         const&subjacentTargetResourceId,
          FrameGraphReadTextureFlags const&flags,
          Range                      const&arraySliceRange = Range(0, 1),
          Range                      const&mipSliceRange   = Range(0, 1));

      FrameGraphResource
        importRenderables();

    private:
      bool isResourceRegistered(FrameGraphResource const&resourceId) const;
      bool isTextureBeingReadInSubresourceRange(
        std::vector<FrameGraphResourceId_t> const&resourceViews,
        Range                               const&arraySliceRange,
        Range                               const&mipSliceRange);
      bool isTextureBeingWrittenInSubresourceRange(
        std::vector<FrameGraphResourceId_t> const&resourceViews,
        Range                               const&arraySliceRange,
        Range                               const&mipSliceRange);

      Ptr<Random::RandomState> m_resourceIdGenerator;
      PassUID_t                m_passUID;

      ResourcePrivateDataMap   m_resourcesPrivateData;
      FrameGraphTextureMap     m_textures;
      FrameGraphTextureViewMap m_textureViews;
      FrameGraphBufferMap      m_buffers;
      FrameGraphBufferViewMap  m_bufferViews;
    };

  }
}

#endif