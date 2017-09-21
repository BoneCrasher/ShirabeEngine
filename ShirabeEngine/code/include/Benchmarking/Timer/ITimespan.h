#ifndef __SHIRABE_TIMESPAN_H__
#define __SHIRABE_TIMESPAN_H__

#include <memory>
#include <chrono>
#include <ctime>

#include <string>
#include <sstream>
#include <iomanip>

#include "Core/EngineTypeHelper.h"
#include "Benchmarking/Timer/TimespanUnit.h"

namespace Engine {

    namespace Helper {
        template<class T>
        int signum(T t)
        {
            if(t < T(0))
                return T(-1);
            return t > T(0);
        }
    }

    //-------------------------------------------------------------------------------------------------------
    //
    //                                                 Timespan
    //
    //-------------------------------------------------------------------------------------------------------
    // A timespan represents a deterministic amount of units expressing a point or duration as temporal units.
    // To support different "Time-Systems", i.e. Frames per Second, Milliseconds, etc, the "Subsecond-Unit"
    // was created.
    //
    // A Subsecond is the n-th fractional part of a second, i.e. if n subseconds are accumulated, the result
    // will be an approx. second.
    //
    //-------------------------------------------------------------------------------------------------------
	class ITimespan; 
	DeclareSharedPointerType(ITimespan);

    /**
     * @brief ITimespan is the base interface class for arbitrary timespan operations
     *        providing access to subseconds, seconds, minutes, hours & days.
     */
	DeclareInterface(ITimespan);
        typedef ITimespan my_type;

        virtual int subseconds() const = 0;
        virtual int seconds()    const = 0;
        virtual int minutes()    const = 0;
        virtual int hours()      const = 0;
        virtual int days()       const = 0;

        virtual double totalSubseconds() const = 0;
        virtual double totalSeconds()    const = 0;
        virtual double totalMinutes()    const = 0;
        virtual double totalHours()      const = 0;
        virtual double totalDays()       const = 0;

        virtual void setTo(double distance, eTimespanUnit unit) = 0;

        virtual int compare(const ITimespanPtr& other) const = 0;

        virtual ITimespanPtr operator+(
                const ITimespanPtr& other) = 0;

        virtual ITimespanPtr operator-(
                const ITimespanPtr& other) = 0;

        virtual bool operator ==(
                const ITimespanPtr& other) = 0;

        virtual bool operator <(
                const ITimespanPtr& other) = 0;

        virtual bool operator >(
                const ITimespanPtr& other) = 0;

        virtual intmax_t subsecondUnits() const = 0;
        virtual double unitConversionFactorForUnit(
                eTimespanUnit to,
                eTimespanUnit from = eTimespanUnit::SubsecondsUnit()) const = 0;

    private:
        // Actual pure virtual declarations. To be overridden in child classes.

    DeclareInterfaceEnd(ITimespan)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function" // Deactivated since these functions are not used by all static-lib-linkers and might cause those warnings depending on where it's linked.
    static ITimespanPtr operator+(
            const ITimespanPtr& l,
            const ITimespanPtr& r) {
        return l->operator +(r);
    }

    static ITimespanPtr operator-(
            const ITimespanPtr& l,
            const ITimespanPtr& r) {
        return l->operator -(r);
    }

    static bool operator ==(
            const ITimespanPtr& l,
            const ITimespanPtr& r) {
        return l->operator ==(r);
    }

    static bool operator <(
            const ITimespanPtr& l,
            const ITimespanPtr& r) {
        return l->operator <(r);
    }

    static bool operator >(
            const ITimespanPtr& l,
            const ITimespanPtr& r) {
        return l->operator >(r);
    }

    static bool operator >= (
            const ITimespanPtr& l,
            const ITimespanPtr& r) {
        return l->operator >(r) || l->operator ==(r);
    }

    static bool operator <= (
            const ITimespanPtr& l,
            const ITimespanPtr& r) {
        return l->operator <(r) || l->operator ==(r);
    }
#pragma GCC diagnostic pop

