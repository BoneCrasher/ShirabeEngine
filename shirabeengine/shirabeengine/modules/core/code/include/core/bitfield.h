#ifndef __SHIRABE_CORE_BITFIELD_H__
#define __SHIRABE_CORE_BITFIELD_H__

#include <type_traits>

namespace Engine {
  namespace Core {

    template <typename TEnum>
    class BitField {
    public:
      using Value_t = std::underlying_type_t<TEnum>;

      static
        Value_t getValue(TEnum const&e);

      BitField();
      BitField(TEnum const&initial);
      BitField(TEnum &&initial);
      BitField(Value_t const&initial);
      BitField(Value_t &&initial);
      BitField(BitField<TEnum> const&initial);
      BitField(BitField<TEnum> &&initial);

      operator Value_t();

      void set(TEnum const&e);
      void unset(TEnum const&e);

      bool check(TEnum const&e) const;
      bool check(BitField<TEnum> const&bf) const;

      Value_t const&value() const;

      BitField<TEnum> &operator=(TEnum const&other);
      BitField<TEnum> &operator=(TEnum &&other);
      BitField<TEnum> &operator=(Value_t const&other);
      BitField<TEnum> &operator=(Value_t &&other);
      BitField<TEnum> &operator=(BitField<TEnum> const&other);
      BitField<TEnum> &operator=(BitField<TEnum> &&other);
      BitField<TEnum> operator|(TEnum const&e);
      BitField<TEnum> operator&(TEnum const&e);

    private:
      Value_t m_bitField;
    };


    template <typename TEnum>
    typename BitField<TEnum>::Value_t
      BitField<TEnum>::getValue(TEnum const&e)
    {
      return static_cast<Value_t>(e);
    }

    template <typename TEnum>
    BitField<TEnum>::BitField()
      : m_bitField(0)
    {}

    template <typename TEnum>
    BitField<TEnum>::BitField(TEnum const&initial)
      : m_bitField(BitField<TEnum>::getValue(initial))
    {}

    template <typename TEnum>
    BitField<TEnum>::BitField(TEnum &&initial)
      : m_bitField(BitField<TEnum>::getValue(initial))
    {}

    template <typename TEnum>
    BitField<TEnum>::BitField(Value_t const&initial)
      : m_bitField(initial)
    {}

    template <typename TEnum>
    BitField<TEnum>::BitField(Value_t &&initial)
      : m_bitField(initial)
    {}

    template <typename TEnum>
    BitField<TEnum>::BitField(BitField<TEnum> const&initial)
      : m_bitField(initial.value())
    {}

    template <typename TEnum>
    BitField<TEnum>::BitField(BitField<TEnum> &&initial)
      : m_bitField(initial.value())
    {}

    template <typename TEnum>
    BitField<TEnum>::operator Value_t()
    {
      return static_cast<std::underlying_type_t<TEnum>>(m_bitField);
    }

    template <typename TEnum>
    void
      BitField<TEnum>::set(TEnum const&e)
    {
      m_bitField = (m_bitField | BitField<TEnum>::getValue(e));
    }

    template <typename TEnum>
    void
      BitField<TEnum>::unset(TEnum const&e)
    {
      m_bitField = (m_bitField ^ (m_bitField & BitField<TEnum>::getValue(e)));
    }

    template <typename TEnum>
    bool
      BitField<TEnum>::check(TEnum const&e) const
    {
      return ((m_bitField & BitField<TEnum>::getValue(e)) == BitField<TEnum>::getValue(e));
    }

    template <typename TEnum>
    bool
      BitField<TEnum>::check(BitField<TEnum> const&bf) const
    {
      return ((m_bitField & bf.value()) == bf.value());
    }

    template <typename TEnum>
    typename BitField<TEnum>::Value_t const&
      BitField<TEnum>::value() const
    {
      return m_bitField;
    }

    template <typename TEnum>
    BitField<TEnum>& BitField<TEnum>::operator=(TEnum const&other) {
      m_bitField = BitField<TEnum>::getValue(other);
      return (*this);
    }

    template <typename TEnum>
    BitField<TEnum>& BitField<TEnum>::operator=(TEnum &&other) {
      m_bitField = BitField<TEnum>::getValue(other);
      return (*this);
    }

    template <typename TEnum>
    BitField<TEnum>& BitField<TEnum>::operator=(Value_t const&other) {
      m_bitField = other;
      return (*this);
    }

    template <typename TEnum>
    BitField<TEnum>& BitField<TEnum>::operator=(Value_t &&other) {
      m_bitField = other;
      return (*this);
    }

    template <typename TEnum>
    BitField<TEnum>& BitField<TEnum>::operator=(BitField<TEnum> const&other) {
      m_bitField = other.value();
      return (*this);
    }

    template <typename TEnum>
    BitField<TEnum>& BitField<TEnum>::operator=(BitField<TEnum> &&other) {
      m_bitField = other.value();
      return (*this);
    }

    template <typename TEnum>
    BitField<TEnum> BitField<TEnum>::operator|(TEnum const&e)
    {
      BitField<TEnum> c(value());
      c.set(e);
      return c;
    }

    template <typename TEnum>
    BitField<TEnum> BitField<TEnum>::operator&(TEnum const&e)
    {
      BitField<TEnum> c(value() & BitField<TEnum>::getValue(e));
      return c;
    }

    template <typename TEnum>
    BitField<TEnum> operator|(TEnum const&l, TEnum const&r) {
      return (BitField<TEnum>(l) | r);
    }
  }
}

#endif