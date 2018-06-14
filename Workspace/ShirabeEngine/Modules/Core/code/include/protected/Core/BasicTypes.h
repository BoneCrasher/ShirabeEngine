#ifndef __SHIRABE_BASICTYPES_H__
#define __SHIRABE_BASICTYPES_H__

#include <array>
#include <math.h>
#include <cmath>
#include <stdint.h>

#include "Platform/Platform.h"
#include "Math/Geometric/Rect.h"

namespace Engine {
  using namespace Engine::Math;

  #ifdef min 
  #undef min
  #endif

  #ifdef max
  #undef max
  #endif
    
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
  T from_string(std::string const&input) {
    T output{ };

    std::stringstream ss;
    ss << input;
    ss >> output;

    return output;
  }

  template <typename T>
  std::string to_string(T const&input)
  {
    std::string output{ };

    std::stringstream ss;
    ss << input;
    output = ss.str();

    return output;
  }

  template <>
  std::string to_string<Range>(Range const&range);
}

#endif