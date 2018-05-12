#include "Core/BasicTypes.h"
#include "Core/String.h"

namespace Engine {


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

  Range::Range()
    : offset(0)
    , length(0)
  {}

  Range::Range(
    uint32_t const&inOffset,
    int32_t  const&inLength)
    : offset(inOffset)
    , length(inLength)
  {}

  bool
    Range::overlapsWith(Range const&other) const
  {
    // Pretest: Both ranges must be at least 1 unit in length to check anything...
    if(!(length && other.length))
      return false;

    bool overlap = false;

    if(offset == other.offset)
      overlap = true;
    else if(offset < other.offset)
      overlap = ((((int32_t)(offset + length)) - ((int32_t)other.offset)) > 0);
    else
      overlap = ((((int32_t)(other.offset + other.length)) - ((int32_t)offset)) > 0);

    return overlap;
  }

  template <>
  std::string to_string<Range>(Range const&range) {
    return String::format("[off:%0,len:%1]", range.offset, range.length);
  }
}