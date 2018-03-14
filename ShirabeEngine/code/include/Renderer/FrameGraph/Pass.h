#ifndef __SHIRABE_FRAMEGRAPH_PASS_H__
#define __SHIRABE_FRAMEGRAPH_PASS_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Resources/Core/ResourceDTO.h"
#include "Renderer/IRenderer.h"

#include "PassLinker.h"

namespace Engine {
  namespace FrameGraph {
    using namespace Renderer;
        
    class GraphBuilder;

    class PassBase {
    public:
      virtual bool setup(GraphBuilder&graphBuilder) = 0;
      virtual bool execute(Ptr<IRenderContext>&)    = 0;
    };

    DeclareSharedPointerType(PassBase);
    DeclareListType(Ptr<PassBase>, PassBase);

    template <typename TPassImplementation>
    class Pass
      : public PassBase
    {
    public:
      using PassImplementation_t = TPassImplementation;

      Pass(
        FrameGraphResourceId_t         const&passUID,
        UniquePtr<TPassImplementation>      &implementation)
        : m_passUID(passUID)
        , m_implementation(std::move(implementation))
      {
        assert(m_implementation != nullptr);
      }

      FrameGraphResourceId_t const&passUID() const { return m_passUID; }

      bool setup(GraphBuilder&graphBuilder) {
        PassLinker<TPassImplementation> passLinker(passUID());
        
        typename PassImplementation_t::InputData  &input  = passLinker.input();
        typename PassImplementation_t::OutputData &output = passLinker.output();

        bool setupSuccessful = m_implementation->setup(passLinker, input, output);
      }

      bool execute(
        Ptr<IRenderContext>&context) 
      {
        return true; // TPassImplementation::execute(context, input, output);
      }

    private:
      FrameGraphResourceId_t         m_passUID;
      UniquePtr<TPassImplementation> m_implementation;
    };


  }
}

#endif