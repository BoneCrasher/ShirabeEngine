#ifndef __SHIRABE_SHIRABE_MATH_TVECTOR_H__
#define __SHIRABE_SHIRABE_MATH_TVECTOR_H__

#include "Math/Field.h"

namespace Engine {
  namespace Math {

    #define DefinePermutationAccessor1D(a)          inline TVectorImpl<T, 1> a()          const { return TVector1D<T>(a());                }
    #define DefinePermutationAccessor2D(a, b)       inline TVectorImpl<T, 2> a##b()       const { return TVector2D<T>(a(), b());           }
    #define DefinePermutationAccessor3D(a, b, c)    inline TVectorImpl<T, 3> a##b##c()    const { return TVector3D<T>(a(), b(), c());      }
    #define DefinePermutationAccessor4D(a, b, c, d) inline TVectorImpl<T, 4> a##b##c##d() const { return TVector4D<T>(a(), b(), c(), d()); }

    #define DeclareImmutableGetter(vec_type, vec_size, component) \
    typename vec_type<T, vec_size>::value_type const              \
      component() const;    

    
    template <typename T, std::size_t N>
    class TVectorImpl;

    /**********************************************************************************************//**
     * \class TVector1D
     *
     * \brief A Tvector 1 d.
     *
     * \tparam  T Generic type parameter.
     **************************************************************************************************/
    template <typename T>
    class TVectorImpl<T, 1>
      : public Field<
          std::enable_if_t<std::is_arithmetic_v<T>, T>,
          sizeof(T), 1, 1>
    {
    public:
      static const constexpr std::size_t N = 1;

      typedef Field<T, sizeof(T), N, 1> base_type;
      typedef TVectorImpl<T, N>         class_type;
      typedef T                         value_type;

      TVectorImpl();
      TVectorImpl(value_type const x);
      TVectorImpl(class_type const& cpy);
      TVectorImpl(std::initializer_list<T> const);
      TVectorImpl(Field<T, sizeof(T), 1, 1> const&);

      DeclareImmutableGetter(TVectorImpl, 1, x);

      // Return a FieldAccessor for the stored x-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void x(value_type const& val);

      static TVectorImpl<T, 1> right();
    };

    /**********************************************************************************************//**
     * \class TVector2D
     *
     * \brief A Tvector 2d.
     *
     * \tparam  T Generic type parameter.
     **************************************************************************************************/
    template <typename T>
    class TVectorImpl<T, 2>
      : public Field<
      std::enable_if_t<std::is_arithmetic_v<T>, T>,
      sizeof(T), 2, 1>
    {
    public:
      static const constexpr std::size_t N = 2;

      typedef Field<T, sizeof(T), N, 1> base_type;
      typedef TVectorImpl<T, N>         class_type;
      typedef T                         value_type;

      TVectorImpl();
      TVectorImpl(
        value_type const x,
        value_type const y);
      TVectorImpl(class_type const& cpy);
      TVectorImpl(std::initializer_list<T> const);
      TVectorImpl(Field<T, sizeof(T), 2, 1> const&);

      DeclareImmutableGetter(TVectorImpl, 2, x);
      DeclareImmutableGetter(TVectorImpl, 2, y);

      // Return a FieldAccessor for the stored x-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void x(value_type const& val);
      // Return a FieldAccessor for the stored y-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void y(value_type const& val);

      static TVectorImpl<T, 2> right();
      static TVectorImpl<T, 2> up();

      DefinePermutationAccessor2D(x, y);
      DefinePermutationAccessor2D(y, x);
    };

