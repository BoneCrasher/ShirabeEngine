#ifndef __SHIRABE_SHIRABE_MATH_VECTOR_H__
#define __SHIRABE_SHIRABE_MATH_VECTOR_H__

#include "Math/Field.h"

namespace Engine {
  namespace Math {

    #define DefinePermutationAccessor1D(a)          inline Vector1D a()          const { return Vector1D(a());          }
    #define DefinePermutationAccessor2D(a, b)       inline Vector2D a##b()       const { return Vector2D(a(), b());       }
    #define DefinePermutationAccessor3D(a, b, c)    inline Vector3D a##b##c()    const { return Vector3D(a(), b(), c());    }
    #define DefinePermutationAccessor4D(a, b, c, d) inline Vector4D a##b##c##d() const { return Vector4D(a(), b(), c(), d()); }

    /**********************************************************************************************//**
     * \class Vector
     *
     * \brief A vector.
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     **************************************************************************************************/
    template <typename T, std::size_t N>
    class Vector
      : public Field<
      std::enable_if_t<std::is_arithmetic_v<T>, T>,
      sizeof(float), N, 1>
    {
    public:
      typedef Field<T, sizeof(T), N, 1> base_type;
      typedef Vector<T, N>              class_type;
      typedef T                         value_type;

      Vector();
      Vector(
        value_type const x,
        value_type const y);
      Vector(class_type const& cpy);

    public:
      class_type scale(value_type const factor);

      value_type length();

      value_type squared_length();

      value_type abs();

      class_type& normalize();
    };


    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> Vector<T, N> +<typename T,std::operator+( Vector<T, N> const& l, Vector<T, N> const& r);
     *
     * \brief Cast that converts the given Vector&lt;T,N&gt; const&amp; to a size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param l A const to process.
     * \param r A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N>
    Vector<T, N> operator +(
      Vector<T, N> const& l,
      Vector<T, N> const& r);

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> Vector<T, N> -<typename T,std::operator-( Vector<T, N> const& l, Vector<T, N> const& r);
     *
     * \brief Cast that converts the given Vector&lt;T,N&gt; const&amp; to a size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param l A const to process.
     * \param r A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N>
    Vector<T, N> operator -(
      Vector<T, N> const& l,
      Vector<T, N> const& r);

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> Vector<T, N> *<typename T,std::operator*( Vector<T, N> const&l, typename Vector<T, N>::value_type const&f);
     *
     * \brief Cast that converts the given Vector&lt;T,N&gt; const&amp; to a size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param l A const to process.
     * \param f A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N>
    Vector<T, N> operator *(
      Vector<T, N>                      const&l,
      typename Vector<T, N>::value_type const&f);

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> Vector<T, N> *<typename T,std::operator*( typename Vector<T, N>::value_type const&f, Vector<T, N> const&l);
     *
     * \brief Cast that converts the given typename Vector&lt;T,N&gt;::value_type const&amp; to a
     *        size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param f A const to process.
     * \param l A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N>
    Vector<T, N> operator *(
      typename Vector<T, N>::value_type const&f,
      Vector<T, N>                      const&l);

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> Vector<T, N> /<typename T,std::operator/( Vector<T, N> const&l, typename Vector<T, N>::value_type const&f);
     *
     * \brief Cast that converts the given Vector&lt;T,N&gt; const&amp; to a size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param l A const to process.
     * \param f A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N>
    Vector<T, N> operator /(
      Vector<T, N>                      const&l,
      typename Vector<T, N>::value_type const&f);

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> Vector<T, N> /<typename T,std::operator/( typename Vector<T, N>::value_type const&f, Vector<T, N> const&l);
     *
     * \brief Cast that converts the given typename Vector&lt;T,N&gt;::value_type const&amp; to a
     *        size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param f A const to process.
     * \param l A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N>
    Vector<T, N> operator /(
      typename Vector<T, N>::value_type const&f,
      Vector<T, N>                      const&l);

    /**********************************************************************************************//**
     * \class Vector1D
     *
     * \brief A vector 1 d.
     *
     * \tparam  T Generic type parameter.
     **************************************************************************************************/
    template <typename T>
    class Vector1D
      : public Vector<T, 1>
    {
    public:
      Vector1D();
      Vector1D(value_type const x);
      Vector1D(class_type const& cpy);

      // Return a copy of the stored x-component. Getter.
      value_type const x() const;

      // Return a FieldAccessor for the stored x-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void x(value_type const& val);

      static Vector1D<T> right();
    };

    /**********************************************************************************************//**
     * \class Vector2D
     *
     * \brief A vector 2d.
     *
     * \tparam  T Generic type parameter.
     **************************************************************************************************/
    template <typename T>
    class Vector2D
      : public Vector<T, 2>
    {
    public:
      Vector2D();
      Vector2D(
        value_type const x,
        value_type const y);
      Vector2D(class_type const& cpy);

      // Return a copy of the stored x-component. Getter.
      value_type const x() const;

      // Return a copy of the stored y-component. Getter.
      value_type const y() const;

      // Return a FieldAccessor for the stored x-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void x(value_type const& val);
      // Return a FieldAccessor for the stored y-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void y(value_type const& val);

      static Vector2D<T> right();
      static Vector2D<T> up();

      DefinePermutationAccessor2D(x, y);
      DefinePermutationAccessor2D(y, x);
    };


