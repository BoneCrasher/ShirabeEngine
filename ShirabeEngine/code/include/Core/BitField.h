#ifndef __SHIRABE_CORE_BITFIELD_H__
#define __SHIRABE_CORE_BITFIELD_H__

#include <type_traits>

namespace Engine {
  namespace Core {

    template <typename TEnum>
    class BitField {
      using Value_t = std::underlying_type_t<TEnum>;

      inline static
        Value_t
        getValue(TEnum const&e)
      {
        return static_cast<Value_t>(e);
      }

    public:
      inline
        BitField()
        : m_bitField(0)
      {}

      inline
        BitField(TEnum const&initial)
        : m_bitField(BitField<TEnum>::getValue(initial))
      {}

      inline
        BitField(TEnum &&initial)
        : m_bitField(BitField<TEnum>::getValue(initial))
      {}

      inline 
        BitField(Value_t const&initial)
        : m_bitField(initial)
      {}

      inline 
        BitField(Value_t &&initial)
        : m_bitField(initial)
      {}

      inline 
        BitField(BitField<TEnum> const&initial)
        : m_bitField(initial.value())
      {}

      inline 
        BitField(BitField<TEnum> &&initial)
        : m_bitField(initial.value())
      {}

      inline
        void
        set(TEnum const&e)
      {
        m_bitField = (m_bitField | BitField<TEnum>::getValue(e));
      }

      inline
        void
        unset(TEnum const&e)
      {
        m_bitField = (m_bitField ^ (m_bitField & BitField<TEnum>::getValue(e)));
      }

      inline
        bool
        check(TEnum const&e) const
      {
        return ((m_bitField & BitField<TEnum>::getValue(e)) == BitField<TEnum>::getValue(e));
      }

      inline 
        bool 
        check(BitField<TEnum> const&bf) const
      {
        return ((m_bitField & bf.value()) == bf.value());
      }

      inline
        Value_t const&
        value() const
      {
        return m_bitField;
      }

      BitField<TEnum> &operator=(TEnum const&other) {
        m_bitField = BitField<TEnum>::getValue(other);
        return (*this);
      }

      BitField<TEnum> &operator=(TEnum &&other) {
        m_bitField = BitField<TEnum>::getValue(other);
        return (*this);
      }

      BitField<TEnum> &operator=(Value_t const&other) {
        m_bitField = other;
        return (*this);
      }

      BitField<TEnum> &operator=(Value_t &&other) {
        m_bitField = other;
        return (*this);
      }

      BitField<TEnum> &operator=(BitField<TEnum> const&other) {
        m_bitField = other.value();
        return (*this);
      }

      BitField<TEnum> &operator=(BitField<TEnum> &&other) {
        m_bitField = other.value();
        return (*this);
      }

      BitField<TEnum> operator|(TEnum const&e)
      {
        BitField<TEnum> c(value());
        c.set(e);
        return c;
      }

      BitField<TEnum> operator&(TEnum const&e)
      {
        BitField<TEnum> c(value() & BitField<TEnum>::getValue(e));
        return c;
      }

    private:
      Value_t m_bitField;
    };

    template <typename TEnum>
    BitField<TEnum> operator|(TEnum const&l, TEnum const&r) {
      return (BitField<TEnum>(l) | r);
    }


  }
}

#endif