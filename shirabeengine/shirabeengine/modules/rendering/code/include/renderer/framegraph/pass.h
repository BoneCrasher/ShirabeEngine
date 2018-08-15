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

        class GraphBuilder;
        class Graph;

        class PassBase
                : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
        {
        public:
            class Accessor {
            public:
                Accessor(PassBase const*);

                FrameGraphResourceIdList const&resourceReferences() const;

            private:
                PassBase const*m_pass;
            };

            class MutableAccessor
                    : public Accessor
            {
            public:
                MutableAccessor(PassBase *);

                FrameGraphResourceIdList &mutableResourceReferences();

                bool registerResource(FrameGraphResourceId_t const&id);

            private:
                PassBase *m_pass;
            };

            UniqueCStdSharedPtr_t<Accessor>
            getAccessor(PassKey<GraphBuilder>&&) const;

            UniqueCStdSharedPtr_t<MutableAccessor>
            getMutableAccessor(PassKey<GraphBuilder>&&);

            UniqueCStdSharedPtr_t<Accessor>
            getAccessor(PassKey<PassBuilder>&&) const;

            UniqueCStdSharedPtr_t<MutableAccessor>
            getMutableAccessor(PassKey<PassBuilder>&&);

            UniqueCStdSharedPtr_t<Accessor>
            getAccessor(PassKey<Graph>&&) const;

            PassBase(
                    PassUID_t   const&passUID,
                    std::string const&passName);

            virtual bool setup(PassBuilder&);
            virtual bool execute(
                    FrameGraphResources           const&frameGraphResources,
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &context);

            std::string const&passName() const;
            PassUID_t   const&passUID()  const;

            virtual
            void acceptSerializer(CStdSharedPtr_t<IFrameGraphSerializer> s);

            virtual
            void acceptDeserializer(CStdSharedPtr_t<IFrameGraphDeserializer> const&d);

        private:
            bool registerResource(FrameGraphResourceId_t const&id);

            PassUID_t   m_passUID;
            std::string m_passName;

            FrameGraphResourceIdList m_resourceReferences;
        };

        DeclareSharedPointerType(PassBase);
        DeclareListType(CStdSharedPtr_t<PassBase>, PassBase);
        DeclareMapType(PassUID_t, CStdSharedPtr_t<PassBase>, Pass);

        template <typename TPassData>
        class CallbackPass
                : public PassBase
        {
        public:
            using SetupCallback_t = std::function<bool(PassBuilder&, TPassData&)>;
            using ExecCallback_t  = std::function<bool(TPassData const&, FrameGraphResources const&, CStdSharedPtr_t<IFrameGraphRenderContext>&)>;

            CallbackPass(
                    PassUID_t       const&passId,
                    std::string     const&passName,
                    SetupCallback_t     &&setupCb,
                    ExecCallback_t      &&execCb);

            bool setup(PassBuilder&builder);
            bool execute(FrameGraphResources const&, CStdSharedPtr_t<IFrameGraphRenderContext>&);

            TPassData const&passData() const { return m_passData; }

        private:
            SetupCallback_t setupCallback;
            ExecCallback_t  execCallback;

            FrameGraphResourceIdList
            m_resources;
            TPassData
            m_passData;
        };

        template <typename TPassData>
        CallbackPass<TPassData>::CallbackPass(
                PassUID_t       const&passUID,
                std::string     const&passName,
                SetupCallback_t     &&setupCb,
                ExecCallback_t      &&execCb)
            : PassBase(passUID, passName)
            , setupCallback(setupCb)
            , execCallback(execCb)
            , m_passData()
        {
            assert(setupCallback != nullptr);
            assert(execCallback  != nullptr);
        }

        template <typename TPassData>
        bool
        CallbackPass<TPassData>::setup(PassBuilder&builder)
        {
            TPassData passData{ };
            if(setupCallback(builder, passData)) {
                m_passData = passData;
                return true;
            }
            else
                return false;
        }

        template <typename TPassData>
        bool
        CallbackPass<TPassData>::execute(FrameGraphResources const&resources, CStdSharedPtr_t<IFrameGraphRenderContext>&context)
        {
            try {
                return execCallback(m_passData, resources, context);
            }
            catch(...) {
                return false;
            }
        }

    }
}

#endif
