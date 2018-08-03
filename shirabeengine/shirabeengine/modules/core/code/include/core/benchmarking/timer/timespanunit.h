#ifndef __TIMESPANUNIT_H__
#define __TIMESPANUNIT_H__

#include <string>

namespace engine {
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

        operator int() const { return m_value; }
        operator std::string() const { return m_name; }

        int         m_value;
        std::string m_name;
    } eTimespanUnit;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function" // Deactivated since these functions are not used by all static-lib-linkers and might cause those warnings depending on where it's linked.
    static bool operator<(ETimespanUnit l, ETimespanUnit r) { return l.m_value < r.m_value; }
    static bool operator>(ETimespanUnit l, ETimespanUnit r) { return l.m_value > r.m_value; }
    static bool operator==(ETimespanUnit l, ETimespanUnit r) { return l.m_value == r.m_value; }
    static bool operator<=(ETimespanUnit l, ETimespanUnit r) { return l < r || l == r; }
    static bool operator>=(ETimespanUnit l, ETimespanUnit r) { return l > r || l == r; }
#pragma GCC diagnostic pop

}

#endif // m_TIMESPANUNIT_H_

