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

#include "FrameGraphSerialization.h"

#include "PassBuilder.h"

namespace Engine {
  namespace FrameGraph {
    using namespace Renderer;
    using namespace Serialization;

    class GraphBuilder;
    
    class PassBase
      : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
    {
    public:
      inline PassBase(PassUID_t const&passUID)
        : m_passUID(passUID)
      {}

      virtual bool execute(Ptr<IRenderContext>&) = 0;

      inline void acceptSerializer(Ptr<IFrameGraphSerializer>&s)
      {
        s->serializePass(GetNonDeletingSelfPtrType(this));
      }

      inline void acceptDeserializer(Ptr<IFrameGraphDeserializer> const&d)
      {
        d->deserializePass(GetNonDeletingSelfPtrType(this));
      }

      inline PassUID_t passUID() const { return m_passUID; }

    private:
      PassUID_t m_passUID;
    };

    DeclareSharedPointerType(PassBase);
    DeclareListType(Ptr<PassBase>, PassBase);
    DeclareMapType(std::string, Ptr<PassBase>, Pass);

    template <typename TPassData>
    class CallbackPass
      : public PassBase
    {
    public:
      using SetupCallback_t = std::function<bool(PassBuilder&, TPassData&)>;
      using ExecCallback_t  = std::function<bool(Ptr<IRenderContext>&)>;

      CallbackPass(
        PassUID_t       const&passId,
        SetupCallback_t     &&setupCb,
        ExecCallback_t      &&execCb);

      bool setup(PassBuilder&builder);
      bool execute(Ptr<IRenderContext>&);

      TPassData const&passData() const { return m_passData; }

    private:
      SetupCallback_t setupCallback;
      ExecCallback_t  execCallback;

      TPassData m_passData;
    };

    template <typename TPassData>
    CallbackPass<TPassData>::CallbackPass(
      PassUID_t       const&passUID,
      SetupCallback_t     &&setupCb,
      ExecCallback_t      &&execCb)
      : PassBase(passUID)
      , setupCallback(setupCb)
      , execCallback(execCb)
      , m_passData()
    {
      assert(passUID > 0);
      assert(setupCb != nullptr);
      assert(execCb  != nullptr);
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
      CallbackPass<TPassData>::execute(Ptr<IRenderContext>&context)
    {
      if(execCallback)
        return execCallback(context);

      return false;
    }
    
  }
}

#endif