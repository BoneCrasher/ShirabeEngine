#ifndef __SHIRABE_SHIRABE_MATH_VECTOR_H__
#define __SHIRABE_SHIRABE_MATH_VECTOR_H__

#include <cstddef>
#include <type_traits>
#include <initializer_list>

#include "math/field.h"

namespace Engine
{
    namespace Math
    {
        #define DefinePermutationAccessor1D(a)                                         \
            inline CVectorImpl<T, 1> a() const                                         \
            {                                                                          \
                return CVectorImpl<T, 1>(a());                                         \
            }

        #define DefinePermutationAccessor2D(a, b)                                      \
            inline CVectorImpl<T, 2> a##b() const                                      \
            {                                                                          \
                return CVectorImpl<T, 2>(a(), b());                                    \
            }
        #define DefinePermutationAccessor3D(a, b, c)                                   \
            inline CVectorImpl<T, 3> a##b##c() const                                   \
            {                                                                          \
                return CVectorImpl<T, 3>(a(), b(), c());                               \
            }
        #define DefinePermutationAccessor4D(a, b, c, d)                                \
            inline CVectorImpl<T, 4> a##b##c##d() const                                \
            {                                                                          \
                return CVectorImpl<T, 4>(a(), b(), c(), d());                          \
            }
        
        #define DeclareImmutableGetter(vec_type, vec_size, component)                  \
            typename vec_type<T, vec_size>::value_type const component() const;
        
        #define DefineImmutableGetter(vec_type, vec_size, component, index)            \
            template <typename T>                                                      \
            typename vec_type<T, vec_size>::value_type const                           \
            vec_type<T, vec_size>::component() const                                   \
            {                                                                          \
                return this->mField[index];                                            \
            }
        
        /**
         *
         */
        template <typename T, std::size_t N>
            class CVectorImpl;
        
        /**
         * The specialization CVectorImpl<T, 1> provides the vector implementation for 1D
         * vectors, so basically a regular number.
         *
         * @tparam T The underlying value type of the vector.
         */
        template <typename T>
        class CVectorImpl<T, 1>
            : public CField<std::enable_if_t<std::is_arithmetic_v<T>, T>, sizeof(T), 1, 1>
        {
        public_static_constants:
            static const constexpr std::size_t N = 1;
            
        public_typedefs:
            typedef CField<T, sizeof(T), N, 1> base_type;
            typedef CVectorImpl<T, N>          class_type;
            typedef T                          value_type;
            
        public_constructors:
            /**
             * Default construct a 1D vector.
             */
            CVectorImpl( );
            /**
             * Copy-Initialize this 1D vector with 'aOther'.
             *
             * @param aOther
             */
            CVectorImpl(class_type const&aOther);

            /**
             * Copy-Initialize this 1D vector with a 1D field 'aOther'.
             *
             * @param aOther
             */
            CVectorImpl(base_type const&aOther);

            /**
             * Initialize this 1D vector with a list of values.
             *
             * @param aInitializer
             */
            CVectorImpl(std::initializer_list<T> const aInitializer);

            /**
             * Initialize this 1D vector with a single value.
             *
             * @param aX
             */
            CVectorImpl(value_type const aX);
            
        public_methods:
            DeclareImmutableGetter(CVectorImpl, 1, x);
            
            /**
             * Set the value of the x-component to 'aValue'.
             *
             * @param aValue
             */
            void x(value_type const&aValue);

            /**
             * Return a 1D axis vector pointing to the right along the x-axis.
             * @return
             */
            static CVectorImpl<T, 1> right();
        };
                
