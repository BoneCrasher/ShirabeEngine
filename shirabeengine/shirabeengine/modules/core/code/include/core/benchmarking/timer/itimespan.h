#ifndef __SHIRABE_TIMESPAN_H__
#define __SHIRABE_TIMESPAN_H__

#include <memory>
#include <chrono>
#include <ctime>

#include <string>
#include <sstream>
#include <iomanip>

#include "core/enginetypehelper.h"
#include "core/benchmarking/timer/timespanunit.h"

namespace Engine
{

    namespace Helper
    {
        /**
         * Returns the sign of a value.
         *
         * @tparam Type of the value.
         */
        template<class T>
        int32_t signum(T aValue)
        {
            if(aValue < T(0))
                return T(-1);

            return (aValue > T(0));
        }
    }

    //-------------------------------------------------------------------------------------------------------
    //
    //                                                 Timespan
    //
    //-------------------------------------------------------------------------------------------------------
    // A timespan represents a deterministic amount of units expressing a point32_t or duration as temporal units.
    // aTo support different "Time-Systems", i.e. Frames per Second, Milliseconds, etc, the "Subsecond-Unit"
    // was created.
    //
    // A Subsecond is the n-th fractional part of a second, i.e. if n subseconds are accumulated, the result
    // will be an approx. second.
    //
    //-------------------------------------------------------------------------------------------------------
    class ITimespan;

    /**
     * ITimespan is the aBase interface class for arbitrary timespan operations
     * providing access aTo subseconds, seconds, minutes, hours & days.
     */
    class ITimespan
    {
        SHIRABE_DECLARE_INTERFACE(ITimespan);

    public_typedefs:
        typedef ITimespan ClassType_t;

    public_methods:
        /**
         * Return the number of subseconds in the timespan clamped to 1..1000
         *
         * @return
         */
        virtual int32_t subseconds() const = 0;
        /**
         * Return the number of seconds in the timespan clamped to 1..60
         *
         * @return
         */
        virtual int32_t seconds() const = 0;
        /**
         * Return the number of minutes in the timespan clamped to 1..60
         *
         * @return
         */
        virtual int32_t minutes() const = 0;
        /**
         * Return the number of hours in the timespan clamped to 1..24
         *
         * @return
         */
        virtual int32_t hours() const = 0;
        /**
         * Return the number of days in the timespan clamped to 1..365
         *
         * @return
         */
        virtual int32_t days() const = 0;

        /**
         * Return the unclamped total number of subseconds in the timespan.
         *
         * @return
         */
        virtual double totalSubseconds() const = 0;
        /**
         * Return the unclamped total number of seconds in the timespan.
         *
         * @return
         */
        virtual double totalSeconds() const = 0;
        /**
         * Return the unclamped total number of minutes in the timespan.
         *
         * @return
         */
        virtual double totalMinutes() const = 0;
        /**
         * Return the unclamped total number of hours in the timespan.
         *
         * @return
         */
        virtual double totalHours() const = 0;
        /**
         * Return the unclamped total number of days in the timespan.
         *
         * @return
         */
        virtual double totalDays() const = 0;

        /**
         * Set the timespan value to an arbitrary multiple of a given timespan unit, e.g. 5 milliseconds.
         *
         * @param distance
         * @param unit
         */
        virtual void setTo(double const &aDistance, eTimespanUnit const &aUnit) = 0;

        /**
         * Compare this timespan to anaOther for it's relative position.
         * If the returned value is less than 0, this instance is located
         * earlier in the timeline.
         *
         * @param aOther
         * @return
         */
        virtual int32_t compare(CStdSharedPtr_t<ITimespan> const& aOther) const = 0;

    public_operators:
        /**
         * Add one timespan to this timespan.
         *
         * @param aOther
         * @return
         */
        virtual CStdSharedPtr_t<ITimespan> operator+(
                CStdSharedPtr_t<ITimespan> const &aOther) = 0;

