#ifndef __SHIRABE_BASICTYPES_H__
#define __SHIRABE_BASICTYPES_H__

#include <array>
#include <math.h>
#include <cmath>
#include <stdint.h>

#include <platform/platform.h>
#include <math/geometric/rect.h>

namespace Engine
{
    using namespace Engine::Math;

    #ifdef min
    #undef min
    #endif

    #ifdef max
    #undef max
    #endif

    #define FRAMEGRAPH_RESOURCECRange_REMAINING -1

    struct SHIRABE_LIBRARY_EXPORT CRange
    {
        CRange();

        CRange(uint32_t const &aOffset,
               int32_t  const &aLength);

        bool overlapsWith(CRange const &aOther) const;

        uint32_t
          offset;
        int32_t
          length;
    };

    static bool operator==(
            CRange const &aLHS,
            CRange const &aRHS)
    {
        return
            ((aLHS.offset == aRHS.offset) &&
             (aLHS.length == aRHS.length));
    }

    template <typename T>
    T from_string(std::string const &aInput)
    {
        T output{ };

        std::stringstream ss;
        ss << aInput;
        ss >> output;

        return output;
    }

    template <typename T>
    std::string to_string(T const &aInput)
    {
        std::string output{ };

        std::stringstream ss;
        ss << aInput;
        output = ss.str();

        return output;
    }

    template <>
    std::string to_string<CRange>(CRange const&aRange);
}

#endif
