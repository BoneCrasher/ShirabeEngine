#include "core/basictypes.h"
#include <base/string.h>

namespace engine
{
    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CRange::CRange()
      : offset(0)
      , length(0)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CRange::CRange(
            uint32_t const &aOffset,
            int32_t  const &aLength)
        : offset(aOffset)
        , length(aLength)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool CRange::overlapsWith(CRange const &aOther) const
    {
        // Pretest: Both CRanges must be at least 1 unit in length to check anything...
        bool const lengthValid = (length && aOther.length);
        if(!lengthValid)
          return false;

        bool overlap = false;

        if(offset == aOther.offset)
          overlap = true;
        else if(offset < aOther.offset)
          overlap = (( (static_cast<int32_t>(offset) + length) - static_cast<int32_t>(aOther.offset) ) > 0);
        else
          overlap = (( (static_cast<int32_t>(aOther.offset) + aOther.length) - static_cast<int32_t>(offset) ) > 0);

        return overlap;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <>
    std::string convert_to_string<CRange>(CRange const& aRange)
    {
        return CString::format("[off:{},len:{}]", aRange.offset, aRange.length);
    }
    //<-----------------------------------------------------------------------------
}
