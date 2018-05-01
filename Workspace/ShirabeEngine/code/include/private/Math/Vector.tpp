#define DefineImmutableGetter(vec_type, component, index)                      \
  template <typename T>                                                        \
  typename vec_type<T>::value_type const                                       \
    vec_type<T>::component() const {                                           \
    return this->m_field[index];                                               \
}

template <typename T, std::size_t N>
TVector<T, N>::TVector()
  : Field<T, sizeof(T), N, 1>()
{ }

template <typename T, std::size_t N>
TVector<T, N>::TVector(std::initializer_list<T> const init)
  : Field<T, sizeof(T), N, 1>(init)
{ }

template <typename T, std::size_t N>
TVector<T, N>::TVector(Field<T, sizeof(T), N, 1> const&f)
  : Field<T, sizeof(T), N, 1>(f)
{}

template <typename T, std::size_t N>
TVector<T, N>::TVector(class_type const& cpy)
  : Field<T, sizeof(T), N, 1>(cpy)
{}

template <typename T, std::size_t N>
bool
TVector<T, N>::operator==(TVector<T, N> const&other)
{
  return Field<T, sizeof(T), N, 1>::operator==(static_cast<Field<T, sizeof(T), N, 1>>(other));
}

template <typename T, std::size_t N>
typename TVector<T, N>::class_type
TVector<T, N>::scale(value_type const factor)
{
  this->operator*=(factor);

  return *this;
}

template <typename T, std::size_t N>
typename TVector<T, N>::value_type
TVector<T, N>::length()
{
  return sqrt(this->squared_length());
}

template <typename T, std::size_t N>
typename TVector<T, N>::value_type
TVector<T, N>::squared_length()
{
  value_type        len = 0;
  const value_type *ptr = this->const_ptr();

  for(size_t i = 0; i < this->size(); ++i)
    len += (ptr[i] * ptr[i]);

  return len;
}

template <typename T, std::size_t N>
typename TVector<T, N>::value_type
TVector<T, N>::abs()
{
  return this->length();
}

template <typename T, std::size_t N>
typename TVector<T, N>::class_type&
TVector<T, N>::normalize()
{
  // For now use the "paper-version" of normalization!
  this->operator/=(this->length());

  return *this;
}

template <typename T, std::size_t N>
TVector<T, N> operator +(
  TVector<T, N> const& l,
  TVector<T, N> const& r)
{
  return
    operator+(
      static_cast<typename TVector<T, N>::base_type>(l),
      static_cast<typename TVector<T, N>::base_type>(r));
}

template <typename T, std::size_t N>
TVector<T, N> operator -(
  TVector<T, N> const&l,
  TVector<T, N> const&r)
{
  return
    operator-(
      static_cast<typename TVector<T, N>::base_type>(l),
      static_cast<typename TVector<T, N>::base_type>(r));
}

template <typename T, std::size_t N>
TVector<T, N> operator *(
  TVector<T, N>                      const&l,
  typename TVector<T, N>::value_type const&f)
{
  return
    operator*(
      static_cast<typename TVector<T, N>::base_type>(l),
      f);
}

template <typename T, std::size_t N>
TVector<T, N> operator *(
  typename TVector<T, N>::value_type const&f,
  TVector<T, N>                      const&l)
{
  return
    operator*(
      static_cast<typename TVector<T, N>::base_type>(l),
      f);
}

template <typename T, std::size_t N>
TVector<T, N> operator /(
  TVector<T, N>                      const&l,
  typename TVector<T, N>::value_type const&f)
{
  return
    operator/(
      static_cast<typename TVector<T, N>::base_type>(l),
      f);
}

template <typename T, std::size_t N>
TVector<T, N> operator /(
  typename TVector<T, N>::value_type const&f,
  TVector<T, N>                      const&l)
{
  return
    operator/(
      static_cast<typename TVector<T, N>::base_type>(l),
      f);
}

template <typename T>
TVector1D<T>::TVector1D()
  : TVector<T, 1>()
{
  value_type *ptr = this->ptr();
  ptr[0] = 0.0f;
}

template <typename T>
TVector1D<T>::TVector1D(
  value_type const x)
  : TVector<T, 1>({ x })
{ }

template <typename T>
TVector1D<T>::TVector1D(class_type const& cpy)
  : TVector<T, 1>(cpy)
{}

DefineImmutableGetter(TVector1D, x, 0);

template <typename T>
void
TVector1D<T>::x(value_type const& val)
{
  this->m_field[0] = val;
}

template <typename T>
TVector1D<T>
TVector1D<T>::right() { return TVector1D<T>({ static_cast<T>(1) }); }

template <typename T>
TVector2D<T>::TVector2D()
  : TVector<T, 2>()
{
  value_type *ptr = this->ptr();
  ptr[0] = 0.0f;
  ptr[1] = 0.0f;
}

template <typename T>
TVector2D<T>::TVector2D(
  value_type const x,
  value_type const y)
  : TVector<T, 2>({ x, y })
{ }

template <typename T>
TVector2D<T>::TVector2D(class_type const& cpy)
  : TVector<T, 2>(cpy)
{}

DefineImmutableGetter(TVector2D, x, 0);
DefineImmutableGetter(TVector2D, y, 1);

template <typename T>
void
TVector2D<T>::x(value_type const& val)
{
  this->m_field[0] = val;
}

template <typename T>
void
TVector2D<T>::y(value_type const& val)
{
  this->m_field[1] = val;
}

template <typename T>
TVector2D<T>
TVector2D<T>::right()
{
  return TVector2D<T>({ 1, 0 });
}

