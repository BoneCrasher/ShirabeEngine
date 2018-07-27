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
            double        aBase,
            eTimespanUnit aFrom,
            eTimespanUnit aTo)
    {
        bool   const invert = (aTo > aFrom);
        double       factor = 1.0;

        if(invert)
        {
            std::swap(aTo, aFrom);
        }

        if(aFrom == eTimespanUnit::SecondsUnit() && aTo == eTimespanUnit::SubsecondsUnit())
            factor = aBase;
        else if(aFrom == eTimespanUnit::MinutesUnit() && aTo == eTimespanUnit::SubsecondsUnit())
            factor = aBase * 60;
        else if(aFrom == eTimespanUnit::MinutesUnit() && aTo == eTimespanUnit::SecondsUnit())
            factor = 60;
        else if(aFrom == eTimespanUnit::HoursUnit() && aTo == eTimespanUnit::SubsecondsUnit())
            factor = aBase * 60 * 60;
        else if(aFrom == eTimespanUnit::HoursUnit() && aTo == eTimespanUnit::SecondsUnit())
            factor = 60 * 60;
        else if(aFrom == eTimespanUnit::HoursUnit() && aTo == eTimespanUnit::MinutesUnit())
            factor = 60;
        else if(aFrom == eTimespanUnit::DaysUnit() && aTo == eTimespanUnit::SubsecondsUnit())
            factor = aBase * 60 * 60 * 24;
        else if(aFrom == eTimespanUnit::DaysUnit() && aTo == eTimespanUnit::SecondsUnit())
            factor = 60 * 60 * 24;
        else if(aFrom == eTimespanUnit::DaysUnit() && aTo == eTimespanUnit::MinutesUnit())
            factor = 60 * 24;
        else if(aFrom == eTimespanUnit::DaysUnit() && aTo == eTimespanUnit::HoursUnit())
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
