#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__

#include <string>
#include <functional>
#include <stack>

#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"
#include "Resources/Core/ResourceDTO.h"

#include "Renderer/FrameGraph/Pass.h"
#include "Renderer/FrameGraph/FrameGraphData.h"

namespace Engine {
  namespace FrameGraph {

    class SHIRABE_TEST_EXPORT FrameGraph {
      DeclareLogTag(FrameGraph);

      friend class GraphBuilder;

    public:
      FrameGraph()  = default;
      ~FrameGraph() = default;

      bool
        execute();

    private:
      PassMap &passes();

      bool addPass(std::string const&, Ptr<PassBase> const&);

      PassMap m_passes;

      std::stack<PassUID_t> m_passExecutionOrder;
    };

  }
}

#endif