        /**
         * The specialization CVectorImpl<T, 2> provides the vector implementation for two
         * dimension vectors, e.g. a 2D Points, Directions, Ranges, ...
         *
         * @tparam T The underlying value type of the vector.
         */
        template <typename T>
        class CVectorImpl<T, 2>
            : public CField<std::enable_if_t<std::is_arithmetic_v<T>, T>, sizeof(T), 2, 1>
        {
        public_static_constants:
            static const constexpr std::size_t N = 2;

        public_typedefs:
            typedef CField<T, sizeof(T), N, 1> base_type;
            typedef CVectorImpl<T, N>          class_type;
            typedef T                          value_type;

        public_constructors:
            /**
             * Default construct a 2D vector.
             */
            CVectorImpl( );
            /**
             * Copy-Initialize this 2D vector with 'aOther'.
             *
             * @param aOther
             */
            CVectorImpl(class_type const&aOther);

            /**
             * Copy-Initialize this 2D vector with a 2D field 'aOther'.
             *
             * @param aOther
             */
            CVectorImpl(base_type const&aOther);

            /**
             * Initialize this 2D vector with a list of values.
             *
             * @param aInitializer
             */
            CVectorImpl(std::initializer_list<T> const aInitializer);

            /**
             * Construct a 2D vector with a x- and y-value.
             *
             * @param aX X-component value
             * @param aY Y-component value
             */
            CVectorImpl(
                value_type const aX,
                value_type const aY
            );

        public_methods:
            DeclareImmutableGetter(CVectorImpl, 2, x);
            DeclareImmutableGetter(CVectorImpl, 2, y);

            /**
             * Set the value of the x-component to 'aValue'.
             *
             * @param aValue
             */
            void x(value_type const &aValue);

            /**
             * Set the value of the y-component to 'aValue'.
             *
             * @param aValue
             */
            void y(value_type const &aValue);

            /**
             * Return a 2D vector pointing along and resembling the local x-axis.
             *
             * @return
             */
            static CVectorImpl<T, 2> right( );

            /**
             * Return a 2D vector pointing along and resembling the local y-axis.
             *
             * @return
             */
            static CVectorImpl<T, 2> up( );

            DefinePermutationAccessor2D(x, y);
            DefinePermutationAccessor2D(y, x);
        };

