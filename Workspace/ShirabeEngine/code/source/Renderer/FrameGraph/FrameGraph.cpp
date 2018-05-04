#include "Renderer/FrameGraph/FrameGraph.h"

namespace Engine {
  namespace FrameGraph {

    bool
      FrameGraph::execute()
    {
      // static_assert(false, LOG_FUNCTION( FrameGraph::execute() :  Not implemented (FrameGraph.h Line __LINE__) ));
      Log::Verbose(logTag(), "Executing passes in order:");
      std::stack<PassUID_t> copy = m_passExecutionOrder;
      while(!copy.empty()) {
        Log::Verbose(logTag(), String::format("  Pass %0", copy.top()));
        copy.pop();
      }

      return true;
    }

    PassMap&
      FrameGraph::passes()
    {
      return m_passes;
    }

    bool 
      FrameGraph::addPass(std::string const&name, Ptr<PassBase> const&pass)
    {
      if(m_passes.find(name) != m_passes.end())
        return false;

      m_passes[name] = pass;

      return true;
    }

  }
}