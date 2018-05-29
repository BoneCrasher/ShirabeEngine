#ifndef __SHIRABE_RENDERER_PASSBUILDER_H__
#define __SHIRABE_RENDERER_PASSBUILDER_H__

#include <string>
#include <functional>

#include "Core/BasicTypes.h"
#include "Core/EngineTypeHelper.h"
#include "Core/Random.h"
#include "Core/UID.h"
#include "Log/Log.h"
#include "Resources/Core/ResourceDTO.h"

#include "FrameGraphData.h"

namespace Engine {
  namespace FrameGraph {

    class PassBase;

    using Engine::Core::IUIDGenerator;
    using namespace Engine::Resources;
    
    enum PassResourceConstraintFlags {
      None          = 0,
      TextureWidth  = 1,
      TextureHeight = 2,
      TextureSize   = 3,
    };

    struct PassResourceConstraint {
      FrameGraphResource
        target,
        source;
      PassResourceConstraintFlags
        flags;

      PassResourceConstraint(
        FrameGraphResource const&inTarget,
        FrameGraphResource const&inSource,
        PassResourceConstraintFlags const&inFlags)
        : target(inTarget)
        , source(inSource)
        , flags(inFlags)
      {}
    };

    /**********************************************************************************************//**
     * \class PassBuilder
     *
     * \brief A pass builder.
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     **************************************************************************************************/
    class PassBuilder {
      DeclareLogTag(PassBuilder);
     
    public:      
      PassBuilder(
        PassUID_t                  const&passUID,
        Ptr<PassBase>                    pass,
        FrameGraphMutableResources      &resourceData);

      PassUID_t const&assignedPassUID() const { return m_passUID; }

      FrameGraphResource
        createTexture(
          std::string       const&name,
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
        importRenderables(
          std::string        const&collectionName, 
          FrameGraphResource const&renderableListResource);
    
    private:

      FrameGraphResourceId_t findDuplicateTextureView(
        FrameGraphResourceId_t            const&subjacentResourceId,
        FrameGraphFormat                  const&format,
        FrameGraphViewSource              const&viewSource,
        Range                             const&arrayRange,
        Range                             const&mipRange,
        BitField<FrameGraphViewAccessMode> const&mode);

      void adjustArrayAndMipSliceRanges(
        FrameGraphResources const&resourceData,
        FrameGraphResource  const&sourceResource,
        Range const&arraySliceRange,
        Range const&mipSliceRange,
        Range      &adjustedArraySliceRange,
        Range      &adjustedMipSliceRange);

      void validateArrayAndMipSliceRanges(
        FrameGraphResources const&resourceData,
        FrameGraphResource  const&sourceResource,
        Range               const&arraySliceRange,
        Range               const&mipSliceRange,
        bool                      validateReads  = true,
        bool                      validateWrites = true);

      bool isTextureBeingReadInSubresourceRange(
        RefIndex            const&resourceViews,
        FrameGraphResources const&resources,
        FrameGraphResource  const&sourceResource,
        Range               const&arraySliceRange,
        Range               const&mipSliceRange);
      bool isTextureBeingWrittenInSubresourceRange(
        RefIndex            const&resourceViews,
        FrameGraphResources const&resources,
        FrameGraphResource  const&sourceResource,
        Range               const&arraySliceRange,
        Range               const&mipSliceRange);

      PassUID_t     m_passUID;
      Ptr<PassBase> m_pass;

      FrameGraphMutableResources &m_resourceData;
    };

  }
}

#endif