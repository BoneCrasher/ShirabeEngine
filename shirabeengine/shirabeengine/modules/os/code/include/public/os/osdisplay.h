#ifndef __SHIRABE_OS_DISPLAY_H__
#define __SHIRABE_OS_DISPLAY_H__

#include <stdint.h>
#include <string>

#include "Platform/Platform.h"

#include "OS/OSDef.h"
#include "Math/Geometric/Rect.h"

namespace engine {
  namespace OS {

    using engine::Math::Rect;

    struct SHIRABE_LIBRARY_EXPORT OSDisplayDescriptor {
      OSHandle     monitorHandle;
      std::string  name;
      Rect         bounds;
      bool         isPrimary;

      OSDisplayDescriptor();
    };

    template <typename TProvider>
    class OSDisplay
      : public TProvider
    {};
  }
}

#endif