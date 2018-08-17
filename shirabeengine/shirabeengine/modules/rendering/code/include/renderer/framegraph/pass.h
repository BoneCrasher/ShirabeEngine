#ifndef __SHIRABE_FRAMEGRAPH_PASS_H__
#define __SHIRABE_FRAMEGRAPH_PASS_H__

#include <assert.h>
#include <string>
#include <functional>
#include <stdint.h>

#include <core/enginetypehelper.h>
#include <core/passkey.h>
#include <os/applicationenvironment.h>
#include <resources/core/resourcedomaintransfer.h>

#include "renderer/irenderer.h"
#include "renderer/framegraph/framegraphdata.h"
#include "renderer/framegraph/framegraphrendercontext.h"
#include "renderer/framegraph/framegraphserialization.h"
#include "renderer/framegraph/passbuilder.h"

namespace engine
{
    namespace framegraph
    {
        using namespace rendering;
        using namespace serialization;

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
                : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
        {
        public_classes:
            /**
             * This accessor provides guarded immutable access to the internals of a CPassBase.
             */
            class CAccessor
            {
                friend class CPassBase;

            public_methods:
                /**
                 * Return the currently attached resources of the referred pass.
                 *
                 * @return See brief.
                 */
                FrameGraphResourceIdList const &resourceReferences() const;

            private_constructors:
                /**
                 * Construct an accessor from a pass to refer to.
                 *
                 * @param aPass The pass to refer to.
                 */
                CAccessor(CPassBase const *aPass);

            private_members:
                CPassBase const *mPass;
            };

            /**
             * Extends CAccessor by mutable access to the referred to pass' internals.
             */
            class CMutableAccessor
                    : public CAccessor
            {
                friend class CPassBase;

            public_methods:
                /**
                 * Return the currently attached resources of the referred pass.
                 *
                 * @return See brief.
                 */
                FrameGraphResourceIdList &mutableResourceReferences();

                /**
                 * Register a resource in the referred to pass.
                 *
                 * @param aResourceId The resource id of the resource to store.
                 * @return            True, if successful. False, otherwise.
                 */
                bool registerResource(FrameGraphResourceId_t const &aResourceId);

            private_constructors:
                /**
                 * Construct an accessor from a pass to refer to.
                 *
                 * @param aPass The pass to refer to.
                 */
                CMutableAccessor(CPassBase *aPass);

            private_members:
                CPassBase *mPass;
            };

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
             * Return an accessor to CGraphBuilder instances.
             *
             * @param aPassKey Pass-Key instance creatable by CGraphBuilder instances only.
             * @return         See brief.
             */
            CStdUniquePtr_t<CAccessor> getAccessor(CPassKey<CGraphBuilder> &&aPassKey) const;

            /**
             * Return an accessor to CGraphBuilder instances.
             *
             * @param aPassKey Pass-Key instance creatable by CGraphBuilder instances only.
             * @return         See brief.
             */
            CStdUniquePtr_t<CMutableAccessor> getMutableAccessor(CPassKey<CGraphBuilder> &&aPassKey);

            /**
             * Return an accessor to PassBuilder instances.
             *
             * @param aPassKey Pass-Key instance creatable by PassBuilder instances only.
             * @return         See brief.
             */
            CStdUniquePtr_t<CAccessor> getAccessor(CPassKey<CPassBuilder> &&aPassKey) const;

            /**
             * Return an accessor to PassBuilder instances.
             *
             * @param aPassKey Pass-Key instance creatable by PassBuilder instances only.
             * @return         See brief.
             */
            CStdUniquePtr_t<CMutableAccessor> getMutableAccessor(CPassKey<CPassBuilder> &&aPassKey);

            /**
             * Return an accessor to CGraph instances.
             *
             * @param aPassKey Pass-Key instance creatable by CGraph instances only.
             * @return         See brief.
             */
            CStdUniquePtr_t<CAccessor> getAccessor(CPassKey<CGraph> &&aPassKey) const;

            /**
             * Return the UID assigned to this pass.
             *
             * @return See brief.
             */
            PassUID_t const &passUID() const;

            /**
             * Return the name assigned to this pass.
             *
             * @return See brief.
             */
            std::string const &passName() const;

            /**
             * Double-Dispatch serialization integration to serialize this pass.
             *
             * @param aSerializer Serializer to accept for pass serialization.
             */
            virtual bool acceptSerializer(IFrameGraphSerializer &aSerializer) const;

            /**
             * Double-Dispatch serialization integration to deserialize this pass.
             *
             * @param aDeserializer Deserializer to accept for pass serialization.
             */
            virtual bool acceptDeserializer(IFrameGraphDeserializer &aDeserializer);

            /**
             * Interface method for all passes' setup.
             * To be implemented by specific pass classes.
             *
             * @param aPassBuilder The pass builder instance to use for setup.
             * @return             True, if successful. False otherwise.
             */
            virtual bool setup(CPassBuilder &aPassBuilder) = 0;

            /**
             * Interface method for all passes' execution.
             * To be implemented by specific pass classes.
             *
             * @param aFrameGraphResources A collection of resolved and loaded resources requested during
             *                             setup for use during exeuction.
             * @param aContext             The render context of the framegraph interfacing with all subsystems.
             * @return                     True, if successful. False otherwise.
             */
            virtual bool execute(
                    CFrameGraphResources                      const&aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aContext) = 0;

        private_methods:
            /**
             * Register a resource in this pass instance.
             *
             * @param aResourceUID The resource uid of the resource to register.
             * @return             True, if successful. False otherwise.
             */
            bool registerResource(FrameGraphResourceId_t const &aResourceUID);

        private_members:
            PassUID_t                mPassUID;
            std::string              mPassName;
            FrameGraphResourceIdList mResourceReferences;
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(CStdSharedPtr_t<CPassBase>, CPassBase);
        SHIRABE_DECLARE_MAP_OF_TYPES(PassUID_t, CStdSharedPtr_t<CPassBase>, Pass);

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
        public_typedefs:
            using SetupCallback_t = std::function<bool(CPassBuilder&, TPassData&)>;
            using ExecCallback_t  = std::function<bool(TPassData const&, CFrameGraphResources const&, CStdSharedPtr_t<IFrameGraphRenderContext>&)>;

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
                    PassUID_t       const  &aPassUID,
                    std::string     const  &aPassName,
                    SetupCallback_t       &&aSetupCb,
                    ExecCallback_t        &&aExecCb);