        /**
         * The specialization CVectorImpl<T, 3> provides the vector implementation for three
         * dimension vectors, e.g. a 3D-Points or directions or axis vectors.
         *
         * @tparam T The underlying value type of the vector.
         */
        template <typename T>
        class CVectorImpl<T, 3>
            : public CField<std::enable_if_t<std::is_arithmetic_v<T>, T>, sizeof(T), 3, 1>
        {
        public_static_constants:
            static const constexpr std::size_t N = 3;

        public_typedefs:
            typedef CField<T, sizeof(T), N, 1> base_type;
            typedef CVectorImpl<T, N>          class_type;
            typedef T                          value_type;

        public_constructors:
            /**
             * Default construct a 3D vector.
             */
            CVectorImpl( );
            /**
             * Copy-Initialize this 3D vector with 'aOther'.
             *
             * @param aOther
             */
            CVectorImpl(class_type const&aOther);

            /**
             * Copy-Initialize this 3D vector with a 3D field 'aOther'.
             *
             * @param aOther
             */
            CVectorImpl(base_type const&aOther);

            /**
             * Initialize this 3D vector with a list of values.
             *
             * @param aInitializer
             */
            CVectorImpl(std::initializer_list<T> const aInitializer);

            /**
             * Construct this 3D vector from the values.
             *
             * @param aX X-component value
             * @param aY Y-component value
             * @param aZ Z-component value
             */
            CVectorImpl(
                value_type const aX,
                value_type const aY,
                value_type const aZ);

            /**
             * Construct this 3D vector from a 2D vector and a z-value.
             *
             * @param aVector2D
             * @param aZ
             */
            CVectorImpl(
                CVectorImpl<T, 2> const&aVector2D,
                T                 const&aZ         = T(0)
            );

        public_methods:
            DeclareImmutableGetter(CVectorImpl, 3, x);
            DeclareImmutableGetter(CVectorImpl, 3, y);
            DeclareImmutableGetter(CVectorImpl, 3, z);

            /**
             * Set the value of the x-component to 'aValue'.
             *
             * @param aValue
             */
            void x(value_type const&aValue);

            /**
             * Set the value of the y-component to 'aValue'.
             *
             * @param aValue
             */
            void y(value_type const&aValue);

            /**
             * Set the value of the z-component to 'aValue'.
             *
             * @param aValue
             */
            void z(value_type const&aValue);

            /**
             * Return a 3D vector pointing along and resembling the local x-axis.
             *
             * @return
             */
            static CVectorImpl<T, 3> forward( );

            /**
             * Return a 3D vector pointing along and resembling the local y-axis.
             *
             * @return
             */
            static CVectorImpl<T, 3> right( );

            /**
             * Return a 3D vector pointing along and resembling the local z-axis.
             *
             * @return
             */
            static CVectorImpl<T, 3> up( );

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

        /**
         * The specialization CVectorImpl<T, 4> provides the vector implementation for four
         * dimension vectors, e.g. a 4D Point, Direction, Quaternions ...
         *
         * @tparam T The underlying value type of the vector.
         */
        template <typename T>
        class CVectorImpl<T, 4>
            : public CField<std::enable_if_t<std::is_arithmetic_v<T>, T>, sizeof(T), 4, 1>
        {
        public_static_constants:
            static const constexpr std::size_t N = 4;

        public_typedefs:
            typedef CField<T, sizeof(T), N, 1> base_type;
            typedef CVectorImpl<T, N>          class_type;
            typedef T                          value_type;

        public_constructors:
            /**
             * Default construct a 4D vector.
             */
            CVectorImpl( );
            /**
             * Copy-Initialize this 4D vector with 'aOther'.
             *
             * @param aOther
             */
            CVectorImpl(class_type const&aOther);

            /**
             * Copy-Initialize this 4D vector with a 4D field 'aOther'.
             *
             * @param aOther
             */
            CVectorImpl(base_type const&aOther);

            /**
             * Initialize this 4D vector with a list of values.
             *
             * @param aInitializer
             */
            CVectorImpl(std::initializer_list<T> const aInitializer);

            /**
             * Construct this 4D vector from four values.
             *
             * @param aX
             * @param aY
             * @param aZ
             * @param aW
             */
            CVectorImpl(
                value_type const &aX,
                value_type const &aY,
                value_type const &aZ,
                value_type const &aW
            );

            /**
             * Construct this 4D vector from a 3D-vector and a w-value.
             *
             * @param aVector3D
             * @param aW
             */
            CVectorImpl(
                CVectorImpl<T, 3> const &aVector3D,
                T                 const &aW         = T(0)
            );

            /**
             * Construct this 4D vector from a 2D vector and a z- and w-value.
             *
             * @param aVector2D
             * @param aZ
             * @param aW
             */
            CVectorImpl(
                CVectorImpl<T, 2> const &aVector2D,
                T                 const &aZ         = T(0),
                T                 const &aW         = T(0)
            );

        public_methods:
            DeclareImmutableGetter(CVectorImpl, 4, x);
            DeclareImmutableGetter(CVectorImpl, 4, y);
            DeclareImmutableGetter(CVectorImpl, 4, z);
            DeclareImmutableGetter(CVectorImpl, 4, w);

            /**
             * Set the value of the x-component to 'aValue'.
             *
             * @param aValue
             */
            void x(value_type const&aValue);

            /**
             * Set the value of the y-component to 'aValue'.
             *
             * @param aValue
             */
            void y(value_type const&aValue);

            /**
             * Set the value of the z-component to 'aValue'.
             *
             * @param aValue
             */
            void z(value_type const&aValue);

            /**
             * Set the value of the z-component to 'aValue'.
             *
             * @param aValue
             */
            void w(value_type const&aValue);

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

        /**
         * The CVector-class is the public interface of any kind of vector and accepts a
         * TDerived-implementation class to derive from, to avoid redundancy with dimension
         * independent functionality common in any kind of vector.
         *
         * @tparam T        Defines the underlying data type of the vector.
         * @tparam N        Defines the number of elements in the vector.
         * @tparam TDerived Defines the vector implementation class to derive from, which
         *                  is required to store type T and must have N elements.
         */
        template <
            typename    T,
            std::size_t N,
            typename    TDerived = CVectorImpl<T, N>
        >
        class CVector
            : public TDerived
        {
        public_typedefs:
            typedef CField<T, sizeof(T), N, 1> base_type;
            typedef CVector<T, N, TDerived>    class_type;
            typedef T                          value_type;

        public_constructors:
            /**
             * Construct this vector as default.
             */
            CVector( );

            /**
             * Construct this vector from a list of values.
             */
            CVector(std::initializer_list<T> const);

            /**
             * Copy-Initialize this vector from a field of T with N elements.
             *
             * @param aOther The field to copy from.
             */
            CVector(base_type const&aOther);

            /**
             * Copy-Initialize this vector from another equally sized and typed vector.
             *
             * @param aOther The vector to copy from.
             */
            CVector(class_type const&aOther);

        public_operators:
            /**
             * Compare this and 'aOther' against equality.
             *
             * @param aOther The vector to compare with.
             * @return       True, if bitwise equal. False otherwise.
             */
            bool operator==(class_type const&aOther);

        public_methods:
            /**
             * Apply the scalar product by factor 'aFactor'.
             *
             * @param aFactor The scale-factor to apply.
             * @return        Returns this vector scaled by 'aFactor'.
             */
            class_type& scale(value_type const aFactor);

            /**
             * Return the length of this vector by calculating square-root of self-dot-product.
             *
             * @return The length of this vector.
             */
            value_type length() const;

            /**
             * Like length(), but without calculating the square-root.
             *
             * @return The squared length of this vector.
             */
            value_type squared_length() const;

            /**
             * Return the absolute of this vector, which is identical to length().
             *
             * @return Returns the absolute of this vector.
             */
            value_type abs() const;

            /**
             * Return this vector w/ length 1 by multiplying by the inverse of it's length.
             *
             * @return Returns this vector normalized.
             */
            class_type& normalize( );
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        CVector<T, N, TDerived>::CVector( )
            : TDerived()
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        CVector<T, N, TDerived>::CVector(std::initializer_list<T> const aInitializer)
            : TDerived(aInitializer)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        CVector<T, N, TDerived>::CVector(
                CField<T, sizeof(T), N, 1> const&aField)
            : TDerived(aField)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        CVector<T, N, TDerived>::CVector(class_type const&aOther)
            : TDerived(aOther)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        bool CVector<T, N, TDerived>::operator==(CVector<T, N, TDerived> const&aOther)
        {
            CField<T, sizeof(T), N, 1> const &field = static_cast<CField<T, sizeof(T), N, 1>>(aOther);
            return CField<T, sizeof(T), N, 1>::operator==(field);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        typename CVector<T, N, TDerived>::class_type
        &CVector<T, N, TDerived>::scale(value_type const aFactor)
        {
            this->operator*=(aFactor);

            return *this;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        typename CVector<T, N, TDerived>::value_type
        CVector<T, N, TDerived>::length() const
        {
            return sqrt(this->squared_length());
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        typename CVector<T, N, TDerived>::value_type
        CVector<T, N, TDerived>::squared_length() const
        {
            value_type        len  = 0;
            value_type const *ptr  = this->const_ptr();

            for(size_t i = 0; i < this->size(); ++i)
            {
                len += ( ptr[i] * ptr[i] );
            }

            return len;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        typename CVector<T, N, TDerived>::value_type
        CVector<T, N, TDerived>::abs() const
        {
            return this->length();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        typename CVector<T, N, TDerived>::class_type&
        CVector<T, N, TDerived>::normalize()
        {
            // For now use the "paper-version" of normalization!
            this->operator/=(this->length());

            return *this;
        }

        /**
         *  Sum up two vectors of equal length and type.
         *
         * @tparam T        Defines the underlying data type of the vector.
         * @tparam N        Defines the number of elements in the vector.
         * @tparam TDerived Defines the vector implementation class to derive from, which
         *                  is required to store type T and must have N elements.
         * @param  aLHS     The left summand.
         * @param  aRHS     The right summand.
         * @return          Returns a new vector initialized with the result of the operation.
         */
        template <
            typename    T,
            std::size_t N,
            typename    TDerived = CVectorImpl<T, N>
        >
        CVector<T, N, TDerived> operator+(CVector<T, N, TDerived> const &aLHS,
                                          CVector<T, N, TDerived> const &aRHS)
        {
            using BaseType_t = typename CVector<T, N, TDerived>::base_type;

            BaseType_t const lhs = static_cast<BaseType_t>(aLHS);
            BaseType_t const rhs = static_cast<BaseType_t>(aRHS);

            BaseType_t vec = operator+<T, sizeof(T), N, 1>(lhs, rhs);

            return CVector<T, N, TDerived>(vec);
        }

        /**
         * Subtract one vector from another.
         * Both vectors must be of equal type and length.
         *
         * @tparam T        Defines the underlying data type of the vector.
         * @tparam N        Defines the number of elements in the vector.
         * @tparam TDerived Defines the vector implementation class to derive from, which
         *                  is required to store type T and must have N elements.
         * @param  aLHS     The minuend.
         * @param  aRHS     The subtrahend.
         * @return          Returns a new vector initialized with the result of the operation.
         */
        template <
            typename    T,
            std::size_t N,
            typename    TDerived = CVectorImpl<T, N>
        >
        CVector<T, N, TDerived> operator-(CVector<T, N, TDerived> const &aLHS,
                                          CVector<T, N, TDerived> const &aRHS)
        {
            using BaseType_t = typename CVector<T, N, TDerived>::base_type;

            BaseType_t lhs = static_cast<BaseType_t>(aLHS);
            BaseType_t rhs = static_cast<BaseType_t>(aRHS);

            BaseType_t vec = operator-<T, sizeof(T), N, 1>(lhs, rhs);

            return CVector<T, N, TDerived>(vec);
        }

        /**
         * Multiply a vector with 'aFactor'.
         *
         * @tparam T        Defines the underlying data type of the vector.
         * @tparam N        Defines the number of elements in the vector.
         * @tparam TDerived Defines the vector implementation class to derive from, which
         *                  is required to store type T and must have N elements.
         * @param  aLHS     The vector to be scaled.
         * @param  aFactor  The factor to multiply with.
         * @return          Returns a new vector initialized with the result of the operation.
         */
        template <
            typename    T,
            std::size_t N,
            typename    TDerived = CVectorImpl<T, N>
        >
        CVector<T, N, TDerived>
        operator*(CVector<T, N, TDerived>                      const &aLHS,
                  typename CVector<T, N, TDerived>::value_type const &aFactor)
        {            
            using BaseType_t = typename CVector<T, N, TDerived>::base_type;

            BaseType_t lhs = static_cast<BaseType_t>(aLHS);
            BaseType_t vec = operator*<T, sizeof(T), N, 1>(lhs, aFactor);

            return CVector<T, N, TDerived>(vec);
        }

        /**
         * Multiply a vector with 'aFactor'.
         *
         * @tparam T        Defines the underlying data type of the vector.
         * @tparam N        Defines the number of elements in the vector.
         * @tparam TDerived Defines the vector implementation class to derive from, which
         *                  is required to store type T and must have N elements.
         * @param  aFactor  The factor to multiply with.
         * @param  aLHS     The vector to be scaled.
         * @return          Returns a new vector initialized with the result of the operation.
         */
        template <
            typename    T,
            std::size_t N,
            typename    TDerived = CVectorImpl<T, N>
        >
        CVector<T, N, TDerived>
        operator*(typename CVector<T, N, TDerived>::value_type const&aFactor,
                  CVector<T, N, TDerived>                      const&aLHS)
        {
            return operator*(aLHS, aFactor);
        }

        /**
         * Divide a vector by 'aFactor'.
         *
         * @tparam T        Defines the underlying data type of the vector.
         * @tparam N        Defines the number of elements in the vector.
         * @tparam TDerived Defines the vector implementation class to derive from, which
         *                  is required to store type T and must have N elements.
         * @param  aLHS     The dividend vector.
         * @param  aFactor  The divisor factor.
         * @return          Returns a new vector initialized with the result of the operation.
         */
        template <
            typename    T,
            std::size_t N,
            typename    TDerived = CVectorImpl<T, N>
        >
        CVector<T, N, TDerived>
        operator/(CVector<T, N, TDerived>                      const &aLHS,
                  typename CVector<T, N, TDerived>::value_type const &aFactor)
        {
            using BaseType_t = typename CVector<T, N, TDerived>::base_type;

            BaseType_t lhs = static_cast<BaseType_t>(aLHS);
            BaseType_t vec = operator/<T, sizeof(T), N, 1>(lhs, aFactor);

            return CVector<T, N, TDerived>(vec);
        }

        /**
         * Divide a vector by 'aFactor'.
         *
         * @tparam T        Defines the underlying data type of the vector.
         * @tparam N        Defines the number of elements in the vector.
         * @tparam TDerived Defines the vector implementation class to derive from, which
         *                  is required to store type T and must have N elements.
         * @param  aFactor  The divisor factor.
         * @param  aLHS     The dividend vector.
         * @return          Returns a new vector initialized with the result of the operation.
         */
        template <
            typename    T,
            std::size_t N,
            typename    TDerived = CVectorImpl<T, N>
        >
        CVector<T, N, TDerived>
        operator/(typename CVector<T, N, TDerived>::value_type const&aFactor,
                  CVector<T, N, TDerived>                      const&aRHS)
        {
            return operator/(aRHS, aFactor);
        }
        
        /**
         * Convenience type-alias to explicitly select 1D vectors.
         */
        template <typename T>
            using CVector1D = CVector<T, 1>;
        /**
         * Convenience type-alias to explicitly select 2D vectors.
         */
        template <typename T>
            using CVector2D = CVector<T, 2>;
        /**
         * Convenience type-alias to explicitly select 3D vectors.
         */
        template <typename T>
            using CVector3D = CVector<T, 3>;
        /**
         * Convenience type-alias to explicitly select 4D vectors.
         */
        template <typename T>
            using CVector4D = CVector<T, 4>;

        /**
         * Perform the dot product of two 2D-vectors.
         *
         * @param aLHS First vector.
         * @param aRHS Second vector.
         * @return     The result of the dot product.
         */
        template <typename T>
        typename CVector2D<T>::value_type dot(CVector2D<T> const &aLHS,
                                              CVector2D<T> const &aRHS);

        /**
         * Perform the dot product of two 3D-vectors.
         *
         * @param aLHS First vector.
         * @param aRHS Second vector.
         * @return     The result of the dot product.
         */
        template <typename T>
        typename CVector3D<T>::value_type dot(CVector3D<T> const &aLHS,
                                              CVector3D<T> const &aRHS);

        /**
         * Perform the dot product of two 4D-vectors.
         *
         * @param aLHS First vector.
         * @param aRHS Second vector.
         * @return     The result of the dot product.
         */
        template <typename T>
        typename CVector4D<T>::value_type dot(CVector4D<T> const &aLHS,
                                              CVector4D<T> const &aRHS);

        /**
         * Perform the cross product of two 2D-vectors.
         *
         * @param aLHS First vector.
         * @param aRHS Second vector.
         * @return     The result of the dot product.
         */
        template <typename T>
        CVector3D<T> cross(
                CVector3D<T> const &aLHS,
                CVector3D<T> const &aRHS);

        /**
         * Scale a 2D-vector by a factor.
         *
         * @param aVector The 2D vector to scale.
         * @param aFactor The factor to scale by.
         * @return        The operation-result as copy.
         */
        template <typename T>
        CVector2D<T> scale(CVector2D<T>                     const &aVector,
                           typename CVector2D<T>::value_type const&aFactor);

        /**
         * Scale a 3D-vector by a factor.
         *
         * @param aVector The 3D vector to scale.
         * @param aFactor The factor to scale by.
         * @return        The operation-result as copy.
         */
        template <typename T>
        CVector3D<T> scale(CVector3D<T>                      const&aVector,
                           typename CVector3D<T>::value_type const&aFactor);

        /**
         * Scale a 4D-vector by a factor.
         *
         * @param aVector The 4D vector to scale.
         * @param aFactor The factor to scale by.
         * @return        The operation-result as copy.
         */
        template <typename T>
        CVector4D<T> scale(CVector4D<T>                      const&aVector,
                           typename CVector4D<T>::value_type const&aFactor);

        /**
         * Normalize a vector of size N.
         * @param vec
         * @return
         */
        template <typename T, size_t N>
        CVector<T, N> normalize(CVector<T, N> const&vec);
        
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 1>::CVectorImpl( )
            : CField<T, sizeof(T), N, 1>({ 0 })
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 1>::CVectorImpl(value_type const aX)
            : CField<T, sizeof(T), 1, 1>({ aX })
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 1>::CVectorImpl(class_type const &aCopy)
            : CField<T, sizeof(T), 1, 1>(aCopy)
        {
        }        
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 1>::CVectorImpl(std::initializer_list<T> const aInitializer)
            : CField<T, sizeof(T), 1, 1>(aInitializer)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 1>::CVectorImpl(base_type const &aField)
            : CField<T, sizeof(T), 1, 1>(aField)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        DefineImmutableGetter(CVectorImpl, 1, x, 0);
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void CVectorImpl<T, 1>::x(value_type const&aValue)
        {
            this->m_field[ 0 ] = aValue;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 1> CVectorImpl<T, 1>::right()
        {
            return CVectorImpl<T, 1>({ static_cast<T>(1) });
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
            CVectorImpl<T, 2>::CVectorImpl( )
            : CField<T, sizeof(T), 2, 1>({ T(0), T(0) })
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 2>::CVectorImpl(
                value_type const aX,
                value_type const aY)
            : CField<T, sizeof(T), 2, 1>({ aX, aY })
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 2>::CVectorImpl(class_type const &aOther)
            : CField<T, sizeof(T), 2, 1>(aOther)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 2>::CVectorImpl(std::initializer_list<T> const aInitializer)
            : CField<T, sizeof(T), 2, 1>(aInitializer)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 2>::CVectorImpl(base_type const &aField)
            : CField<T, sizeof(T), 2, 1>(aField)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        DefineImmutableGetter(CVectorImpl, 2, x, 0);        
        DefineImmutableGetter(CVectorImpl, 2, y, 1);
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void CVectorImpl<T, 2>::x(value_type const &aValue)
        {
            this->m_field[ 0 ] = aValue;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void CVectorImpl<T, 2>::y(value_type const &aValue)
        {
            this->m_field[ 1 ] = aValue;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 2> CVectorImpl<T, 2>::right()
        {
            return CVectorImpl<T, 2>({ 1, 0 });
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 2> CVectorImpl<T, 2>::up()
        {
            return CVectorImpl<T, 2>({ 0, 1 });
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl( )
            : CField<T, sizeof(T), 3, 1>({ T(0), T(0), T(0) })
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl(value_type const aX,
                                       value_type const aY,
                                       value_type const aZ)
            : CField<T, sizeof(T), 3, 1>({ aX, aY, aZ })
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl(class_type const&aOther)
            : CField<T, sizeof(T), 3, 1>(aOther)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl(std::initializer_list<T> const aInitializer)
            : CField<T, sizeof(T), 3, 1>(aInitializer)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl(base_type const &aField)
            : CField<T, sizeof(T), 3, 1>(aField)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl(CVectorImpl<T, 2> const&aVector2D,
                                       T                 const&aZ)
            : CField<T, sizeof(T), 3, 1>({ aVector2D.x(), aVector2D.y(), aZ})
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        DefineImmutableGetter(CVectorImpl, 3, x, 0);        
        DefineImmutableGetter(CVectorImpl, 3, y, 1);        
        DefineImmutableGetter(CVectorImpl, 3, z, 2);
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void CVectorImpl<T, 3>::x(value_type const&aValue)
        {
            this->m_field[ 0 ] = aValue;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void CVectorImpl<T, 3>::y(value_type const&aValue)
        {
            this->m_field[ 1 ] = aValue;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void CVectorImpl<T, 3>::z(value_type const&aValue)
        {
            this->m_field[ 2 ] = aValue;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 3> CVectorImpl<T, 3>::forward( )
        {
            return CVectorImpl<T, 3>({ 1, 0, 0 });
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 3> CVectorImpl<T, 3>::right( )
        {
            return CVectorImpl<T, 3>({ 0, 1, 0 });
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 3> CVectorImpl<T, 3>::up( )
        {
            return CVectorImpl<T, 3>({ 0, 0, 1 });
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl( )
            : CField<T, sizeof(T), 4, 1>({ T(0), T(0), T(0), T(0) })
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(value_type const &aX,
                                       value_type const &aY,
                                       value_type const &aZ,
                                       value_type const &aW)
            : CField<T, sizeof(T), 4, 1>({ aX, aY, aZ, aW })
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(class_type const&aOther)
            : CField<T, sizeof(T), 4, 1>(aOther)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(std::initializer_list<T> const aInitializer)
            : CField<T, sizeof(T), 4, 1>(aInitializer)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(base_type const&aField)
            : CField<T, sizeof(T), 4, 1>(aField)
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(CVectorImpl<T, 2> const&aVector,
                                       T                 const&aZ,
                                       T                 const&aW)
            : CField<T, sizeof(T), 4, 1>({ aVector.x(), aVector.y(), aZ, aW })
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(CVectorImpl<T, 3> const &aVector,
                                       T                 const &aW)
            : CField<T, sizeof(T), 4, 1>({ aVector.x(), aVector.y(), aVector.z(), aW})
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        DefineImmutableGetter(CVectorImpl, 4, x, 0);        
        DefineImmutableGetter(CVectorImpl, 4, y, 1);        
        DefineImmutableGetter(CVectorImpl, 4, z, 2);        
        DefineImmutableGetter(CVectorImpl, 4, w, 3);
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void CVectorImpl<T, 4>::x(value_type const &aValue)
        {
            this->mField[ 0 ] = aValue;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void CVectorImpl<T, 4>::y(value_type const&aValue)
        {
            this->mField[ 1 ] = aValue;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void CVectorImpl<T, 4>::z(value_type const &aValue)
        {
            this->mField[ 2 ] = aValue;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        void CVectorImpl<T, 4>::w(value_type const &aValue)
        {
            this->mField[ 3 ] = aValue;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        typename CVector2D<T>::value_type dot(CVector2D<T> const &aLHS,
                                              CVector2D<T> const &aRHS)
        {
            return (( aLHS.x() * aRHS.x()) + ( aLHS.y() * aRHS.y()));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        typename CVector3D<T>::value_type dot(CVector3D<T> const &aLHS,
                                              CVector3D<T> const &aRHS)
        {
            return (( aLHS.x() * aRHS.x()) + ( aLHS.y() * aRHS.y()) + ( aLHS.z() * aRHS.z()));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        typename CVector4D<T>::value_type dot(CVector4D<T> const &aLHS,
                                              CVector4D<T> const &aRHS)
        {
            return (( aLHS.x() * aRHS.x()) +
                    ( aLHS.y() * aRHS.y()) +
                    ( aLHS.z() * aRHS.z()) +
                    ( aLHS.w() + aRHS.w()));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVector3D<T> cross(CVector3D<T> const &aLHS, CVector3D<T> const &aRHS)
        {
            return CVector3D<T>({ ( (aLHS.y() * aRHS.z()) - (aLHS.z() * aRHS.y()) ),
                                  ( (aLHS.z() * aRHS.x()) - (aLHS.x() * aRHS.z()) ),
                                  ( (aLHS.x() * aRHS.y()) - (aLHS.y() * aRHS.x()) ) });
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVector2D<T> scale(CVector2D<T>                      const &aVector,
                           typename CVector2D<T>::value_type const &aFactor)
        {
            return CVector2D<T>(aVector).scale(aFactor);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVector3D<T> scale(CVector3D<T>                      const&aVector,
                           typename CVector3D<T>::value_type const&aFactor)
        {
            return CVector3D<T>(aVector).scale(aFactor);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T>
        CVector4D<T> scale(CVector4D<T>                      const&aVector,
                           typename CVector4D<T>::value_type const&aFactor)
        {
            return CVector4D<T>(aVector).scale(aFactor);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename T, size_t N>
        CVector<T, N> normalize(CVector<T, N> const &aVector)
        {
            return CVector<T, N>(aVector).normalize();
        }
        
        /**
         * Convenience type alias for 1D float vectors.
         */
        using CVector1D_t = CVector1D<float>;
        /**
         * Convenience type alias for 2D float vectors.
         */
        using CVector2D_t = CVector2D<float>;
        /**
         * Convenience type alias for 3D float vectors.
         */
        using CVector3D_t = CVector3D<float>;
        /**
         * Convenience type alias for 4D float vectors.
         */
        using CVector4D_t = CVector4D<float>;
        
    } // namespace Math
} // namespace Engine

#endif
