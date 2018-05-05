#include "Renderer/FrameGraph/FrameGraph.h"

namespace Engine {
  namespace FrameGraph {

    bool
      Graph::execute()
    {
      // static_assert(false, LOG_FUNCTION( Graph::execute() :  Not implemented (Graph.h Line __LINE__) ));
      Log::Verbose(logTag(), "Executing passes in order:");
      std::stack<PassUID_t> copy = m_passExecutionOrder;
      while(!copy.empty()) {
        Log::Verbose(logTag(), String::format("  Pass %0", copy.top()));
        copy.pop();
      }

      return true;
    }

    PassMap&
      Graph::passes()
    {
      return m_passes;
    }

    bool 
      Graph::addPass(Ptr<PassBase> const&pass)
    {
      if(m_passes.find(pass->passUID()) != m_passes.end())
        return false;

      m_passes[pass->passUID()] = pass;

      return true;
    }

  }
}