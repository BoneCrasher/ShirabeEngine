#ifndef __SHIRABE_FRAMEGRAPH_PASS_H__
#define __SHIRABE_FRAMEGRAPH_PASS_H__

#include <assert.h>
#include <string>
#include <functional>
#include <stdint.h>

#include "Core/EngineTypeHelper.h"
#include "Core/PassKey.h"
#include "OS/ApplicationEnvironment.h"
#include "Resources/Core/ResourceDTO.h"

#include "Renderer/IRenderer.h"
#include "Renderer/FrameGraph/FrameGraphData.h"
#include "Renderer/FrameGraph/FrameGraphRenderContext.h"
#include "Renderer/FrameGraph/FrameGraphSerialization.h"

#include "PassBuilder.h"

namespace Engine {
  namespace FrameGraph {
    using namespace Rendering;
    using namespace Serialization;
    
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

      UniquePtr<Accessor>
        getAccessor(PassKey<GraphBuilder>&&) const;

      UniquePtr<MutableAccessor>
        getMutableAccessor(PassKey<GraphBuilder>&&);

      UniquePtr<Accessor>
        getAccessor(PassKey<PassBuilder>&&) const;

      UniquePtr<MutableAccessor>
        getMutableAccessor(PassKey<PassBuilder>&&);

      UniquePtr<Accessor>
        getAccessor(PassKey<Graph>&&) const;

      PassBase(
        PassUID_t   const&passUID,
        std::string const&passName);
      
      virtual bool setup(PassBuilder&);
      virtual bool execute(
        FrameGraphResources           const&frameGraphResources,
        Ptr<IFrameGraphRenderContext>      &context);

      std::string const&passName() const;
      PassUID_t   const&passUID()  const;

      virtual
        void acceptSerializer(Ptr<IFrameGraphSerializer> s);

      virtual
        void acceptDeserializer(Ptr<IFrameGraphDeserializer> const&d);

    private:
      bool registerResource(FrameGraphResourceId_t const&id);

      PassUID_t   m_passUID;
      std::string m_passName;

      FrameGraphResourceIdList m_resourceReferences;
    };

    DeclareSharedPointerType(PassBase);
    DeclareListType(Ptr<PassBase>, PassBase);
    DeclareMapType(PassUID_t, Ptr<PassBase>, Pass);

    template <typename TPassData>
    class CallbackPass
      : public PassBase
    {
    public:
      using SetupCallback_t = std::function<bool(PassBuilder&, TPassData&)>;
      using ExecCallback_t  = std::function<bool(TPassData const&, FrameGraphResources const&, Ptr<IFrameGraphRenderContext>&)>;

      CallbackPass(
        PassUID_t       const&passId,
        std::string     const&passName,
        SetupCallback_t     &&setupCb,
        ExecCallback_t      &&execCb);

      bool setup(PassBuilder&builder);
      bool execute(FrameGraphResources const&, Ptr<IFrameGraphRenderContext>&);

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
      CallbackPass<TPassData>::execute(FrameGraphResources const&resources, Ptr<IFrameGraphRenderContext>&context)
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