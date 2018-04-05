#ifndef __SHIRABE_CORE_BASICTYPES_TPP__
#define __SHIRABE_CORE_BASICTYPES_TPP__

template <typename T, std::size_t N>
TVectorND<T, N>::TVectorND()
{ }

template <typename T, std::size_t N>
TVectorND<T, N>::TVectorND(std::initializer_list<T> const& init)
  : m_values()
{
  int32_t k=0;
  for(const T& v : init) {
    if(k >= N)
      break;

    m_values[k++] = v;
  }
}
template <typename T, std::size_t N>
template <typename U>
TVectorND<T, N>::TVectorND(TVectorND<U, N> const& other)
{
  this->operator=<U, N>(other);
}

template <typename T, std::size_t N>
template <typename U, std::size_t NU>
TVectorND<T, N>::TVectorND(TVectorND<U, NU> const& other)
{
  this->operator=<U, NU>(other);
}

template <typename T, std::size_t N>
template <typename U>
TVectorND<T, N>&
TVectorND<T, N>::operator =(
  TVectorND<typename std::enable_if<std::is_convertible<U, T>::value, U>::type, N> const&other)
{
  m_values.fill(0);
  std::copy(other.values().begin(), other.values().end(), m_values.begin());
  return *this;
}

template <typename T, std::size_t N>
template <typename U, std::size_t NU>
TVectorND<T, N>&
TVectorND<T, N>::operator =(
  TVectorND<typename std::enable_if<std::is_convertible<U, T>::value, U>::type, NU> const&other)
{
  m_values.fill(0);
  std::copy(other.values().begin(), other.values().begin() + (std::min(N, NU)), m_values.begin());
  return *this;
}

template <typename T, std::size_t N>
T const&
TVectorND<T, N>::operator[](std::size_t const& index) const
{
  if(index < 0 || index >= N)
    throw EEngineStatus::OutOfBounds;

  return m_values[index];
}

template <typename T, std::size_t N>
T&
TVectorND<T, N>::operator[](std::size_t const& index)
{
  if(index < 0 || index >= N)
    throw EEngineStatus::OutOfBounds;
  return m_values[index];
}

template <typename T, std::size_t N>
bool
TVectorND<T, N>::operator==(TVectorND<T, N> const& other) const
{
  return (m_values == other.m_values);
}

template <typename T, std::size_t N>
std::array<T, N> const&
TVectorND<T, N>::values() const
{
  return m_values;
}

#endif
