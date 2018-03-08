#ifndef __SHIRABE_FRAMEGRAPH_PASS_H__
#define __SHIRABE_FRAMEGRAPH_PASS_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Resources/Core/ResourceDTO.h"

namespace Engine {
  namespace FrameGraph {
    
    class PassBase {
    public:
      virtual bool setup() = 0;
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

      bool setup() { return TPassImplementation::setup(); }
      bool execute() { return TPassImplementation::execute(); }
    };


  }
}

#endif