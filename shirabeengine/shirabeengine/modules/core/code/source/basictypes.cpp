#include "Core/BasicTypes.h"
#include "Core/String.h"

namespace Engine {

  
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