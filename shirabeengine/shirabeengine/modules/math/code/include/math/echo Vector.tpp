#define DefineImmutableGetter(vec_type, vec_size, component, index)\
  template <typename T>                                            \
  typename vec_type<T, vec_size>::value_type const                 \
    vec_type<T, vec_size>::component() const {                     \
    return this->m_field[index];                                   \
}

template <typename T>
TVectorImpl<T, 1>::TVectorImpl()
  : Field<T, sizeof(T), N, 1>({ 0 })
{ }

template <typename T>
TVectorImpl<T, 1>::TVectorImpl(
  value_type const x)
  : Field<T, sizeof(T), 1, 1>({ x })
{ }

template <typename T>
TVectorImpl<T, 1>::TVectorImpl(class_type const& cpy)
  : Field<T, sizeof(T), 1, 1>(cpy)
{}

template <typename T>
TVectorImpl<T, 1>::TVectorImpl(std::initializer_list<T> const init)
  : Field<T, sizeof(T), 1, 1>(init)
{ }

template <typename T>
TVectorImpl<T, 1>::TVectorImpl(Field<T, sizeof(T), 1, 1> const&f)
  : Field<T, sizeof(T), 1, 1>(f)
{}

DefineImmutableGetter(TVectorImpl, 1, x, 0);

template <typename T>
void
TVectorImpl<T, 1>::x(value_type const& val)
{
  this->m_field[0] = val;
}

template <typename T>
TVectorImpl<T, 1>
TVectorImpl<T, 1>::right() { return TVectorImpl<T, 1>({ static_cast<T>(1) }); }

template <typename T>
TVectorImpl<T, 2>::TVectorImpl()
  : Field<T, sizeof(T), 2, 1>({ T(0), T(0) })
{ }

template <typename T>
TVectorImpl<T, 2>::TVectorImpl(
  value_type const x,
  value_type const y)
  : Field<T, sizeof(T), 2, 1>({ x, y })
{ }

template <typename T>
TVectorImpl<T, 2>::TVectorImpl(class_type const& cpy)
  : Field<T, sizeof(T), 2, 1>(cpy)
{}

template <typename T>
TVectorImpl<T, 2>::TVectorImpl(std::initializer_list<T> const init)
  : Field<T, sizeof(T), 2, 1>(init)
{ }

template <typename T>
TVectorImpl<T, 2>::TVectorImpl(Field<T, sizeof(T), 2, 1> const&f)
  : Field<T, sizeof(T), 2, 1>(f)
{}


DefineImmutableGetter(TVectorImpl, 2, x, 0);
DefineImmutableGetter(TVectorImpl, 2, y, 1);

template <typename T>
void
TVectorImpl<T, 2>::x(value_type const& val)
{
  this->m_field[0] = val;
}

template <typename T>
void
TVectorImpl<T, 2>::y(value_type const& val)
{
  this->m_field[1] = val;
}

template <typename T>
TVectorImpl<T, 2>
TVectorImpl<T, 2>::right()
{
  return TVectorImpl<T, 2>({ 1, 0 });
}

template <typename T>
TVectorImpl<T, 2>
TVectorImpl<T, 2>::up()
{
  return TVectorImpl<T, 2>({ 0, 1 });
}

template <typename T>
TVectorImpl<T, 3>::TVectorImpl()
  : Field<T, sizeof(T), 3, 1>({ T(0), T(0), T(0) })
{ }

template <typename T>
TVectorImpl<T, 3>::TVectorImpl(
  value_type const x,
  value_type const y,
  value_type const z)
  : Field<T, sizeof(T), 3, 1>({ x, y, z })
{ }

template <typename T>
TVectorImpl<T, 3>::TVectorImpl(class_type const& cpy)
  : Field<T, sizeof(T), 3, 1>(cpy)
{}

template <typename T>
TVectorImpl<T, 3>::TVectorImpl(std::initializer_list<T> const init)
  : Field<T, sizeof(T), 3, 1>(init)
{ }