    template <typename T>
    class Vector3D
      : public Vector<T, 3>
    {
    public:
      Vector3D();
      Vector3D(
        value_type const x,
        value_type const y,
        value_type const z);
      Vector3D(
        Vector2D<T> const&v,
        T           const&z = T(0));
      Vector3D(class_type const& cpy);

      // Return a copy of the stored x-component. Getter.
      value_type const x() const;
      // Return a copy of the stored y-component. Getter.
      value_type const y() const;
      // Return a copy of the stored z-component. Getter.
      value_type const z() const;

      // Return a FieldAccessor for the stored x-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void x(value_type const& val);
      // Return a FieldAccessor for the stored y-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void y(value_type const& val);
      // Return a FieldAccessor for the stored z-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void z(value_type const& val);

      static Vector3D<T> forward();
      static Vector3D<T> right();
      static Vector3D<T> up();

      DefinePermutationAccessor2D(x, y);
      DefinePermutationAccessor2D(y, x);
      DefinePermutationAccessor2D(x, z);
      DefinePermutationAccessor2D(z, x);
      DefinePermutationAccessor2D(y, z);
      DefinePermutationAccessor2D(z, y);

      DefinePermutationAccessor3D(x, y, z);
      DefinePermutationAccessor3D(x, z, y);
      DefinePermutationAccessor3D(y, x, z);
      DefinePermutationAccessor3D(y, z, x);
      DefinePermutationAccessor3D(z, x, y);
      DefinePermutationAccessor3D(z, y, x);
    };

    template <typename T>
    class Vector4D
      : public Vector<T, 4> {
    public:
      Vector4D();
      Vector4D(
        value_type const x,
        value_type const y,
        value_type const z,
        value_type const w);
      Vector4D(class_type const& cpy);
      Vector4D(
        Vector3D<T> const& other,
        T           const& w = T(0));
      Vector4D(
        Vector2D<T> const&other,
        T           const&z = T(0),
        T           const&w = T(0));
      
      // Return a copy of the stored x-component. Getter.
      value_type const x() const;
      // Return a copy of the stored y-component. Getter.
      value_type const y() const;
      // Return a copy of the stored z-component. Getter.
      value_type const z() const;
      // Return a copy of the stored z-component. Getter.
      value_type const w() const;

      // Return a FieldAccessor for the stored x-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void x(value_type const& val);
      // Return a FieldAccessor for the stored y-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void y(value_type const& val);
      // Return a FieldAccessor for the stored z-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void z(value_type const& val);
      // Return a FieldAccessor for the stored z-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void w(value_type const& val);
    };

    // Returns a copy of the 2D dot product of two Vec2 instances.
    template <typename T>
    Vector2D<T>::value_type dot(const Vector2D l, const Vector2D r)
    {
      return ((l.x() * r.x()) + (l.y() * r.y()));
    }

    // Returns a copy of the 3D dot product of two Vec3 instances.
    template <typename T>
    Vector3D<T>::value_type dot(const Vector3D l, const Vector3D r)
    {
      return ((l.x() * r.x())
        + (l.y() * r.y())
        + (l.z() * r.z()));
    }

    // Returns a copy of the 4D dot product of two Vec4 instances.
    template <typename T>
    Vector4D<T>::value_type dot(const Vector4D l, const Vector4D r)
    {
      return ((l.x() * r.x()) +
        (l.y() * r.y()) +
        (l.z() * r.z()) +
        (l.w() + r.w()));
    }

    // Returns a copy of the 3D cross product of two Vec3 instances.
    template <typename T>
    inline Vector3D cross(const Vector3D l, const Vector3D r)
    {
      return Vector3D((l.y()*r.z() - l.z()*r.y()),
        (l.z()*r.x() - l.x()*r.z()),
        (l.x()*r.y() - l.y()*r.x()));
    }

    // Returns a scaled copy of an arbitrary Vec2 instance.
    template <typename T>
    inline Vector2D scale(const Vector2D&  vec, const Vector2D::value_type factor)
    {
      return Vector2D(vec).scale(factor);
    }

    // Returns a scaled copy of an arbitrary Vec3 instance.
    template <typename T>
    inline Vector3D scale(const Vector3D&  vec, const Vector3D::value_type factor)
    {
      return Vector3D(vec).scale(factor);
    }

    // Returns a scaled copy of an arbitrary Vec4 instance.
    template <typename T>
    inline Vector4D scale(const Vector4D&  vec, const Vector4D::value_type factor)
    {
      return Vector4D(vec).scale(factor);
    }

    template <typename T, size_t N>
    inline Vector<N> normalize(const Vector<N>& vec)
    {
      return *(Vector<N>(vec).normalize());
    }

    #include "Math/Vector.tpp"

  } // ::Math
} // ::Engine

#endif