    static double UnitConversionFactorImpl(
            double        base,
            eTimespanUnit from,
            eTimespanUnit to) {
        bool invert = (to > from);
        double factor = 1.0;

        if(invert) {
            eTimespanUnit tmp = to;
            to = from;
            from = tmp;
        }

        if(from == eTimespanUnit::SecondsUnit() && to == eTimespanUnit::SubsecondsUnit())
            factor = base;
        else if(from == eTimespanUnit::MinutesUnit() && to == eTimespanUnit::SubsecondsUnit())
            factor = base * 60;
        else if(from == eTimespanUnit::MinutesUnit() && to == eTimespanUnit::SecondsUnit())
            factor = 60;
        else if(from == eTimespanUnit::HoursUnit() && to == eTimespanUnit::SubsecondsUnit())
            factor = base * 60 * 60;
        else if(from == eTimespanUnit::HoursUnit() && to == eTimespanUnit::SecondsUnit())
            factor = 60 * 60;
        else if(from == eTimespanUnit::HoursUnit() && to == eTimespanUnit::MinutesUnit())
            factor = 60;
        else if(from == eTimespanUnit::DaysUnit() && to == eTimespanUnit::SubsecondsUnit())
            factor = base * 60 * 60 * 24;
        else if(from == eTimespanUnit::DaysUnit() && to == eTimespanUnit::SecondsUnit())
            factor = 60 * 60 * 24;
        else if(from == eTimespanUnit::DaysUnit() && to == eTimespanUnit::MinutesUnit())
            factor = 60 * 24;
        else if(from == eTimespanUnit::DaysUnit() && to == eTimespanUnit::HoursUnit())
            factor = 24;

        if(invert)
            factor = 1.0 / (double) factor;

        return factor;
    }


    template <intmax_t iSubsecondUnits>
    class Timespan
            : public ITimespan
    {
    public:
        /**
         * @brief Creates a new instance of this specific timespan type.
         */
        class Factory {
        public:
            static inline std::shared_ptr<Timespan<iSubsecondUnits>> create(intmax_t units) {
                return std::make_shared<Timespan<iSubsecondUnits>>(units);
            }
        };

        template <intmax_t iSourceUnitsPerSecond,
                  intmax_t iTargetUnitsPerSecond>
        /**
         * @brief Converts one timespan type to another!
         */
        class Converter {
        public:
            static inline Timespan<iTargetUnitsPerSecond> convert(
                    Timespan<iSourceUnitsPerSecond> from) {
            }
        };

        // Some convenience helpers
        typedef typename std::ratio<1, iSubsecondUnits>              ratio_type;
        typedef typename std::chrono::duration<intmax_t, ratio_type> chrono_duration_type;
        typedef Timespan<iSubsecondUnits>                            my_type;

        static double SecToMillisec(double sec) {
            return sec * 1000.0;
        }

        Timespan()
            : _internalValue(0)
        {
        }

        Timespan(const my_type& otherToCopy)
            : _internalValue(otherToCopy.totalSubseconds()) {
        }

        Timespan(my_type&& otherToMove)
            : _internalValue(otherToMove.totalSubseconds()) {
        }

        template <intmax_t iOtherBaseDenominator>
        Timespan(const Timespan<iOtherBaseDenominator>& other)
            : _internalValue(chrono_duration_type(other).size())
        {}

        explicit Timespan(intmax_t initialValue)
            : _internalValue(initialValue) {
        }

        explicit Timespan(int days,
                          int hours,
                          int minutes,
                          int seconds,
                          int subseconds) {
            chrono_duration_type tmp(subseconds);
            tmp += std::chrono::seconds(seconds)
                    + std::chrono::minutes(minutes)
                    + std::chrono::hours(hours)
                    + std::chrono::duration<int, std::ratio<86400, 1>>(days);
            _internalValue = tmp;
        }

        inline Timespan<iSubsecondUnits> base() { return Factory::create(1); }

        inline int subseconds() const { return (_internalValue.size() % iSubsecondUnits); }
        inline int seconds() const { return  ((int) totalSeconds() % 60); }
        inline int minutes() const { return  ((int) totalMinutes() % 60); }
        inline int hours()   const { return  ((int) totalHours() % 24); }
        inline int days()    const { return  ((int) totalDays() % 365); }

        inline double totalSubseconds() const { return (double) _internalValue.size(); }
        inline double totalSeconds()    const { return (double) _internalValue.size() / (double)iSubsecondUnits; }
        inline double totalMinutes()    const { return (double) _internalValue.size() / (double)(iSubsecondUnits * 60); }
        inline double totalHours()      const { return (double) _internalValue.size() / (double)(iSubsecondUnits * 60 * 60); }
        inline double totalDays()       const { return (double) _internalValue.size() / (double)(iSubsecondUnits * 60 * 60 * 24); }

        inline int compare(const ITimespanPtr& other) const
        { return compareImpl(other); }

        inline ITimespanPtr operator+(
                const ITimespanPtr& other) {
            return add(other);
        }

        inline ITimespanPtr operator-(
                const ITimespanPtr& other) {
            return sub(other);
        }

        inline bool operator ==(
                const ITimespanPtr& other) {
            return compare(other) == 0;
        }

        inline bool operator>(
                const ITimespanPtr& other) {
            return compare(other) > 0;
        }

        inline bool operator<(
                const ITimespanPtr& other) {
            return compare(other) < 0;
        }

        static intmax_t SubsecondUnits() { return iSubsecondUnits; }

        inline intmax_t subsecondUnits() const {
            return SubsecondUnits();
        }

        double unitConversionFactorForUnit(
                eTimespanUnit to,
                eTimespanUnit from = eTimespanUnit::SubsecondsUnit()) const {
            return Timespan<iSubsecondUnits>::UnitConversionFactor(from, to);
        }

        void setTo(double distance, eTimespanUnit unit) {
            double factor = Timespan<iSubsecondUnits>::unitConversionFactorForUnit(eTimespanUnit::SubsecondsUnit(), unit);
            double final = distance * factor;

            _internalValue = chrono_duration_type((intmax_t) final);
        }

    private:

        inline std::shared_ptr<Timespan<iSubsecondUnits>> add(
                const ITimespanPtr& other) {
            return Factory::create((totalSeconds() + other->totalSeconds()) * iSubsecondUnits);
        }

        inline std::shared_ptr<Timespan<iSubsecondUnits>> sub(
                const ITimespanPtr& other) {
            return Factory::create((totalSeconds() - other->totalSeconds()) * iSubsecondUnits);
        }


        inline int compareImpl(const ITimespanPtr& other) const {
            double diff = this->totalSeconds() - other->totalSeconds();
            int result = 0;
            if(diff > 0)
                result = 1;

            if(diff < 0)
                result = -1;

            return result;
        }

        template <typename TRet, typename TPeriod>
        inline TRet convertTo(const chrono_duration_type& inValue) const {
            return std::chrono::duration_cast<std::chrono::duration<TRet, TPeriod>>(inValue).size();
        }

        static double UnitConversionFactor(
                eTimespanUnit from,
                eTimespanUnit to) {
            return UnitConversionFactorImpl(iSubsecondUnits, from, to);
        }

        chrono_duration_type _internalValue;
    };

