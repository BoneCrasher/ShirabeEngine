#ifndef __SHIRABE_FRAMEGRAPH_PASS_H__
#define __SHIRABE_FRAMEGRAPH_PASS_H__

#include <string>
#include <functional>
#include <stdint.h>

#include "Core/EngineTypeHelper.h"
#include "Platform/ApplicationEnvironment.h"
#include "Resources/Core/ResourceDTO.h"

#include "Renderer/IRenderer.h"
#include "Renderer/FrameGraph/FrameGraphData.h"
#include "Renderer/FrameGraph/FrameGraphRenderContext.h"
#include "Renderer/FrameGraph/FrameGraphSerialization.h"

#include "PassBuilder.h"

namespace Engine {
  namespace FrameGraph {
    using namespace Renderer;
    using namespace Serialization;

    class GraphBuilder;

    class PassBase
      : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
    {
      friend class FrameGraphGraphVizSerializer;

    public:
      inline PassBase(
        PassUID_t   const&passUID,
        std::string const&passName)
        : m_passUID(passUID)
        , m_passName(passName)
      {}

      virtual bool setup(PassBuilder&) { return true; }
      virtual bool execute(FrameGraphResources const&frameGraphResources, Ptr<IFrameGraphRenderContext>&) { return true; }

      inline std::string const&passName() const { return m_passName; }
      inline PassUID_t   const&passUID()  const { return m_passUID; }

      virtual inline
        void acceptSerializer(Ptr<IFrameGraphSerializer> s)
      {
        s->serializePass(*this);
      }

      virtual inline
        void acceptDeserializer(Ptr<IFrameGraphDeserializer> const&d)
      {
        d->deserializePass(*this);
      }

    private:
      PassUID_t   m_passUID;
      std::string m_passName;
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

      TPassData m_passData;
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