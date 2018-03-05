#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBackend.h"

namespace Engine {
  namespace DX {
    namespace _11 {

      DX11ResourceTaskBackend
        ::DX11ResourceTaskBackend(Ptr<DX11Environment> const& device)
        : GFXAPIResourceTaskBackend<EngineTypes>()
        , m_dx11Environment(device)
      {
        assert(device != nullptr);
      }

    }
  }
}