    typedef Timespan<100> Timespan_t;
    typedef Timespan<24>   Timespan24FPS;
    typedef Timespan<25>   Timespan25FPS;

    template <int iSubseconds>
    static ITimespanPtr MakeTimespanPtr(intmax_t units) {
        return std::make_shared<Timespan<iSubseconds>>(units);
    }

    template <int iSubseconds>
    static std::shared_ptr<Timespan<iSubseconds>> FromTimeString(std::string timeStr) {
        std::stringstream timestream(timeStr);
        int HH, mm, ss, mss;
        std::string strHH, strmm, strss, strmss;
        std::getline(timestream, strHH, ':');
        std::getline(timestream, strmm, ':');
        std::getline(timestream, strss, '.');
        if(timeStr.rfind('.') != std::string::npos) {
            std::getline(timestream, strmss);
        } else
            strmss = "000";

        std::stringstream sstrHH(strHH);
        std::stringstream sstrMM(strmm);
        std::stringstream sstrss(strss);
        std::stringstream sstrmss(strmss);
        sstrHH  >> HH;
        sstrMM  >> mm;
        sstrss  >> ss;
        sstrmss >> mss;

        return std::make_shared<Timespan<iSubseconds>>(0, HH, mm, ss, mss);
    }

    static std::string ToTimeString(ITimespanPtr timespan, int precision = 3) {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << timespan->hours()   << ":";
        ss << std::setfill('0') << std::setw(2) << timespan->minutes() << ":";
        ss << std::setfill('0') << std::setw(2) << timespan->seconds() << ".";
        ss << std::setw(precision) << std::setprecision(precision) << (int)(((double) timespan->subseconds() / (double) timespan->subsecondUnits()) * 1000);
        return ss.str();
    }
}

#endif // _TIMESPAN_H_

