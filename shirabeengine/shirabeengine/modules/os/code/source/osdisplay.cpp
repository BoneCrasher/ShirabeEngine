#include "OS/OSDisplay.h"

namespace engine {
  namespace OS {

    OSDisplayDescriptor::OSDisplayDescriptor()
      : monitorHandle(0)
      , name("")
      , bounds()
      , isPrimary(false)
    {}
  }
}