            /**
             * Setup implementation, invoking the setup callback.
             *
             * @param aBuilder The pass builder to use for setup.
             * @return         True, if successful. False otherwise.
             */
            bool setup(CPassBuilder &aBuilder);

            /**
             * Execute implementation, invoking the execute callback.
             *
             * @param aFrameGraphResources A collection of resolved and loaded resources requested during
             *                             setup for use during exeuction.
             * @param aContext             The render context of the framegraph interfacing with all subsystems.
             * @return                     True, if successful. False otherwise.
             */
            bool execute(
                    CFrameGraphResources                      const &aFrameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &aContext);

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
            SetupCallback_t          mSetupCallback;
            ExecCallback_t           mExecCallback;
            FrameGraphResourceIdList mResources;
            TPassData                mPassData;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TPassData>
        CallbackPass<TPassData>::CallbackPass(
                PassUID_t       const&passUID,
                std::string     const&passName,
                SetupCallback_t     &&setupCb,
                ExecCallback_t      &&execCb)
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
        bool CallbackPass<TPassData>::setup(CPassBuilder &aBuilder)
        {
            TPassData passData{ };
            if(mSetupCallback(aBuilder, passData))
            {
                mPassData = passData;
                return true;
            }
            else
                return false;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TPassData>
        bool CallbackPass<TPassData>::execute(
                CFrameGraphResources                      const &aFrameGraphResources,
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aContext)
        {
            try
            {
                return mExecCallback(mPassData, aFrameGraphResources, aContext);
            }
            catch(...) {
                return false;
            }
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif
