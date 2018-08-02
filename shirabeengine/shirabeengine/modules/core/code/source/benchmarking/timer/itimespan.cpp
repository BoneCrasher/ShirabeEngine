#include <algorithm>

#include "core/benchmarking/timer/itimespan.h"

namespace Engine
{
    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CStdSharedPtr_t<ITimespan> operator+(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS)
    {
        return aLHS->operator +(aRHS);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CStdSharedPtr_t<ITimespan> operator-(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS)
    {
        return aLHS->operator -(aRHS);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool operator ==(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS)
    {
        return aLHS->operator ==(aRHS);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool operator <(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS)
    {
        return aLHS->operator <(aRHS);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool operator >(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS)
    {
        return aLHS->operator >(aRHS);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool operator >= (
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS)
    {
        return aLHS->operator >(aRHS) || aLHS->operator ==(aRHS);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    bool operator <= (
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS)
    {
        return aLHS->operator <(aRHS) || aLHS->operator ==(aRHS);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    double UnitConversionFactorImpl(
            double               aBase,
            eTimespanUnit const &aFrom,
            eTimespanUnit const &aTo)
    {
        bool   const invert = (aTo > aFrom);
        double       factor = 1.0;

        eTimespanUnit from = aFrom;
        eTimespanUnit to   = aTo;

        if(invert)
        {
            from = aTo;
            to   = aFrom;
        }

        if(from == eTimespanUnit::SecondsUnit() && to == eTimespanUnit::SubsecondsUnit())
            factor = aBase;
        else if(from == eTimespanUnit::MinutesUnit() && to == eTimespanUnit::SubsecondsUnit())
            factor = aBase * 60;
        else if(from == eTimespanUnit::MinutesUnit() && to == eTimespanUnit::SecondsUnit())
            factor = 60;
        else if(from == eTimespanUnit::HoursUnit() && to == eTimespanUnit::SubsecondsUnit())
            factor = aBase * 60 * 60;
        else if(from == eTimespanUnit::HoursUnit() && to == eTimespanUnit::SecondsUnit())
            factor = 60 * 60;
        else if(from == eTimespanUnit::HoursUnit() && to == eTimespanUnit::MinutesUnit())
            factor = 60;
        else if(from == eTimespanUnit::DaysUnit() && to == eTimespanUnit::SubsecondsUnit())
            factor = aBase * 60 * 60 * 24;
        else if(from == eTimespanUnit::DaysUnit() && to == eTimespanUnit::SecondsUnit())
            factor = 60 * 60 * 24;
        else if(from == eTimespanUnit::DaysUnit() && to == eTimespanUnit::MinutesUnit())
            factor = 60 * 24;
        else if(from == eTimespanUnit::DaysUnit() && to == eTimespanUnit::HoursUnit())
            factor = 24;

        if(invert)
            factor = (1.0 / (double) factor);

        return factor;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
}