template <typename T>
TVector2D<T>
TVector2D<T>::up()
{
  return TVector2D<T>({ 0, 1 });
}

template <typename T>
TVector3D<T>::TVector3D()
  : TVector<T, 3>({ 0.0f, 0.0f, 0.0f })
{
  value_type *ptr = this->ptr();
  ptr[2] = 0.0f;
}

template <typename T>
TVector3D<T>::TVector3D(
  value_type const x,
  value_type const y,
  value_type const z)
  : TVector<T, 3>({ x, y, z })
{
}

template <typename T>
TVector3D<T>::TVector3D(
  TVector2D<T> const&v,
  T           const&z)
  : TVector<T, 3>(v.x(), v.y(), z)
{}

template <typename T>
TVector3D<T>::TVector3D(class_type const& cpy)
  : TVector<T, 3>(cpy)
{}

// Return a copy of the stored x-component. Getter.


DefineImmutableGetter(TVector3D, x, 0);
DefineImmutableGetter(TVector3D, y, 1);
DefineImmutableGetter(TVector3D, z, 2);

// Return a FieldAccessor for the stored x-component to safely assign a new value 
// using the assignment-operator. Setter.

template <typename T>
void
TVector3D<T>::x(value_type const& val)
{
  this->m_field[0] = val;
}
// Return a FieldAccessor for the stored y-component to safely assign a new value 
// using the assignment-operator. Setter.

template <typename T>
void
TVector3D<T>::y(value_type const& val)
{
  this->m_field[1] = val;
}
// Return a FieldAccessor for the stored z-component to safely assign a new value 
// using the assignment-operator. Setter.

template <typename T>
void
TVector3D<T>::z(value_type const& val)
{
  this->m_field[2] = val;
}


template <typename T>
TVector3D<T>
TVector3D<T>::forward()
{
  return TVector3D({ 1, 0, 0 });
}

template <typename T>
TVector3D<T>
TVector3D<T>::right()
{
  return TVector3D({ 0, 1, 0 });
}

template <typename T>
TVector3D<T>
TVector3D<T>::up()
{
  return TVector3D({ 0, 0, 1 });
}

template <typename T>
TVector4D<T>::TVector4D()
  : TVector<T, 4>({ 0.0f, 0.0f, 0.0f, 0.0f })
{
}

template <typename T>
TVector4D<T>::TVector4D(
  value_type const x,
  value_type const y,
  value_type const z,
  value_type const w)
  : TVector<T, 4>({ x, y, z, w })
{
}

template <typename T>
TVector4D<T>::TVector4D(class_type const& cpy)
  : TVector<T, 4>(cpy)
{}

template <typename T>
TVector4D<T>::TVector4D(
  TVector3D<T> const& other,
  T           const& w)
  : TVector<T, 4>({ other.x(), other.y(), other.z(), w })
{}

template <typename T>
TVector4D<T>::TVector4D(
  TVector2D<T> const&other,
  T           const&z,
  T           const&w)
  : TVector<T, 4>({ other.x(), other.y(), z, w })
{}

DefineImmutableGetter(TVector4D, x, 0);
DefineImmutableGetter(TVector4D, y, 1);
DefineImmutableGetter(TVector4D, z, 2);
DefineImmutableGetter(TVector4D, w, 3);

template <typename T>
void
TVector4D<T>::x(value_type const& val)
{
  this->m_field[0] = val;
}

template <typename T>
void
TVector4D<T>::y(value_type const& val)
{
  this->m_field[1] = val;
}

template <typename T>
void
TVector4D<T>::z(value_type const& val)
{
  this->m_field[2] = val;
}

template <typename T>
void
TVector4D<T>::w(value_type const& val)
{
  this->m_field[3] = val;
}

template <typename T>
typename TVector2D<T>::value_type dot(TVector2D<T> const&l, TVector2D<T> const&r)
{
  return ((l.x() * r.x()) + (l.y() * r.y()));
}

template <typename T>
typename  TVector3D<T>::value_type dot(TVector3D<T> const&l, TVector3D<T> const&r)
{
  return ((l.x() * r.x())
    + (l.y() * r.y())
    + (l.z() * r.z()));
}

template <typename T>
typename TVector4D<T>::value_type dot(TVector4D<T> const&l, TVector4D<T> const&r)
{
  return
    ((l.x() * r.x()) +
    (l.y() * r.y()) +
      (l.z() * r.z()) +
      (l.w() + r.w()));
}

template <typename T>
TVector3D<T> cross(TVector3D<T> const&l, TVector3D<T> const&r)
{
  return
    TVector3D<T>(
    (l.y()*r.z() - l.z()*r.y()),
      (l.z()*r.x() - l.x()*r.z()),
      (l.x()*r.y() - l.y()*r.x()));
}

template <typename T>
TVector2D<T> scale(
  TVector2D<T>                      const&vec,
  typename TVector2D<T>::value_type const&factor)
{
  return TVector2D<T>(vec).scale(factor);
}

template <typename T>
TVector3D<T> scale(
  TVector3D<T>                      const&vec,
  typename TVector3D<T>::value_type const&factor)
{
  return TVector3D<T>(vec).scale(factor);
}

template <typename T>
TVector4D<T> scale(
  TVector4D<T>                      const&vec,
  typename TVector4D<T>::value_type const&factor)
{
  return TVector4D<T>(vec).scale(factor);
}

template <typename T, size_t N>
TVector<T, N> normalize(TVector<T, N> const&vec)
{
  return *(TVector<T, N>(vec).normalize());
}