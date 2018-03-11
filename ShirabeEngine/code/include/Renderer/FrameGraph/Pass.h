#ifndef __SHIRABE_FRAMEGRAPH_PASS_H__
#define __SHIRABE_FRAMEGRAPH_PASS_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Resources/Core/ResourceDTO.h"

namespace Engine {
  namespace FrameGraph {

#include <string>
    
    class GraphBuilder;

    class PassBase {
    public:
      virtual bool setup(GraphBuilder&graphBuilder) = 0;
      virtual bool execute() = 0;
    };
    DeclareSharedPointerType(PassBase);
    DeclareListType(Ptr<PassBase>, PassBase);

    template <typename TPassImplementation>
    class Pass
      : public PassBase
    {
    public:
      using PassImplementation_t = TPassImplementation;

      bool setup(PassBuilder<TPassImplementation>&passBuilder) {
        return TPassImplementation::setup(passBuilder);
      }

      bool execute() { 
        return TPassImplementation::execute(); 
      }

    };


  }
}

#endif