#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Resources/Core/ResourceDTO.h"

#include "Pass.h"


namespace Engine {
  namespace FrameGraph {


    class FrameGraph {
      friend class GraphBuilder;

    public:
      bool
        execute();

    private:
      inline PassBaseList & passes() { return m_passes; }

      PassBaseList m_passes;
    };

  }
}

#endif