#include "math/geometric/rect.h"

namespace Engine {
  namespace Math {

    CRect::CRect()
      : position({ 0, 0 })
      , size({ 0, 0 })
    {}

    CRect::CRect(
      uint16_t const&aX,
      uint16_t const&aY,
      uint16_t const&aWidth,
      uint16_t const&aHeight)
      : position({ aX, aY })
      , size({ aWidth, aHeight })
    {
    }

    CRect::CRect(
            CVector2D<uint16_t> const& aPosition,
            CVector2D<uint16_t> const& aSize)
      : position(aPosition)
      , size(aSize)
    {
    }

  }
}