        /**
         * Subtract one timespan from this instance.
         *
         * @param aOther
         * @return
         */
        virtual CStdSharedPtr_t<ITimespan> operator-(
                CStdSharedPtr_t<ITimespan> const &aOther) = 0;

        /**
         * Compare this timespan with anaOther for equality.
         *
         * @param aOther
         * @return
         */
        virtual bool operator ==(
                CStdSharedPtr_t<ITimespan> const &aOther) = 0;

        /**
         * Test whether this timespan is located earlier in the timeline.
         *
         * @param aOther
         * @return
         */
        virtual bool operator <(
                CStdSharedPtr_t<ITimespan> const &aOther) = 0;

        /**
         * Test whether this timespan is located later in the timeline.
         *
         * @param aOther
         * @return
         */
        virtual bool operator >(
                CStdSharedPtr_t<ITimespan> const &aOther) = 0;

        /**
         * Returns the number of elements inside a subsecond unit.
         * 1000 for milliseconds.
         * 24   for PAL
         * 25   for NTSC
         * etc...
         *
         * @return
         */
        virtual intmax_t subsecondUnits() const = 0;

        /**
         * Calculates the conversion factor required to go from a specific timespan unit to anaOther.
         *
         * @param aTo
         * @param aFrom
         * @return
         */
        virtual double unitConversionFactorForUnit(
                eTimespanUnit aTo,
                eTimespanUnit aFrom = eTimespanUnit::SubsecondsUnit()) const = 0;
    };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function" // Deactivated since these functions are not used by all static-lib-linkers and might cause those warnings depending on where it's linked.

