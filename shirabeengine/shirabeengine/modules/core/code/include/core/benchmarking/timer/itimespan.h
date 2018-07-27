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
     * @brief ITimespan is the aBase interface class for arbitrary timespan operations
     *        providing access aTo subseconds, seconds, minutes, hours & days.
     */
    class ITimespan
    {
        SHIRABE_DECLARE_INTERFACE(ITimespan);

    public_typedefs:
        typedef ITimespan my_type;

    public_methods:
        virtual int32_t subseconds() const = 0;
        virtual int32_t seconds()    const = 0;
        virtual int32_t minutes()    const = 0;
        virtual int32_t hours()      const = 0;
        virtual int32_t days()       const = 0;

        virtual double totalSubseconds() const = 0;
        virtual double totalSeconds()    const = 0;
        virtual double totalMinutes()    const = 0;
        virtual double totalHours()      const = 0;
        virtual double totalDays()       const = 0;

        virtual void setTo(double distance, eTimespanUnit unit) = 0;

        virtual int32_t compare(CStdSharedPtr_t<ITimespan> const& other) const = 0;

    public_operators:
        virtual CStdSharedPtr_t<ITimespan> operator+(
                CStdSharedPtr_t<ITimespan> const &other) = 0;

        virtual CStdSharedPtr_t<ITimespan> operator-(
                CStdSharedPtr_t<ITimespan> const &other) = 0;

        virtual bool operator ==(
                CStdSharedPtr_t<ITimespan> const &other) = 0;

        virtual bool operator <(
                CStdSharedPtr_t<ITimespan> const &other) = 0;

        virtual bool operator >(
                CStdSharedPtr_t<ITimespan> const &other) = 0;

        virtual intmax_t subsecondUnits() const = 0;
        virtual double unitConversionFactorForUnit(
                eTimespanUnit to,
                eTimespanUnit aFrom = eTimespanUnit::SubsecondsUnit()) const = 0;
    };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function" // Deactivated since these functions are not used by all static-lib-linkers and might cause those warnings depending on where it's linked.

    /**
     * @brief operator +
     * @param aLHS
     * @param aRHS
     * @return
     */
    static CStdSharedPtr_t<ITimespan> operator+(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * @brief operator -
     * @param aLHS
     * @param aRHS
     * @return
     */
    static CStdSharedPtr_t<ITimespan> operator-(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * @brief operator ==
     * @param aLHS
     * @param aRHS
     * @return
     */
    static bool operator ==(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * @brief operator <
     * @param aLHS
     * @param aRHS
     * @return
     */
    static bool operator <(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * @brief operator >
     * @param aLHS
     * @param aRHS
     * @return
     */
    static bool operator >(
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * @brief operator >=
     * @param aLHS
     * @param aRHS
     * @return
     */
    static bool operator >= (
            CStdSharedPtr_t<ITimespan> const &aLHS,
            CStdSharedPtr_t<ITimespan> const &aRHS);

    /**
     * @brief operator <=
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
     * @brief UnitConversionFactorImpl
     * @param base
     * @param from
     * @param to
     * @return
     */
    static double UnitConversionFactorImpl(
            double        aBase,
            eTimespanUnit aFrom,
            eTimespanUnit aTo);

    /**
     * @brief The Timespan class
     */
    template <intmax_t iSubsecondUnits>
    class CTimespan
            : public ITimespan
    {
    public:
        /**
         * @brief Creates a new instance of this specific timespan type.
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
         * @brief Converts one timespan type aTo another!
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
        typedef typename std::ratio<1, iSubsecondUnits>              ratio_type;
        typedef typename std::chrono::duration<intmax_t, ratio_type> chrono_duration_type;
        typedef CTimespan<iSubsecondUnits>                           my_type;

    public_static_functions:
        /**
         * @brief SecToMillisec
         * @param sec
         * @return
         */
        static double SecToMillisec(double sec)
        {
            return sec * 1000.0;
        }

        /**
         * @brief CTimespan
         */
        CTimespan()
            : mInternalValue(0)
        {
        }

        /**
         * @brief CTimespan
         * @param otherToCopy
         */
        CTimespan(my_type const &otherToCopy)
            : mInternalValue(otherToCopy.totalSubseconds())
        {
        }

        /**
         * @brief CTimespan
         * @param otherToMove
         */
        CTimespan(my_type&& otherToMove)
            : mInternalValue(otherToMove.totalSubseconds())
        {
        }

        /**
         * @brief CTimespan
         * @param aOther
         */
        template <intmax_t iOtherBaseDenominator>
        CTimespan(CTimespan<iOtherBaseDenominator> const &aOther)
            : mInternalValue(chrono_duration_type(aOther).size())
        {}

        /**
         * @brief CTimespan
         * @param initialValue
         */
        explicit CTimespan(intmax_t initialValue)
            : mInternalValue(initialValue) {
        }

        /**
         * @brief CTimespan
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
            chrono_duration_type tmp(aSubseconds);
            tmp += std::chrono::seconds(aSeconds)
                    + std::chrono::minutes(aMinutes)
                    + std::chrono::hours(aHours)
                    + std::chrono::duration<int, std::ratio<86400, 1>>(aDays);

            mInternalValue = tmp;
        }

        /**
         * @brief base
         * @return
         */
        CTimespan<iSubsecondUnits> base()
        {
            return CFactory::create(1);
        }

        /**
         * @brief subseconds
         * @return
         */
        int32_t subseconds() const
        {
            return ( mInternalValue.size() % iSubsecondUnits);
        }

        /**
         * @brief seconds
         * @return
         */
        int32_t seconds() const
        {
            return  ((int32_t) totalSeconds() % 60);
        }

        /**
         * @brief minutes
         * @return
         */
        int32_t minutes() const
        {
            return  ((int32_t) totalMinutes() % 60);
        }

        /**
         * @brief hours
         * @return
         */
        int32_t hours() const
        {
            return  ((int32_t) totalHours() % 24);
        }

        /**
         * @brief days
         * @return
         */
        int32_t days() const
        {
            return  ((int32_t) totalDays() % 365);
        }

        /**
         * @brief totalSubseconds
         * @return
         */
        double totalSubseconds() const
        {
            return (double) mInternalValue.size();
        }

        /**
         * @brief totalSeconds
         * @return
         */
        double totalSeconds() const
        {
            return (double) mInternalValue.size() / (double)iSubsecondUnits;
        }

        /**
         * @brief totalMinutes
         * @return
         */
        double totalMinutes() const
        {
            return (double) mInternalValue.size() / (double)(iSubsecondUnits * 60);
        }

        /**
         * @brief totalHours
         * @return
         */
        double totalHours() const
        {
            return (double) mInternalValue.size() / (double)(iSubsecondUnits * 60 * 60);
        }

        /**
         * @brief totalDays
         * @return
         */
        double totalDays() const
        {
            return (double) mInternalValue.size() / (double)(iSubsecondUnits * 60 * 60 * 24);
        }

        /**
         * @brief compare
         * @param other
         * @return
         */
        int32_t compare(CStdSharedPtr_t<ITimespan> const &aOther) const
        {
            return compareImpl(aOther);
        }

        /**
         * @brief operator +
         * @param aOther
         * @return
         */
        CStdSharedPtr_t<ITimespan> operator+(
                CStdSharedPtr_t<ITimespan> &aOther)
        {
            return add(aOther);
        }

        /**
         * @brief operator -
         * @param aOther
         * @return
         */
        CStdSharedPtr_t<ITimespan> operator-(
                CStdSharedPtr_t<ITimespan> &aOther)
        {
            return sub(aOther);
        }

        /**
         * @brief operator ==
         * @param aOther
         * @return
         */
        bool operator ==(
                CStdSharedPtr_t<ITimespan> &aOther)
        {
            return compare(aOther) == 0;
        }

        /**
         * @brief operator >
         * @param aOther
         * @return
         */
        bool operator>(
                CStdSharedPtr_t<ITimespan> &aOther)
        {
            return compare(aOther) > 0;
        }

        /**
         * @brief operator <
         * @param aOther
         * @return
         */
        bool operator<(
                CStdSharedPtr_t<ITimespan> &aOther)
        {
            return compare(aOther) < 0;
        }

        /**
         * @brief SubsecondUnits
         * @return
         */
        static intmax_t SubsecondUnits()
        {
            return iSubsecondUnits;
        }

        /**
         * @brief subsecondUnits
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
         * @brief setTo
         * @param distance
         * @param unit
         */
        void setTo(double distance, eTimespanUnit unit)
        {
            double const factor = CTimespan<iSubsecondUnits>::unitConversionFactorForUnit(eTimespanUnit::SubsecondsUnit(), unit);
            double const final  = (distance * factor);

            mInternalValue = chrono_duration_type((intmax_t) final);
        }

    private:

        /**
         * @brief add
         * @param aOther
         * @return
         */
        CStdSharedPtr_t<CTimespan<iSubsecondUnits>> add(CStdSharedPtr_t<ITimespan> &aOther)
        {
            return CFactory::create((totalSeconds() + aOther->totalSeconds()) * iSubsecondUnits);
        }

        /**
         * @brief sub
         * @param aOther
         * @return
         */
        CStdSharedPtr_t<CTimespan<iSubsecondUnits>> sub(CStdSharedPtr_t<ITimespan> const &aOther)
        {
            return CFactory::create((totalSeconds() - aOther->totalSeconds()) * iSubsecondUnits);
        }

        /**
         * @brief compareImpl
         * @param aOther
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
        TRet convertTo(chrono_duration_type const &aValue) const
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
                eTimespanUnit aFrom,
                eTimespanUnit aTo)
        {
            return UnitConversionFactorImpl(iSubsecondUnits, aFrom, aTo);
        }

        chrono_duration_type mInternalValue;
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

