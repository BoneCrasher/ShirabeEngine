#include "math/geometric/rect.h"

namespace Engine {
  namespace Math {

    Rect::Rect()
      : position({ 0, 0 })
      , size({ 0, 0 })
    {}

    Rect::Rect(
      uint16_t const&x,
      uint16_t const&y,
      uint16_t const&width,
      uint16_t const&height)
      : position({ x, y })
      , size({ width, height })
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
