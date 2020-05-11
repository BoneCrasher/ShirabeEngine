#include <assert.h>
#include "renderer/framegraph/graphbuilder.h"

namespace engine
{
    namespace framegraph
    {
        /**
         * Function signature used for a check for a uid within a list of UIDs.
         */
        template <typename TUID>
        using AlreadyRegisteredFn_t = std::function<bool(std::vector<TUID> const&, TUID const&)>;

        /**
         * Functor usable as a predicate to determine, whether adjacency info was already stored for a
         * node UID.
         */
        template <typename TUID>
        static AlreadyRegisteredFn_t<TUID> alreadyRegisteredFn =
                [] (std::vector<TUID> const &aAdjacency,
                    TUID              const &aPossiblyAdjacent)
        {
            return (aAdjacency.end() != std::find(aAdjacency.begin(), aAdjacency.end(), aPossiblyAdjacent));
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CGraphBuilder::CGraphBuilder()
            : mGraphMode              (framegraph::CGraph::EGraphMode::Compute                                        )
            , mRenderToBackBuffer     (false                                                                          )
            , mOutputResourceId       (0                                                                              )
            , mApplicationEnvironment (nullptr                                                                        )
            , mDisplay                (nullptr                                                                        )
            , mRenderPassUIDGenerator (std::make_shared<CSequenceUIDGenerator<RenderPassUID_t>>(0))
            , mSubpassUIDGenerator    (std::make_shared<CSequenceUIDGenerator<PassUID_t>>(0))
            , mResourceUIDGenerator   (std::make_shared<CSequenceUIDGenerator<RenderGraphResourceId_t >>(1))
            , mRenderPasses           (                                                                               )
            , mResources              (                                                                               )
            , mResourceData           (                                                                               )
            , mRenderPassTree         (                                                                               )
            , mRenderGraph             (nullptr                                                                        )
    #if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            , mResourceAdjacency      (                                          )
            , mPassToResourceAdjacency(                                          )
    #endif
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        RenderPassUID_t CGraphBuilder::generateRenderPassUID()
        {
            return mRenderPassUIDGenerator->generate();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        PassUID_t CGraphBuilder::generateSubpassUID()
        {
            return mSubpassUIDGenerator->generate();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        RenderGraphResourceId_t CGraphBuilder::generateResourceUID()
        {
            return mResourceUIDGenerator->generate();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        Unique<CGraph> &CGraphBuilder::graph()
        {
            return mRenderGraph;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::initialize(
                Shared<os::SApplicationEnvironment> const &aApplicationEnvironment,
                Shared<wsi::CWSIDisplay>            const &aDisplay)
        {
            bool const inputInvalid =
                    nullptr == aApplicationEnvironment or
                    nullptr == aDisplay;

            if(inputInvalid)
            {
                return { EEngineStatus::Error };
            }

            graph() = makeUnique<CGraph>();

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::deinitialize()
        {
            graph() = nullptr;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<Shared<CRenderPass>> CGraphBuilder::beginRenderPass(std::string const &aName)
        {
            if(nullptr != mRenderPassUnderConstruction)
            {
                CLog::Error(logTag(), "Cannot begin a render pass, if another one is currently under construction.");
                return EEngineStatus::Error;
            }

            RenderPassUID_t const uid = generateRenderPassUID();

            Shared<CRenderPass> renderPass = makeShared<CRenderPass>(uid, aName, mResources, mResourceData);

            mRenderPasses.insert({uid, renderPass});
            mRenderPassTree.add(uid);

            mRenderPassUnderConstruction = renderPass;

            return { EEngineStatus::Ok, mRenderPassUnderConstruction };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::endRenderPass()
        {
            mRenderPassUnderConstruction = nullptr;

            return { EEngineStatus::Ok };
        }

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::createRenderPassDependency(std::string const &aPassSource,
                                                                  std::string const &aPassTarget)
        {
            auto const sourcePredicate = [&] (PassMap::value_type const &aPair) -> bool
            {
                std::string const &name = aPair.second->passName();
                return (0 == aPassSource.compare(name));
            };

            auto const targetPredicate = [&] (PassMap::value_type const &aPair) -> bool
            {
                std::string const &name = aPair.second->passName();
                return (0 == aPassTarget.compare(name));
            };

            RenderPassMap::const_iterator sourcePass = std::find_if(mRenderPasses.begin(), mRenderPasses.end(), sourcePredicate);
            RenderPassMap::const_iterator targetPass = std::find_if(mRenderPasses.begin(), mRenderPasses.end(), targetPredicate);

            bool const containsSourcePass = (mRenderPasses.end() != sourcePass);
            bool const containsTargetPass = (mRenderPasses.end() != targetPass);

            if(not (containsSourcePass && containsTargetPass))
            {
                CLog::Error(logTag(), CString::format("Cannot create pass dependency from {} to {}. At least one pass was not found.", aPassSource, aPassTarget));
                return { EEngineStatus::Error };
            }

            PassUID_t const &sourcePassUID = sourcePass->second->getRenderPassUid();
            PassUID_t const &targetPassUID = targetPass->second->getRenderPassUid();

            createRenderPassDependencyByUID(sourcePassUID, targetPassUID);

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::createRenderPassDependencyByUID(RenderPassUID_t const &aPassSource,
                                                                       RenderPassUID_t const &aPassTarget)
        {
            mRenderPassTree.add(aPassSource);
            mRenderPassTree.add(aPassTarget);
            mRenderPassTree.connect(aPassSource, aPassTarget);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<Unique<CGraph>> CGraphBuilder::compile()
        {
            // First (No-Op, automatically performed on call to 'setPassDependency(...)':
            //   Evaluate explicit pass dependencies without resource flow.

            // Evaluate all pass resources and their implicit relations to passes and other resources.
            for(RenderPassMap::value_type const &renderPassAssignment : graph()->renderPasses())
            {
                RenderPassUID_t     renderpassUid = renderPassAssignment.first;
                Shared<CRenderPass> renderpass    = renderPassAssignment.second;

                CEngineResult<> const collection = collectRenderPass(renderpass);
                if(not collection.successful())
                {
                    CLog::Error(logTag(), "Failed to perform renderpass collection.");
                    return { EEngineStatus::Error };
                }
            }

            // Fourth: Do the same sorting for the renderpasses itself.
            std::vector<RenderPassUID_t> const topologicalRenderpassSort = mRenderPassTree.topologicalSort();

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION

            // Fourth: Sort the resources by their relationships and dependencies.
            bool const topologicalResourceSortSuccessful = topologicalSort<RenderGraphResourceId_t>(accessor->mutableResourceOrder());
            if(!topologicalResourceSortSuccessful)
            {
                CLog::Error(logTag(), "Failed to perform topologicalSort(...) for resources on graph compilation.");
                return nullptr;
            }
#endif

#if defined SHIRABE_DEBUG || defined SHIRABE_TEST

            CEngineResult<> const validation = validate();
            if(not validation.successful())
            {
                CLog::Error(logTag(), "Failed to perform validation(...) on graph compilation.");
                return { EEngineStatus::Error };
            }

#endif

            // Move out the current adjacency state to the frame graph, so that it can be used for further processing.
            // It is no more needed at this point within the GraphBuilder.
            graph()->mSubpasses                = std::move(this->mSubpasses);
            graph()->mRenderPasses             = std::move(this->mRenderPasses);
            graph()->mRenderpassExecutionOrder = std::move(topologicalRenderpassSort);

            graph()->mResources          = std::move(this->mResources);
            graph()->mResourceData.mergeIn(this->mResourceData);

            graph()->mGraphMode               = mGraphMode;
            graph()->mRenderToBackBuffer      = mRenderToBackBuffer;
            graph()->mOutputTextureResourceId = mOutputResourceId;

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            accessor->mutableResourceAdjacency()       = std::move(this->mResourceAdjacency);
            accessor->mutablePassToResourceAdjacency() = std::move(this->mPassToResourceAdjacency);
#endif


            return { EEngineStatus::Ok, std::move(graph()) };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<RenderGraphResourceId_t> CGraphBuilder::findSubjacentResource(
               SRenderGraphResourceMap const &aResources,
               SRenderGraphResource    const &aStart)
        {
            if(SHIRABE_FRAMEGRAPH_UNDEFINED_RESOURCE == aStart.parentResource)
            {
                return { EEngineStatus::Ok, aStart.resourceId };
            }
            else
            {
                if(aResources.end() != aResources.find(aStart.parentResource))
                {
                    return findSubjacentResource(aResources, aResources.at(aStart.parentResource));
                }
                else
                {
                    return { EEngineStatus::Error };
                }
            }
        }        
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::collectRenderPass(Shared<CRenderPass> aRenderPass)
        {
            if(nullptr == aRenderPass)
            {
                return { EEngineStatus::Error };
            }

            aRenderPass->collectSubpasses(*this);

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraphBuilder::validate()
        {
            bool allBindingsValid = true;

            for(RefIndex_t::value_type const &textureViewRef : mResourceData.textureViews())
            {
                CEngineResult<Shared<SRenderGraphImageView>> textureViewFetch = mResourceData.getResource<SRenderGraphImageView>(textureViewRef);
                if(not textureViewFetch.successful())
                {
                    CLog::Error(logTag(), "Failed to getResource texture view to validate.");
                    return { textureViewFetch.result() };
                }

                SRenderGraphImageView const &textureView = *(textureViewFetch.data());

                // Adjust resource access flags in the subjacent resource to have the texture creation configure
                // everything appropriately.

                RenderGraphResourceId_t const  subjacentResourceId =  textureView.subjacentResource;

                CEngineResult<Shared<SRenderGraphImage>> subjacentFetch = mResourceData.getResource<SRenderGraphImage>(subjacentResourceId);
                if(not textureViewFetch.successful())
                {
                    CLog::Error(logTag(), "Failed to getResource subjacent texture to validate.");
                    return { textureViewFetch.result() };
                }

                SRenderGraphImage const &texture = *(subjacentFetch.data());

                bool viewBindingValid = true;
                viewBindingValid  = validateTextureView(texture, textureView);
                allBindingsValid &= viewBindingValid;

            } // foreach TextureView

            return { allBindingsValid
                        ? EEngineStatus::Ok
                        : EEngineStatus::Error };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::validateTextureView(
            SRenderGraphImage     const &aTexture,
                SRenderGraphImageView const &atextureView)
        {
            bool const usageValid             = validateTextureUsage(aTexture);
            bool const formatValid            = validateTextureFormat(aTexture, atextureView);
            bool const subresourceAccessValid = validateTextureSubresourceAccess(aTexture, atextureView);

            return (usageValid && formatValid && subresourceAccessValid);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::validateTextureUsage(SRenderGraphImage const &aTexture)
        {
            // Cross both bitsets... permittedUsage should fully contain requestedUsage
            return aTexture.permittedUsage.check(aTexture.requestedUsage);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::validateTextureFormat(
            SRenderGraphImage     const &aTexture,
                SRenderGraphImageView const &aTextureView)
        {
            using FormatValue_t = std::underlying_type_t<RenderGraphFormat_t>;

            auto const nearestPowerOf2Ceil = [] (FormatValue_t value) -> uint64_t
            {
                uint64_t power = 2;
                while(value >>= 1)
                {
                    power <<= 1;
                }
                return power;
            };

            RenderGraphFormat_t const &source = aTexture.format;
            RenderGraphFormat_t const &target = aTextureView.format;

            bool formatsCompatible = false;

            switch(target)
            {
            case RenderGraphFormat_t::Undefined:
            case RenderGraphFormat_t::Structured:
                // Invalid!
                formatsCompatible = false;
                break;
            case RenderGraphFormat_t::Automatic:
                // Should never be accessed as automatic is resolved beforehand though...
                break;
            default:
                FormatValue_t sourceUID = static_cast<FormatValue_t>(source);
                FormatValue_t targetUID = static_cast<FormatValue_t>(target);
                // For now -> Simple test: Bitdepth...
                formatsCompatible = (nearestPowerOf2Ceil(sourceUID) == nearestPowerOf2Ceil(targetUID));
                break;
            }

            return formatsCompatible;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraphBuilder::validateTextureSubresourceAccess(
            SRenderGraphImage     const &aTexture,
                SRenderGraphImageView const &aTextureView)
        {
            CRange const &arraySliceRange = aTextureView.arraySliceRange;
            CRange const &mipSliceRange   = aTextureView.mipSliceRange;

            bool arraySliceRangeValid = true;
            arraySliceRangeValid &= (arraySliceRange.offset < aTexture.arraySize);
            arraySliceRangeValid &= ((arraySliceRange.offset + static_cast<uint32_t>(arraySliceRange.length)) <= aTexture.arraySize);

            bool mipSliceRangeValid = true;
            mipSliceRangeValid &= (mipSliceRange.offset < aTexture.mipLevels);
            mipSliceRangeValid &= ((mipSliceRange.offset + static_cast<uint32_t>(mipSliceRange.length)) <= aTexture.mipLevels);

            bool const valid = (arraySliceRangeValid && mipSliceRangeValid);
            return valid;
        }
        //<-----------------------------------------------------------------------------
    }
}
