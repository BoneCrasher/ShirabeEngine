#include "Math/Geometric/Rect.h"

namespace Engine {
  namespace Math {

    Rect::Rect()
      : position(0, 0)
      , size(0, 0)
    {}

    Rect::Rect(
      long const&x,
      long const&y,
      long const&width,
      long const&height)
      : position(x, y)
      , size(width, height)
    {
    }

    Rect::Rect(
      TVector2D<uint16_t> const& pos,
      TVector2D<uint16_t> const& sz)
      : position(pos)
      , size(sz)
    {
    }

  }
}
