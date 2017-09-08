#ifndef _TIMESPANUNIT_H_
#define _TIMESPANUNIT_H_

#include <string>

namespace Engine {
    typedef struct ETimespanUnit {
    private:
        enum {
            SUBSECONDS = 1,
            SECONDS = 2,
            MINUTES = 4,
            HOURS = 8,
            DAYS = 16
        };

    public:
        static ETimespanUnit SubsecondsUnit() {
            return { ETimespanUnit::SUBSECONDS, "Subseconds" };
        }

        static ETimespanUnit SecondsUnit() {
            return { ETimespanUnit::SECONDS, "Seconds" };
        }

        static ETimespanUnit MinutesUnit() {
            return { ETimespanUnit::MINUTES, "Minutes" };
        }

        static ETimespanUnit HoursUnit() {
            return { ETimespanUnit::HOURS, "Hours" };
        }

        static ETimespanUnit DaysUnit() {
            return { ETimespanUnit::DAYS, "Days" };
        }

        static ETimespanUnit fromValue(int value) {
            switch(value) {
            default:
            case ETimespanUnit::SUBSECONDS:
                return SubsecondsUnit();
            case ETimespanUnit::SECONDS:
                return SecondsUnit();
            case ETimespanUnit::MINUTES:
                return MinutesUnit();
            case ETimespanUnit::HOURS:
                return HoursUnit();
            case ETimespanUnit::DAYS:
                return DaysUnit();
            }
        }

        operator int() const { return _value; }
        operator std::string() const { return _name; }

        int         _value;
        std::string _name;
    } eTimespanUnit;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function" // Deactivated since these functions are not used by all static-lib-linkers and might cause those warnings depending on where it's linked.
    static bool operator<(ETimespanUnit l, ETimespanUnit r) { return l._value < r._value; }
    static bool operator>(ETimespanUnit l, ETimespanUnit r) { return l._value > r._value; }
    static bool operator==(ETimespanUnit l, ETimespanUnit r) { return l._value == r._value; }
    static bool operator<=(ETimespanUnit l, ETimespanUnit r) { return l < r || l == r; }
    static bool operator>=(ETimespanUnit l, ETimespanUnit r) { return l > r || l == r; }
#pragma GCC diagnostic pop

}

#endif // _TIMESPANUNIT_H_

