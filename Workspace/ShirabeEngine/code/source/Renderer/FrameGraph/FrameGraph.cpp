#include "Renderer/FrameGraph/FrameGraph.h"

namespace Engine {
  namespace FrameGraph {

    bool
      Graph::execute(Ptr<IRenderContext>&renderContext)
    {
      assert(renderContext != nullptr);

      FrameGraphResources resources(m_resources);

      std::stack<PassUID_t> copy = m_passExecutionOrder;
      while(!copy.empty()) {
        PassUID_t     passUID = copy.top();
        Ptr<PassBase> pass    = m_passes.at(passUID);

        if(!pass->execute(resources, renderContext)) {
          Log::Error(logTag(), String::format("Failed to execute pass %0", pass->passUID()));
        }

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