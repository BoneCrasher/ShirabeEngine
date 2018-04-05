template <typename T, std::size_t N>
Vector<T, N>::Vector()
  : Field<T, sizeof(T), N, 1>
{ }

template <typename T, std::size_t N>
Vector<T, N>::Vector(
  value_type const x,
  value_type const y)
  : Field<T, sizeof(T), N, 1>()
{
}

template <typename T, std::size_t N>
Vector<T, N>::Vector(class_type const& cpy)
  : Field<T, sizeof(T), N, 1>()
{}

template <typename T, std::size_t N>
typename Vector<T, N>::class_type
Vector<T, N>::scale(value_type const factor)
{
  this->operator*=(factor);

  return *this;
}

template <typename T, std::size_t N>
typename Vector<T, N>::value_type
Vector<T, N>::length()
{
  return sqrt(this->squared_length());
}

template <typename T, std::size_t N>
typename Vector<T, N>::value_type
Vector<T, N>::squared_length()
{
  value_type        len = 0;
  const value_type *ptr = this->const_ptr();

  for(size_t i = 0; i < this->size(); ++i)
    len += (ptr[i] * ptr[i]);

  return len;
}

template <typename T, std::size_t N>
typename Vector<T, N>::value_type
Vector<T, N>::abs()
{
  return this->length();
}

template <typename T, std::size_t N>
typename Vector<T, N>::class_type&
Vector<T, N>::normalize()
{
  // For now use the "paper-version" of normalization!
  this->operator/=(this->length());

  return *this;
}

template <typename T, std::size_t N>
Vector<T, N> operator +(
  Vector<T, N> const& l,
  Vector<T, N> const& r)
{
  return
    operator+(
      static_cast<typename Vector<T, N>::base_type>(l),
      static_cast<typename Vector<T, N>::base_type>(r));
}

template <typename T, std::size_t N>
Vector<T, N> operator -(
  Vector<T, N> const&l,
  Vector<T, N> const&r)
{
  return
    operator-(
      static_cast<typename Vector<T, N>::base_type>(l),
      static_cast<typename Vector<T, N>::base_type>(r));
}

template <typename T, std::size_t N>
Vector<T, N> operator *(
  Vector<T, N>                      const&l,
  typename Vector<T, N>::value_type const&f)
{
  return
    operator*(
      static_cast<typename Vector<T, N>::base_type>(l),
      f);
}

template <typename T, std::size_t N>
Vector<T, N> operator *(
  typename Vector<T, N>::value_type const&f,
  Vector<T, N>                      const&l)
{
  return
    operator*(
      static_cast<typename Vector<T, N>::base_type>(l),
      f);
}

template <typename T, std::size_t N>
Vector<T, N> operator /(
  Vector<T, N>                      const&l,
  typename Vector<T, N>::value_type const&f)
{
  return
    operator/(
      static_cast<typename Vector<T, N>::base_type>(l),
      f);
}

template <typename T, std::size_t N>
Vector<T, N> operator /(
  typename Vector<T, N>::value_type const&f,
  Vector<T, N>                      const&l)
{
  return
    operator/(
      static_cast<typename Vector<T, N>::base_type>(l),
      f);
}

template <typename T>
Vector1D<T>::Vector1D()
  : Vector<T, 1>()
{
  value_type *ptr = this->ptr();
  ptr[0] = 0.0f;
}

template <typename T>
Vector1D<T>::Vector1D(
  value_type const x)
  : Vector<T, 1>({ x })
{ }

template <typename T>
Vector1D<T>::Vector1D(class_type const& cpy)
  : Vector<T, 1>(cpy)
{}

template <typename T>
typename Vector1D<T>::value_type const
Vector1D<T>::x() const
{
  return this->m_field[0];
}

template <typename T>
void
Vector1D<T>::x(value_type const& val)
{
  this->m_field[0] = val;
}

template <typename T>
Vector1D<T>
Vector1D<T>::right() { return Vector1D<T>({ static_cast<T>(1) }); }

template <typename T>
Vector2D()
  : Vector<T, 2>()
{
  value_type *ptr = this->ptr();
  ptr[0] = 0.0f;
  ptr[1] = 0.0f;
}

template <typename T>
Vector2D(
  value_type const x,
  value_type const y)
  : Vector<T, 2>({ x, y })
{ }

template <typename T>
Vector2D<T>::Vector2D(class_type const& cpy)
  : Vector<T, 2>(cpy)
{}

template <typename T>
typename Vector2D<T>::value_type const
Vector2D<T>::x() const
{
  return this->m_field[0];
}

template <typename T>
typename Vector2D<T>::value_type const
Vector2D<T>::y() const
{
  return this->m_field[1];
}