    template <typename T>
    class TVectorImpl<T, 3>
      : public Field<
      std::enable_if_t<std::is_arithmetic_v<T>, T>,
      sizeof(T), 3, 1>
    {
    public:
      static const constexpr std::size_t N = 3;

      typedef Field<T, sizeof(T), N, 1> base_type;
      typedef TVectorImpl<T, N>         class_type;
      typedef T                         value_type;

      TVectorImpl();
      TVectorImpl(
        value_type const x,
        value_type const y,
        value_type const z);
      TVectorImpl(
        TVectorImpl<T, 2> const&v,
        T                 const&z = T(0));
      TVectorImpl(class_type const& cpy);
      TVectorImpl(std::initializer_list<T> const);
      TVectorImpl(Field<T, sizeof(T), 3, 1> const&);

      DeclareImmutableGetter(TVectorImpl, 3, x);
      DeclareImmutableGetter(TVectorImpl, 3, y);
      DeclareImmutableGetter(TVectorImpl, 3, z);

      // Return a FieldAccessor for the stored x-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void x(value_type const& val);
      // Return a FieldAccessor for the stored y-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void y(value_type const& val);
      // Return a FieldAccessor for the stored z-component to safely assign a new value 
      // using the assignment-operator. Setter.
      void z(value_type const& val);

      static TVectorImpl<T, 3> forward();
      static TVectorImpl<T, 3> right();
      static TVectorImpl<T, 3> up();

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
    class TVectorImpl<T, 4>
      : public Field<
      std::enable_if_t<std::is_arithmetic_v<T>, T>,
      sizeof(T), 4, 1> 
    {
    public:
      static const constexpr std::size_t N = 4;

      typedef Field<T, sizeof(T), N, 1> base_type;
      typedef TVectorImpl<T, N>         class_type;
      typedef T                         value_type;

      TVectorImpl();
      TVectorImpl(
        value_type const x,
        value_type const y,
        value_type const z,
        value_type const w);
      TVectorImpl(class_type const& cpy);
      TVectorImpl(
        TVectorImpl<T, 3> const& other,
        T                 const& w = T(0));
      TVectorImpl(
        TVectorImpl<T, 2> const&other,
        T                 const&z = T(0),
        T                 const&w = T(0));
      TVectorImpl(std::initializer_list<T> const);
      TVectorImpl(Field<T, sizeof(T), 4, 1> const&);

      DeclareImmutableGetter(TVectorImpl, 4, x);
      DeclareImmutableGetter(TVectorImpl, 4, y);
      DeclareImmutableGetter(TVectorImpl, 4, z);
      DeclareImmutableGetter(TVectorImpl, 4, w);

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

      DefinePermutationAccessor2D(x, y);
      DefinePermutationAccessor2D(y, x);
      DefinePermutationAccessor2D(x, z);
      DefinePermutationAccessor2D(z, x);
      DefinePermutationAccessor2D(y, z);
      DefinePermutationAccessor2D(z, y);
      DefinePermutationAccessor2D(x, w);
      DefinePermutationAccessor2D(w, x);
      DefinePermutationAccessor2D(y, w);
      DefinePermutationAccessor2D(w, y);
      DefinePermutationAccessor2D(z, w);
      DefinePermutationAccessor2D(w, z);

      DefinePermutationAccessor3D(x, y, z);
      DefinePermutationAccessor3D(x, z, y);
      DefinePermutationAccessor3D(y, x, z);
      DefinePermutationAccessor3D(y, z, x);
      DefinePermutationAccessor3D(z, x, y);
      DefinePermutationAccessor3D(z, y, x);
      DefinePermutationAccessor3D(x, y, w);
      DefinePermutationAccessor3D(x, w, y);
      DefinePermutationAccessor3D(x, z, w);
      DefinePermutationAccessor3D(x, w, z);
      DefinePermutationAccessor3D(y, x, w);
      DefinePermutationAccessor3D(y, w, x);
      DefinePermutationAccessor3D(y, z, w);
      DefinePermutationAccessor3D(y, w, z);
      DefinePermutationAccessor3D(z, x, w);
      DefinePermutationAccessor3D(z, w, x);
      DefinePermutationAccessor3D(z, y, w);
      DefinePermutationAccessor3D(z, w, y);
      DefinePermutationAccessor3D(w, x, y);
      DefinePermutationAccessor3D(w, y, x);
      DefinePermutationAccessor3D(w, x, z);
      DefinePermutationAccessor3D(w, z, x);
      DefinePermutationAccessor3D(w, y, z);
      DefinePermutationAccessor3D(w, z, y);

      DefinePermutationAccessor4D(x, y, z, w);
      DefinePermutationAccessor4D(x, y, w, z);
      DefinePermutationAccessor4D(x, z, y, w);
      DefinePermutationAccessor4D(x, z, w, y);
      DefinePermutationAccessor4D(x, w, y, z);
      DefinePermutationAccessor4D(x, w, z, y);
      DefinePermutationAccessor4D(y, x, z, w);
      DefinePermutationAccessor4D(y, x, w, z);
      DefinePermutationAccessor4D(y, z, x, w);
      DefinePermutationAccessor4D(y, z, w, x);
      DefinePermutationAccessor4D(y, w, x, z);
      DefinePermutationAccessor4D(y, w, z, x);
      DefinePermutationAccessor4D(z, x, y, w);
      DefinePermutationAccessor4D(z, x, w, y);
      DefinePermutationAccessor4D(z, y, x, w);
      DefinePermutationAccessor4D(z, y, w, x);
      DefinePermutationAccessor4D(z, w, x, y);
      DefinePermutationAccessor4D(z, w, y, x);
      DefinePermutationAccessor4D(w, x, y, z);
      DefinePermutationAccessor4D(w, x, z, y);
      DefinePermutationAccessor4D(w, y, x, z);
      DefinePermutationAccessor4D(w, y, z, x);
      DefinePermutationAccessor4D(w, z, x, y);
      DefinePermutationAccessor4D(w, z, y, x);
    };

    /**********************************************************************************************//**
     * \class TVector
     *
     * \brief A Tvector.
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     **************************************************************************************************/
    template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
    class TVector
      : public TDerived
    {
    public:
      typedef Field<T, sizeof(T), N, 1> base_type;
      typedef TVector<T, N, TDerived>   class_type;
      typedef T                         value_type;

      TVector();
      TVector(std::initializer_list<T> const);
      TVector(Field<T, sizeof(T), N, 1> const&);
      TVector(class_type const& cpy);

      bool operator==(TVector<T, N, TDerived> const&);

    public:
      class_type scale(value_type const factor);

      value_type length();

      value_type squared_length();

      value_type abs();

      class_type& normalize();
    };

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> TVector<T, N> +<typename T,std::operator+( TVector<T, N> const& l, TVector<T, N> const& r);
     *
     * \brief Cast that converts the given TVector&lt;T,N&gt; const&amp; to a size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param l A const to process.
     * \param r A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
    TVector<T, N, TDerived> operator +(
      TVector<T, N, TDerived> const& l,
      TVector<T, N, TDerived> const& r);

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> TVector<T, N> -<typename T,std::operator-( TVector<T, N> const& l, TVector<T, N> const& r);
     *
     * \brief Cast that converts the given TVector&lt;T,N&gt; const&amp; to a size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param l A const to process.
     * \param r A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
    TVector<T, N, TDerived> operator -(
      TVector<T, N, TDerived> const& l,
      TVector<T, N, TDerived> const& r);

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> TVector<T, N> *<typename T,std::operator*( TVector<T, N> const&l, typename TVector<T, N>::value_type const&f);
     *
     * \brief Cast that converts the given TVector&lt;T,N&gt; const&amp; to a size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param l A const to process.
     * \param f A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
    TVector<T, N, TDerived> operator *(
      TVector<T, N, TDerived>                      const&l,
      typename TVector<T, N, TDerived>::value_type const&f);

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> TVector<T, N> *<typename T,std::operator*( typename TVector<T, N>::value_type const&f, TVector<T, N> const&l);
     *
     * \brief Cast that converts the given typename TVector&lt;T,N&gt;::value_type const&amp; to a
     *        size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param f A const to process.
     * \param l A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
    TVector<T, N, TDerived> operator *(
      typename TVector<T, N, TDerived>::value_type const&f,
      TVector<T, N, TDerived>                      const&l);

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> TVector<T, N> /<typename T,std::operator/( TVector<T, N> const&l, typename TVector<T, N>::value_type const&f);
     *
     * \brief Cast that converts the given TVector&lt;T,N&gt; const&amp; to a size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param l A const to process.
     * \param f A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
    TVector<T, N, TDerived> operator /(
      TVector<T, N, TDerived>                      const&l,
      typename TVector<T, N, TDerived>::value_type const&f);

    /**********************************************************************************************//**
     * \fn  template <typename T, std::size_t N> TVector<T, N> /<typename T,std::operator/( typename TVector<T, N>::value_type const&f, TVector<T, N> const&l);
     *
     * \brief Cast that converts the given typename TVector&lt;T,N&gt;::value_type const&amp; to a
     *        size_t N&gt;
     *
     * \tparam  T Generic type parameter.
     * \tparam  N Type of the n.
     * \param f A const to process.
     * \param l A const to process.
     *
     * \return  The result of the operation.
     **************************************************************************************************/
    template <typename T, std::size_t N, typename TDerived = TVectorImpl<T, N>>
    TVector<T, N, TDerived> operator /(
      typename TVector<T, N, TDerived>::value_type const&f,
      TVector<T, N, TDerived>                      const&l);

    template <typename T>
    using TVector1D = TVector<T, 1>;
    template <typename T>
    using TVector2D = TVector<T, 2>;
    template <typename T>
    using TVector3D = TVector<T, 3>;
    template <typename T>
    using TVector4D = TVector<T, 4>;

    // Returns a copy of the 2D dot product of two Vec2 instances.
    template <typename T>
    typename TVector2D<T>::value_type dot(TVector2D<T> const&l, TVector2D<T> const&r);

    // Returns a copy of the 3D dot product of two Vec3 instances.
    template <typename T>
    typename TVector3D<T>::value_type dot(TVector3D<T> const&l, TVector3D<T> const&r);

    // Returns a copy of the 4D dot product of two Vec4 instances.
    template <typename T>
    typename TVector4D<T>::value_type dot(TVector4D<T> const&l, TVector4D<T> const&r);

    // Returns a copy of the 3D cross product of two Vec3 instances.
    template <typename T>
    TVector3D<T> cross(TVector3D<T> const&l, TVector3D<T> const&r);

    // Returns a scaled copy of an arbitrary Vec2 instance.
    template <typename T>
    TVector2D<T> scale(
      TVector2D<T>                      const&vec,
      typename TVector2D<T>::value_type const&factor);

    // Returns a scaled copy of an arbitrary Vec3 instance.
    template <typename T>
    TVector3D<T> scale(
      TVector3D<T>                      const&vec,
      typename TVector3D<T>::value_type const&factor);

    // Returns a scaled copy of an arbitrary Vec4 instance.
    template <typename T>
    TVector4D<T> scale(
      TVector4D<T>                      const&vec,
      typename TVector4D<T>::value_type const&factor);

    template <typename T, size_t N>
    TVector<T, N> normalize(TVector<T, N> const&vec);

    #include "Math/Vector.tpp"

    using Vector1D = TVector1D<float>;
    using Vector2D = TVector2D<float>;
    using Vector3D = TVector3D<float>;
    using Vector4D = TVector4D<float>;

  } // ::Math
} // ::Engine

#endif