template <typename T>
TVectorImpl<T, 3>::TVectorImpl(Field<T, sizeof(T), 3, 1> const&f)
  : Field<T, sizeof(T), 3, 1>(f)
{}

template <typename T>
TVectorImpl<T, 3>::TVectorImpl(
  TVectorImpl<T, 2> const&v,
  T                 const&z)
  : Field<T, sizeof(T), 3, 1>({ v.x(), v.y(), z })
{}

// Return a copy of the stored x-component. Getter.


DefineImmutableGetter(TVectorImpl, 3, x, 0);
DefineImmutableGetter(TVectorImpl, 3, y, 1);
DefineImmutableGetter(TVectorImpl, 3, z, 2);

// Return a FieldAccessor for the stored x-component to safely assign a new value 
// using the assignment-operator. Setter.

template <typename T>
void
TVectorImpl<T, 3>::x(value_type const& val)
{
  this->m_field[0] = val;
}
// Return a FieldAccessor for the stored y-component to safely assign a new value 
// using the assignment-operator. Setter.

template <typename T>
void
TVectorImpl<T, 3>::y(value_type const& val)
{
  this->m_field[1] = val;
}
// Return a FieldAccessor for the stored z-component to safely assign a new value 
// using the assignment-operator. Setter.

template <typename T>
void
TVectorImpl<T, 3>::z(value_type const& val)
{
  this->m_field[2] = val;
}


template <typename T>
TVectorImpl<T, 3>
TVectorImpl<T, 3>::forward()
{
  return TVectorImpl<T, 3>({ 1, 0, 0 });
}

template <typename T>
TVectorImpl<T, 3>
TVectorImpl<T, 3>::right()
{
  return TVectorImpl<T, 3>({ 0, 1, 0 });
}

template <typename T>
TVectorImpl<T, 3>
TVectorImpl<T, 3>::up()
{
  return TVectorImpl<T, 3>({ 0, 0, 1 });
}

template <typename T>
TVectorImpl<T, 4>::TVectorImpl()
  : Field<T, sizeof(T), 4, 1>({ T(0), T(0), T(0), T(0) })
{ }

template <typename T>
TVectorImpl<T, 4>::TVectorImpl(
  value_type const x,
  value_type const y,
  value_type const z,
  value_type const w)
  : Field<T, sizeof(T), 4, 1>({ x, y, z, w })
{ }

template <typename T>
TVectorImpl<T, 4>::TVectorImpl(class_type const& cpy)
  : Field<T, sizeof(T), 4, 1>(cpy)
{}

template <typename T>
TVectorImpl<T, 4>::TVectorImpl(std::initializer_list<T> const init)
  : Field<T, sizeof(T), 4, 1>(init)
{ }

template <typename T>
TVectorImpl<T, 4>::TVectorImpl(Field<T, sizeof(T), 4, 1> const&f)
  : Field<T, sizeof(T), 4, 1>(f)
{}

template <typename T>
TVectorImpl<T, 4>::TVectorImpl(
  TVectorImpl<T, 2> const&v,
  T                 const&z,
  T                 const&w)
  : Field<T, sizeof(T), 4, 1>({ v.x(), v.y(), z, w })
{}

template <typename T>
TVectorImpl<T, 4>::TVectorImpl(
  TVectorImpl<T, 3> const&v,
  T                 const&w)
  : Field<T, sizeof(T), 4, 1>({ v.x(), v.y(), v.z(), w })
{}

DefineImmutableGetter(TVectorImpl, 4, x, 0);
DefineImmutableGetter(TVectorImpl, 4, y, 1);
DefineImmutableGetter(TVectorImpl, 4, z, 2);
DefineImmutableGetter(TVectorImpl, 4, w, 3);

template <typename T>
void
TVectorImpl<T, 4>::x(value_type const& val)
{
  this->m_field[0] = val;
}

template <typename T>
void
TVectorImpl<T, 4>::y(value_type const& val)
{
  this->m_field[1] = val;
}

