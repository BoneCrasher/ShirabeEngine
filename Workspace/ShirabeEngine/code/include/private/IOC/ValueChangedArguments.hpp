#ifndef m_VALUECHANGEDARGUMETNS_H_
#define m_VALUECHANGEDARGUMETNS_H_

#include <memory>

namespace DataBinding {
    template <typename T>
    // Generic container encapsulating an old and a new value reflecting a value change by assignment.
    class ValueChangedArguments {
    public:
        ValueChangedArguments(const T oldValue, const T newValue)
            : m_oldValue(oldValue), m_newValue(newValue)
        {}

        ~ValueChangedArguments() { }

        const T getOldValue() const { return m_oldValue; }
        const T getNewValue() const { return m_newValue; }

    private:
        T m_oldValue, m_newValue;
    };
}

#endif
