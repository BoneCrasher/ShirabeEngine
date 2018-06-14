#include "OS/OSDisplay.h"

namespace Engine {
  namespace OS {

    OSDisplayDescriptor::OSDisplayDescriptor()
      : monitorHandle(0)
      , name("")
      , bounds()
      , isPrimary(false)
    {}
  }
}