#ifndef __SHIRABE_MATH_GEOMETRIC_RECT_H__
#define __SHIRABE_MATH_GEOMETRIC_RECT_H__

#include <stdint.h>
#include "Math/Vector.h"

namespace Engine {
  namespace Math {

    struct SHIRABE_LIBRARY_EXPORT Rect {
      TVector2D<uint16_t> position;
      TVector2D<uint16_t> size;

      Rect();
      Rect(
        uint16_t const&x,
        uint16_t const&y,
        uint16_t const&width,
        uint16_t const&height);

      explicit Rect(
        TVector2D<uint16_t> const& pos,
        TVector2D<uint16_t> const& sz);
    };

  }
}

#endif