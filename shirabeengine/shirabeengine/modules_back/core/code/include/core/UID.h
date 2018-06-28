#ifndef __SHIRABE_CORE_UID_H__
#define __SHIRABE_CORE_UID_H__

#include "Core/EngineTypeHelper.h"

namespace Engine {
  namespace Core {

    template <typename TUIDUnderlyingType>
    DeclareTemplatedInterface(IUIDGenerator, Template(IUIDGenerator<TUIDUnderlyingType>));

    virtual TUIDUnderlyingType generate() = 0;

    DeclareInterfaceEnd(IUIDGenerator);

  }
}

#endif