#ifndef __SHIRABE_BASICTYPES_H__
#define __SHIRABE_BASICTYPES_H__

#include <array>
#include <math.h>
#include <cmath>
#include <stdint.h>

#include "Platform/Platform.h"
#include "Math/Vector.h"

namespace Engine {
  using namespace Engine::Math;

  #ifdef min 
  #undef min
  #endif

  #ifdef max
  #undef max
  #endif
  
  struct SHIRABE_LIBRARY_EXPORT Rect {
    TVector2D<uint16_t> position;
    TVector2D<uint16_t> size;

    Rect();
    Rect(
      long const&x,
      long const&y,
      long const&width,
      long const&height);

    explicit Rect(
      TVector2D<uint16_t> const& pos,
      TVector2D<uint16_t> const& sz);
  };

  #define FRAMEGRAPH_RESOURCERANGE_REMAINING -1

  struct SHIRABE_LIBRARY_EXPORT Range {
    Range();

    Range(
      uint32_t const&inOffset,
      int32_t  const&inLength);

    bool overlapsWith(Range const&other) const;

    uint32_t
      offset;
    int32_t
      length;
  };

  static bool operator==(Range const&lhs, Range const&rhs) {
    return
      ((lhs.offset == rhs.offset) &&
      (lhs.length == rhs.length));
  }

  template <typename T>
  std::string to_string(T const&input) { throw std::runtime_error("Unsupported type."); }

  template <>
  std::string to_string<Range>(Range const&range);
}

#endif