template <typename T>
void
Vector2D<T>::x(value_type const& val)
{
  this->m_field[0] = val;
}

template <typename T>
void
Vector2D<T>::y(value_type const& val)
{
  this->m_field[1] = val;
}

template <typename T>
Vector2D<T>
Vector2D<T>::right()
{
  return Vector2D<T>({ 1, 0 });
}

template <typename T>
Vector2D<T>
Vector2D<T>::up()
{
  return Vector2D<T>({ 0, 1 });
}

template <typename T>
Vector3D<T>::Vector3D()
  : Vector<3>({ 0.0f, 0.0f, 0.0f })
{
  value_type *ptr = this->ptr();
  ptr[2] = 0.0f;
}

template <typename T>
Vector3D<T>::Vector3D(
  value_type const x,
  value_type const y,
  value_type const z)
  : Vector<T, 3>({ x, y, z })
{
}

template <typename T>
Vector3D<T>::Vector3D(
  Vector2D<T> const&v,
  T           const&z)
  : Vector<T, 3>(v.x(), v.y(), z)
{}

template <typename T>
Vector3D<T>::Vector3D(class_type const& cpy)
  : Vector<T, 3>(cpy)
{}

// Return a copy of the stored x-component. Getter.

template <typename T>
typename Vector3D<T>::value_type const
Vector3D<T>::x() const
{
  return this->m_field[0];
}
// Return a copy of the stored y-component. Getter.

template <typename T>
typename Vector3D<T>::value_type const
Vector3D<T>::y() const
{
  return this->m_field[1];
}
// Return a copy of the stored z-component. Getter.

template <typename T>
typename Vector3D<T>::value_type const
Vector3D<T>::z() const
{
  return this->m_field[2];
}

// Return a FieldAccessor for the stored x-component to safely assign a new value 
// using the assignment-operator. Setter.

template <typename T>
void
Vector3D<T>::x(value_type const& val)
{
  this->m_field[0] = val;
}
// Return a FieldAccessor for the stored y-component to safely assign a new value 
// using the assignment-operator. Setter.

template <typename T>
void
Vector3D<T>::y(value_type const& val)
{
  this->m_field[1] = val;
}
// Return a FieldAccessor for the stored z-component to safely assign a new value 
// using the assignment-operator. Setter.

template <typename T>
void
Vector3D<T>::z(value_type const& val)
{
  this->m_field[2] = val;
}


template <typename T>
Vector3D<T>
Vector3D<T>::forward()
{
  return Vector3D({ 1, 0, 0 });
}

template <typename T>
Vector3D<T>
Vector3D<T>::right()
{
  return Vector3D({ 0, 1, 0 });
}

template <typename T>
Vector3D<T>
Vector3D<T>::up()
{
  return Vector3D({ 0, 0, 1 });
}

template <typename T>
Vector4D<T>::Vector4D()
  : Vector<T, 4>({ 0.0f, 0.0f, 0.0f, 0.0f })
{
}

template <typename T>
Vector4D<T>::Vector4D(
  value_type const x,
  value_type const y,
  value_type const z,
  value_type const w)
  : Vector<T, 4>({ x, y, z, w })
{
}

template <typename T>
Vector4D<T>::Vector4D(class_type const& cpy)
  : Vector<T, 4>(cpy)
{}

template <typename T>
Vector4D<T>::Vector4D(
  Vector3D<T> const& other,
  T           const& w)
  : Vector<T, 4>({ other.x(), other.y(), other.z(), w })
{}

template <typename T>
Vector4D<T>::Vector4D(
  Vector2D<T> const&other,
  T           const&z,
  T           const&w)
  : Vector<T, 4>({ other.x(), other.y(), z, w })
{}

template <typename T>
typename Vector4D<T>::value_type const
Vector4D<T>::x() const
{
  return this->m_field[0];
}

template <typename T>
typename Vector4D<T>::value_type const
Vector4D<T>::y() const
{
  return this->m_field[1];
}

template <typename T>
typename Vector4D<T>::value_type const
Vector4D<T>::z() const
{
  return this->m_field[2];
}

template <typename T>
typename Vector4D<T>::value_type const
Vector4D<T>::w() const
{
  return this->m_field[3];
}

template <typename T>
void
Vector4D<T>::x(value_type const& val)
{
  this->m_field[0] = val;
}

template <typename T>
void
Vector4D<T>::y(value_type const& val)
{
  this->m_field[1] = val;
}

template <typename T>
void
Vector4D<T>::z(value_type const& val)
{
  this->m_field[2] = val;
}

template <typename T>
void
Vector4D<T>::w(value_type const& val)
{
  this->m_field[3] = val;
}