    /**
     * Add to timespans and return the result as a copy.
     *
     * @param aLHS
     * @param aRHS
     * @return
     */
    static CStdSharedPtr_t<ITimespan> operator+(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * Subtract one timespan from another and return the result as a copy.
     *
     * @param aLHS
     * @param aRHS
     * @return
     */
    static CStdSharedPtr_t<ITimespan> operator-(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * Compare two timespan instances for equality.
     *
     * @param aLHS
     * @param aRHS
     * @return
     */
    static bool operator ==(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * Test, whether aLHS is located earlier in the timeline than aRHS.
     *
     * @param aLHS
     * @param aRHS
     * @return
     */
    static bool operator <(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * Test, whether aLHS is located later in the timeline than aRHS.
     *
     * @param aLHS
     * @param aRHS
     * @return
     */
    static bool operator >(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * Test, whether aLHS is located later in the timeline than aRHS or at the same time point.
     *
     * @param aLHS
     * @param aRHS
     * @return
     */
    static bool operator >= (
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * Test, whether aLHS is located earlier in the timeline than aRHS or at the same time point.
     * @param aLHS
     * @param aRHS
     * @return
     */
    static bool operator <= (
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);
    //<-----------------------------------------------------------------------------
#pragma GCC diagnostic pop

    /**
     * Calculates the conversion factor for a conversion from aFrom to aTo, based
     * on a subsecond unit lenght of "base".
     * @param aBase
     * @param aFrom
     * @param aTo
     * @return
     */
    static double UnitConversionFactorImpl(
            double              aBase,
            eTimespanUnit const &aFrom,
            eTimespanUnit const &aTo);

    /**
     * CTimespan provides a default implementation of ITimespan.
     */
    template <intmax_t iSubsecondUnits>
    class CTimespan
            : public ITimespan
    {
    public_classes:
        /**
         * Creates a new instance of this specific timespan type.
         */
        class CFactory
        {
        public_static_functions:
            static CStdSharedPtr_t<CTimespan<iSubsecondUnits>> create(intmax_t units)
            {
                return makeCStdSharedPtr<CTimespan<iSubsecondUnits>>(units);
            }
        };

        /**
         * Converts one timespan type aTo anaOther!
         */
        template <intmax_t iSourceUnitsPerSecond,
                  intmax_t iTargetUnitsPerSecond>
        class CConverter
        {
        public_static_functions:
            static CTimespan<iTargetUnitsPerSecond> convert(CTimespan<iSourceUnitsPerSecond> aFrom)
            { }
        };

    public_typedefs:
        using RatioType_t          = typename std::ratio<1, iSubsecondUnits>              ;
        using ChronoDurationType_t = typename std::chrono::duration<intmax_t, RatioType_t>;
        using ClassType_t          = CTimespan<iSubsecondUnits>                           ;

    public_static_functions:
        /**
         * Convert a second value to it's millisecond representation.
         * @param sec
         * @return
         */
        static double SecToMillisec(double sec)
        {
            return sec * 1000.0;
        }

    public_constructors:
        /**
         * Construct a zero timespan.
         */
        CTimespan()
            : mInternalValue(0)
        {
        }

        /**
         * Copy-Construct a timespan from another.
         *
         * @param aOtherToCopy
         */
        CTimespan(ClassType_t const &aOtherToCopy)
            : mInternalValue(aOtherToCopy.totalSubseconds())
        {
        }

        /**
         * Move-Construct a timespan from another.
         *
         * @param aOtherToMove
         */
        CTimespan(ClassType_t&& aOtherToMove)
            : mInternalValue(aOtherToMove.totalSubseconds())
        {
        }

        /**
         * Create a Timespan from another timespan of another subsecond resolution.
         *
         * @param aaOther
         */
        template <intmax_t iaOtherBaseDenominator>
        CTimespan(CTimespan<iaOtherBaseDenominator> const &aaOther)
            : mInternalValue(ChronoDurationType_t(aaOther).size())
        {}

        /**
         * Create a Timespan from a subsecond based time value.
         *
         * @param initialValue
         */
        explicit CTimespan(intmax_t initialValue)
            : mInternalValue(initialValue) {
        }

        /**
         * Construct a timespan from explicit time component values.
         *
         * @param aDays
         * @param aHours
         * @param aMinutes
         * @param aSeconds
         * @param aSubseconds
         */
        explicit CTimespan(
                int32_t aDays,
                int32_t aHours,
                int32_t aMinutes,
                int32_t aSeconds,
                int32_t aSubseconds)
        {
            ChronoDurationType_t tmp(aSubseconds);
            tmp += std::chrono::seconds(aSeconds)
                    + std::chrono::minutes(aMinutes)
                    + std::chrono::hours(aHours)
                    + std::chrono::duration<int, std::ratio<86400, 1>>(aDays);

            mInternalValue = tmp;
        }

        /**
         * Return the current base of this timespan unit.
         *
         * @return
         */
        CTimespan<iSubsecondUnits> base()
        {
            return CFactory::create(1);
        }

        /**
         * Return the number of subseconds, clamped to 0..subseconds-max
         *
         * @return
         */
        int32_t subseconds() const
        {
            return ( mInternalValue.size() % iSubsecondUnits);
        }

        /**
         * Return the number of seconds, clamped to 0..60
         *
         * @return
         */
        int32_t seconds() const
        {
            return  ((int32_t) totalSeconds() % 60);
        }

        /**
         * Return the number of minutes, clamped to 0..60
         *
         * @return
         */
        int32_t minutes() const
        {
            return  ((int32_t) totalMinutes() % 60);
        }

        /**
         * Return the number of hours, clamped to 0..24.
         *
         * @return
         */
        int32_t hours() const
        {
            return  ((int32_t) totalHours() % 24);
        }

        /**
         * Return the number of days, clamped to 0.365.
         *
         * @return
         */
        int32_t days() const
        {
            return  ((int32_t) totalDays() % 365);
        }

        /**
         * Return the total number of subseconds representing this timespan.
         *
         * @return
         */
        double totalSubseconds() const
        {
            return (double) mInternalValue.size();
        }

        /**
         * Return the total number of seconds representing this timespan.
         *
         * @return
         */
        double totalSeconds() const
        {
            return (double) mInternalValue.size() / (double)iSubsecondUnits;
        }

        /**
         * Return the total number of minutes representing this timespan.
         *
         * @return
         */
        double totalMinutes() const
        {
            return (double) mInternalValue.size() / (double)(iSubsecondUnits * 60);
        }

        /**
         * Return the total number of hours representing this timespan.
         *
         * @return
         */
        double totalHours() const
        {
            return (double) mInternalValue.size() / (double)(iSubsecondUnits * 60 * 60);
        }

        /**
         * Return the total number of days representing this timespan.
         *
         * @return
         */
        double totalDays() const
        {
            return (double) mInternalValue.size() / (double)(iSubsecondUnits * 60 * 60 * 24);
        }

        /**
         * Compare this timespan to another for timeline relative position.
         *
         * @param aOther
         * @return
         */
        int32_t compare(CStdSharedPtr_t<ITimespan> const &aOther) const
        {
            return compareImpl(aOther);
        }

        /**
         * Add another timespan to this instance.
         *
         * @param aaOther
         * @return
         */
        CStdSharedPtr_t<ITimespan> operator+(
                CStdSharedPtr_t<ITimespan> &aOther)
        {
            return add(aOther);
        }

        /**
         * Subtract another timespan from this instance.
         *
         * @param aaOther
         * @return
         */
        CStdSharedPtr_t<ITimespan> operator-(
                CStdSharedPtr_t<ITimespan> &aOther)
        {
            return sub(aOther);
        }

        /**
         * Compare this timespan and another for equality.
         *
         * @param aaOther
         * @return
         */
        bool operator ==(
                CStdSharedPtr_t<ITimespan> &aOther)
        {
            return compare(aOther) == 0;
        }

        /**
         * Compare this instance to another for less-than relation.
         *
         * @param aaOther
         * @return        True, if the other instance is located earlier in the timeline.
         */
        bool operator>(
                CStdSharedPtr_t<ITimespan> &aOther)
        {
            return compare(aOther) > 0;
        }

        /**
         * Compare this instance to another for greater-than relation.
         *
         * @param aaOther
         * @return        True, if the other instance is located later in the timeline.
         */
        bool operator<(
                CStdSharedPtr_t<ITimespan> &aOther)
        {
            return compare(aOther) < 0;
        }

        /**
         * Return the current subsecond units of this timespan.
         * 1000     Milliseconds
         * 24       PAL
         * 25       NTSC
         * @return
         */
        static intmax_t SubsecondUnits()
        {
            return iSubsecondUnits;
        }

        /**
         * Return the current subsecond units of this timespan.
         * 1000     Milliseconds
         * 24       PAL
         * 25       NTSC
         * @return
         */
        intmax_t subsecondUnits() const
        {
            return SubsecondUnits();
        }

        /**
         * @brief unitConversionFactorForUnit
         * @param aTo
         * @param aFrom
         * @return
         */
        double unitConversionFactorForUnit(
                eTimespanUnit aTo,
                eTimespanUnit aFrom = eTimespanUnit::SubsecondsUnit()) const
        {
            return CTimespan<iSubsecondUnits>::UnitConversionFactor(aFrom, aTo);
        }

        /**
         * Set the timespan value to an arbitrary multiple of a given timespan unit, e.g. 5 milliseconds.
         *
         * @param aDistance
         * @param aUnit
         */
        void setTo(double const &aDistance, eTimespanUnit const &aUnit)
        {
            double const factor = CTimespan<iSubsecondUnits>::unitConversionFactorForUnit(eTimespanUnit::SubsecondsUnit(), aUnit);
            double const final  = (aDistance * factor);

            mInternalValue = ChronoDurationType_t((intmax_t) final);
        }

    private:

        /**
         * @brief add
         * @param aaOther
         * @return
         */
        CStdSharedPtr_t<CTimespan<iSubsecondUnits>> add(CStdSharedPtr_t<ITimespan> &aOther)
        {
            return CFactory::create((totalSeconds() + aOther->totalSeconds()) * iSubsecondUnits);
        }

        /**
         * @brief sub
         * @param aaOther
         * @return
         */
        CStdSharedPtr_t<CTimespan<iSubsecondUnits>> sub(CStdSharedPtr_t<ITimespan> const &aOther)
        {
            return CFactory::create((totalSeconds() - aOther->totalSeconds()) * iSubsecondUnits);
        }

        /**
         * @brief compareImpl
         * @param aaOther
         * @return
         */
        int32_t compareImpl(CStdSharedPtr_t<ITimespan> &aOther) const
        {
            double const diff = (this->totalSeconds() - aOther->totalSeconds());

            int32_t result = 0;
            if(diff > 0)
                result = 1;

            if(diff < 0)
                result = -1;

            return result;
        }

        /**
         * @brief convertTo
         * @param aValue
         * @return
         */
        template <typename TRet, typename TPeriod>
        TRet convertTo(ChronoDurationType_t const &aValue) const
        {
            return std::chrono::duration_cast<std::chrono::duration<TRet, TPeriod>>(aValue).size();
        }

        /**
         * @brief UnitConversionFactor
         * @param from
         * @param to
         * @return
         */
        static double UnitConversionFactor(
                eTimespanUnit const &aFrom,
                eTimespanUnit const &aTo)
        {
            return UnitConversionFactorImpl(iSubsecondUnits, aFrom, aTo);
        }

        ChronoDurationType_t mInternalValue;
    };

    using Timespan_t    = CTimespan<100>;
    using Timespan24FPS = CTimespan<24> ;
    using Timespan25FPS = CTimespan<25> ;

    /**
     * @brief MakeTimespanPtr
     * @param aUnits
     * @return
     */
    template <int32_t iSubseconds>
    static CStdSharedPtr_t<ITimespan> MakeTimespanPtr(intmax_t aUnits)
    {
        return std::make_shared<CTimespan<iSubseconds>>(aUnits);
    }

    /**
     * @brief FromTimeString
     * @param aTimeString
     * @return
     */
    template <int32_t iSubseconds>
    static CStdSharedPtr_t<CTimespan<iSubseconds>> FromTimeString(std::string aTimeString)
    {
        std::stringstream timestream(aTimeString);
        int32_t HH, mm, ss, mss;
        std::string strHH, strmm, strss, strmss;
        std::getline(timestream, strHH, ':');
        std::getline(timestream, strmm, ':');
        std::getline(timestream, strss, '.');

        if(aTimeString.rfind('.') != std::string::npos)
        {
            std::getline(timestream, strmss);
        }
        else
            strmss = "000";

        std::stringstream sstrHH(strHH);
        std::stringstream sstrMM(strmm);
        std::stringstream sstrss(strss);
        std::stringstream sstrmss(strmss);
        sstrHH  >> HH;
        sstrMM  >> mm;
        sstrss  >> ss;
        sstrmss >> mss;

        return makeCStdSharedPtr<CTimespan<iSubseconds>>(0, HH, mm, ss, mss);
    }

    /**
     * @brief ToTimeString
     * @param aTimespan
     * @param aPrecision
     * @return
     */
    static std::string ToTimeString(CStdSharedPtr_t<ITimespan> aTimespan, int32_t aPrecision = 3)
    {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << aTimespan->hours()   << ":";
        ss << std::setfill('0') << std::setw(2) << aTimespan->minutes() << ":";
        ss << std::setfill('0') << std::setw(2) << aTimespan->seconds() << ".";
        ss << std::setw(aPrecision) << std::setprecision(aPrecision)
           << (int)(((double) aTimespan->subseconds() / (double) aTimespan->subsecondUnits()) * 1000.0);

        return ss.str();
    }
}

#endif // m_TIMESPAN_H_

