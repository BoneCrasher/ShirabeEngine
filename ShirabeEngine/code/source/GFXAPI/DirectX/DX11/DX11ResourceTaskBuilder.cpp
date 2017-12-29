#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBuilder.h"

namespace Engine {
  namespace DX {
    namespace _11 {

      DX11ResourceTaskBuilder
        ::DX11ResourceTaskBuilder(Ptr<DX11Environment> const& device)
        : m_dx11Environment(device)
      {
        assert(device != nullptr);
      }

    }
  }
}
