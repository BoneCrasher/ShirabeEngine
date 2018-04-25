#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Resources/Core/ResourceDTO.h"

#include "Renderer/FrameGraph/Pass.h"
#include "Renderer/FrameGraph/FrameGraphData.h"

namespace Engine {
  namespace FrameGraph {

    class SHIRABE_TEST_EXPORT FrameGraph {
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
    };

  }
}

#endif