template <typename T>
void
TVectorImpl<T, 4>::z(value_type const& val)
{
  this->m_field[2] = val;
}

template <typename T>
void
TVectorImpl<T, 4>::w(value_type const& val)
{
  this->m_field[3] = val;
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
TVector<T, N, TDerived>::TVector()
  : TDerived()
{ }

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
TVector<T, N, TDerived>::TVector(std::initializer_list<T> const init)
  : TDerived(init)
{ }

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
TVector<T, N, TDerived>::TVector(Field<T, sizeof(T), N, 1> const&f)
  : TDerived(f)
{}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
TVector<T, N, TDerived>::TVector(class_type const& cpy)
  : TDerived(cpy)
{}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
bool
TVector<T, N, TDerived>::operator==(TVector<T, N, TDerived> const&other)
{
  return Field<T, sizeof(T), N, 1>::operator==(static_cast<Field<T, sizeof(T), N, 1>>(other));
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
typename TVector<T, N, TDerived>::class_type
TVector<T, N, TDerived>::scale(value_type const factor)
{
  this->operator*=(factor);

  return *this;
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
typename TVector<T, N, TDerived>::value_type
TVector<T, N, TDerived>::length()
{
  return sqrt(this->squared_length());
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
typename TVector<T, N, TDerived>::value_type
TVector<T, N, TDerived>::squared_length()
{
  value_type        len = 0;
  const value_type *ptr = this->const_ptr();

  for(size_t i = 0; i < this->size(); ++i)
    len += (ptr[i] * ptr[i]);

  return len;
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
typename TVector<T, N, TDerived>::value_type
TVector<T, N, TDerived>::abs()
{
  return this->length();
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
typename TVector<T, N, TDerived>::class_type&
TVector<T, N, TDerived>::normalize()
{
  // For now use the "paper-version" of normalization!
  this->operator/=(this->length());

  return *this;
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
TVector<T, N, TDerived> operator +(
  TVector<T, N, TDerived> const& l,
  TVector<T, N, TDerived> const& r)
{
  return
    operator+(
      static_cast<typename TVector<T, N, TDerived>::base_type>(l),
      static_cast<typename TVector<T, N, TDerived>::base_type>(r));
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
TVector<T, N, TDerived> operator -(
  TVector<T, N, TDerived> const&l,
  TVector<T, N, TDerived> const&r)
{
  return
    operator-(
      static_cast<typename TVector<T, N, TDerived>::base_type>(l),
      static_cast<typename TVector<T, N, TDerived>::base_type>(r));
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
TVector<T, N, TDerived> operator *(
  TVector<T, N, TDerived>                      const&l,
  typename TVector<T, N, TDerived>::value_type const&f)
{
  return
    operator*(
      static_cast<typename TVector<T, N, TDerived>::base_type>(l),
      f);
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
TVector<T, N, TDerived> operator *(
  typename TVector<T, N, TDerived>::value_type const&f,
  TVector<T, N, TDerived>                      const&l)
{
  return
    operator*(
      static_cast<typename TVector<T, N, TDerived>::base_type>(l),
      f);
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
TVector<T, N, TDerived> operator /(
  TVector<T, N, TDerived>                      const&l,
  typename TVector<T, N, TDerived>::value_type const&f)
{
  return
    operator/(
      static_cast<typename TVector<T, N, TDerived>::base_type>(l),
      f);
}

template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
TVector<T, N, TDerived> operator /(
  typename TVector<T, N, TDerived>::value_type const&f,
  TVector<T, N, TDerived>                      const&l)
{
  return
    operator/(
      static_cast<typename TVector<T, N, TDerived>::base_type>(l),
      f);
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
    TVector3D<T>({
      (l.y()*r.z() - l.z()*r.y()),
      (l.z()*r.x() - l.x()*r.z()),
      (l.x()*r.y() - l.y()*r.x())
      });
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
  return TVector<T, N>(vec).normalize();
}