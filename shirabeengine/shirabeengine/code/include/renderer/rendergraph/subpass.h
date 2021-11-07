#ifndef __SHIRABE_FRAMEGRAPH_PASS_H__
#define __SHIRABE_FRAMEGRAPH_PASS_H__

#include <assert.h>
#include <string>
#include <functional>
#include <cstdint>

#include <core/enginetypehelper.h>
#include <core/passkey.h>
#include <os/applicationenvironment.h>

#include "renderer/rendergraph/rendergraphdata.h"
#include "renderer/rendergraph/framegraphcontexts.h"
#include "renderer/rendergraph/rendergraphserialization.h"
#include "renderer/rendergraph/passbuilder.h"
#include "renderer/rendergraph/scenedatasource.h"
#include "renderer/rendergraph/platformcontext.h"

namespace engine
{
    namespace framegraph
    {
        using namespace rendering;

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
        using namespace serialization;
#endif

        class CGraphBuilder;
        class CGraph;

        /**
         * Pass base implementation providing UID, name and resource references.
         *
         * Any deriving pass has to define setup and execute, as these are pure virtual
         * in this base implementation.
         * As such, this pass implementation can not be instantiate, set-up or executed.
         */
        class CPassBase
        #if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
                : public ISerializable<IRenderGraphSerializer, IRenderGraphDeserializer>
        #endif
        {
            SHIRABE_DECLARE_LOG_TAG(CPassBase);

        public_constructors:
            /**
             * Construct a base pass from UID and name.
             *
             * @param aPassUID  The pass UID assigned to this pass.
             * @param aPassName The pass name assigned to this pass.
             */
            CPassBase(
                    PassUID_t   const &aPassUID,
                    std::string const &aPassName);

        public_methods:

            /**
             * Return the UID assigned to this pass.
             *
             * @return See brief.
             */
            PassUID_t const &getSubpassUid() const;

            /**
             * Return the name assigned to this pass.
             *
             * @return See brief.
             */
            std::string const &passName() const;


#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            /**
             * Double-Dispatch serialization integration to serialize this pass.
             *
             * @param aSerializer Serializer to accept for pass serialization.
             */
            virtual bool acceptSerializer(IRenderGraphSerializer &aSerializer) const;

            /**
             * Double-Dispatch serialization integration to deserialize this pass.
             *
             * @param aDeserializer Deserializer to accept for pass serialization.
             */
            virtual bool acceptDeserializer(IRenderGraphDeserializer &aDeserializer);
#endif

            CRenderGraphResourceReferences_t const &resourceReferences() const { return mResourceReferences; }
            CRenderGraphResourceReferences_t       &mutableResourceReferences() { return mResourceReferences; }

            /**
             * Register a resource in this pass instance.
             *
             * @param aResourceUID The resource uid of the resource to register.
             * @return             True, if successful. False otherwise.
             */
            template <typename T>
            CEngineResult<> registerResource(RenderGraphResourceId_t const &aResourceUID);

            /**
             * Interface method for all passes' setup.
             * To be implemented by specific pass classes.
             *
             * @param aPassBuilder The pass builder instance to use for setup.
             * @return             True, if successful. False otherwise.
             */
            virtual CEngineResult<> setup(
                CPassBuilder                        &aPassBuilder
                , SRenderGraphPlatformContext const &aPlatformContext
                , SRenderGraphDataSource const      &aDataSource) = 0;

            /**
             * Interface method for all passes' execution.
             * To be implemented by specific pass classes.
             *
             * @param aRenderGraphResources A collection of resolved and loaded resources requested during
             *                             setup for use during exeuction.
             * @param aContext             The render context of the rendergraph interfacing with all subsystems.
             * @return                     True, if successful. False otherwise.
             */
            virtual CEngineResult<> execute(
                SRenderGraphPlatformContext const &aPlatformContext,
                SRenderGraphDataSource const      &aDataSource,
                CRenderGraphResources const       &aRenderGraphResources,
                SRenderGraphRenderContextState    &aContextState,
                SRenderGraphResourceContext       &aResourceContext,
                SRenderGraphRenderContext         &aRenderContext) = 0;

        private_members:
            PassUID_t                        mPassUID;
            std::string                      mPassName;
            CRenderGraphResourceReferences_t mResourceReferences;
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(Shared<CPassBase>, CPassBase)
        SHIRABE_DECLARE_MAP_OF_TYPES(PassUID_t, Shared<CPassBase>, Pass)

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CEngineResult<> CPassBase::registerResource(RenderGraphResourceId_t const &aResourceUID)
        {
            static_cast<CRenderGraphResourcesRef<typename std::enable_if_t<std::is_base_of_v<SRenderGraphResource, T>, T>> *>(&mResourceReferences)->insert(aResourceUID);
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        /**
         * The CallbackPass is a pass implementation, which accepts callbacks for setup and execute,
         * permitting a very elegant way to implement passes without thousands of separate classes.
         * Each CallbackPass operates on a 'TPassData' instance, i.e. the input, state and output data
         * of the pass, being externally managed and provided to the callback pass on execution.
         *
         * @tparam TPassData The data struct of the pass to externally manage and provide on execution.
         */
        template <typename TPassData>
        class CallbackPass
                : public CPassBase
        {
            SHIRABE_DECLARE_LOG_TAG(CallbackPass);

        public_typedefs:
            using SetupCallback_t = std::function<CEngineResult<>(CPassBuilder&
                                                                , TPassData&
                                                                , SRenderGraphPlatformContext const &
                                                                , SRenderGraphDataSource const &)>;
            using ExecCallback_t  = std::function<CEngineResult<>(TPassData const&
                                                                  , SRenderGraphPlatformContext const &
                                                                  , SRenderGraphDataSource const &
                                                                  , CRenderGraphResources const &
                                                                  , SRenderGraphRenderContextState &
                                                                  , SRenderGraphResourceContext &
                                                                  , SRenderGraphRenderContext &)>;

        public_constructors:
            /**
             * Construct a callback pass from a UID, name, setup callback and execute callback.
             *
             * @param aPassUID  The UID assigned to this pass.
             * @param aPassName The name assigned to this pass.
             * @param aSetupCb  The setup callback for this pass, immediately invoked on construction.
             * @param aExecCb   The execution callback invoked deferred.
             */
            CallbackPass(
                    PassUID_t       const   &aPassUID,
                    std::string     const   &aPassName,
                    SetupCallback_t        &&aSetupCb,
                    ExecCallback_t         &&aExecCb);

            /**
             * Setup implementation, invoking the setup callback.
             *
             * @param aBuilder The pass builder to use for setup.
             * @return         True, if successful. False otherwise.
             */
            CEngineResult<> setup(CPassBuilder                        &aPassBuilder
                                  , SRenderGraphPlatformContext const &aPlatformContext
                                  , SRenderGraphDataSource const      &aDataSource);

            /**
             * Execute implementation, invoking the execute callback.
             *
             * @param aRenderGraphResources A collection of resolved and loaded resources requested during
             *                             setup for use during exeuction.
             * @param aContext             The render context of the rendergraph interfacing with all subsystems.
             * @return                     True, if successful. False otherwise.
             */
            CEngineResult<> execute(
                    SRenderGraphPlatformContext const &aPlatformContext
                    , SRenderGraphDataSource const    &aDataSource
                    , CRenderGraphResources const     &aRenderGraphResources
                    , SRenderGraphRenderContextState  &aContextState
                    , SRenderGraphResourceContext     &aResourceContext
                    , SRenderGraphRenderContext       &aRenderContext) final;

            /**
             * Return the pass data struct associated with this callback pass.
             *
             * @return See brief.
             */
            TPassData const &passData() const
            {
                return mPassData;
            }

        private_members:
            SetupCallback_t                  mSetupCallback;
            ExecCallback_t                   mExecCallback;
            TPassData                        mPassData;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TPassData>
        CallbackPass<TPassData>::CallbackPass(
                PassUID_t       const  &passUID,
                std::string     const  &passName,
                SetupCallback_t       &&setupCb,
                ExecCallback_t        &&execCb)
            : CPassBase(passUID, passName)
            , mSetupCallback(setupCb)
            , mExecCallback(execCb)
            , mPassData()
        {
            assert(nullptr != mSetupCallback);
            assert(nullptr != mExecCallback );
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TPassData>
        CEngineResult<> CallbackPass<TPassData>::setup(CPassBuilder                        &aPassBuilder
                                                       , SRenderGraphPlatformContext const &aPlatformContext
                                                       , SRenderGraphDataSource const      &aDataSource)
        {
            TPassData passData{ };

            CEngineResult<> setup = mSetupCallback(aPassBuilder, passData, aPlatformContext, aDataSource);
            if(setup.successful())
            {
                mPassData = passData;
            }

            return setup;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TPassData>
        CEngineResult<> CallbackPass<TPassData>::execute(SRenderGraphPlatformContext const &aPlatformContext
                                                         , SRenderGraphDataSource const    &aDataSource
                                                         , CRenderGraphResources const     &aRenderGraphResources
                                                         , SRenderGraphRenderContextState  &aContextState
                                                         , SRenderGraphResourceContext     &aResourceContext
                                                         , SRenderGraphRenderContext       &aRenderContext)
        {
            try
            {
                CEngineResult<> execution = mExecCallback(mPassData, aPlatformContext, aDataSource, aRenderGraphResources, aContextState, aResourceContext, aRenderContext);
                return execution;
            }
            catch(std::runtime_error const &e)
            {
                CLog::Error(logTag(), e.what());
                return { EEngineStatus::Error };
            }
            catch(...)
            {
                CLog::Error(logTag(), "Unknown error while executing pass.");
                return { EEngineStatus::Error };
            }
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
