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
        
        template <typename T, std::size_t N>
            class CVectorImpl;
        
        /**
         * @brief The CVectorImpl<T, _Tp2> class
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
            CVectorImpl( );
            CVectorImpl(class_type const&aOther);
            CVectorImpl(base_type const&aOther);
            CVectorImpl(std::initializer_list<T> const aInitializer);
            CVectorImpl(value_type const aX);
            
        public_methods:
            DeclareImmutableGetter(CVectorImpl, 1, x)
            
            // Return a CFieldAccessor for the stored x-component to safely
            // assign a new value using the assignment-operator. Setter.
            void x(value_type const&aValue);
            
            static CVectorImpl<T, 1> right( );
        };
        
        /**
         * @brief The CVectorImpl<T, _Tp2> class
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
            CVectorImpl( );
            CVectorImpl(class_type const &aOther);
            CVectorImpl(base_type const &aOther);
            CVectorImpl(std::initializer_list<T> const aInitializer);
            CVectorImpl(
                value_type const aX,
                value_type const aY
            );

        public_methods:
            DeclareImmutableGetter(CVectorImpl, 2, x);
            DeclareImmutableGetter(CVectorImpl, 2, y);

            // Return a CFieldAccessor for the stored x-component to safely
            // assign a new value using the assignment-operator. Setter.
            void x(value_type const &aValue);

            // Return a CFieldAccessor for the stored y-component to safely
            // assign a new value using the assignment-operator. Setter.
            void y(value_type const &aValue);

            static CVectorImpl<T, 2> right( );
            static CVectorImpl<T, 2> up( );

            DefinePermutationAccessor2D(x, y);
            DefinePermutationAccessor2D(y, x);
        };
        
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
            CVectorImpl( );
            CVectorImpl(class_type const &aOther);
            CVectorImpl(base_type const &aOther);
            CVectorImpl(std::initializer_list<T> const);
            CVectorImpl(
                value_type const aX,
                value_type const aY,
                value_type const aZ);
            CVectorImpl(
                CVectorImpl<T, 2> const&aVector2D,
                T                 const&aZ         = T(0)
            );

        public_methods:
            DeclareImmutableGetter(CVectorImpl, 3, x);
            DeclareImmutableGetter(CVectorImpl, 3, y);
            DeclareImmutableGetter(CVectorImpl, 3, z);

// Return a CFieldAccessor for the stored x-component to safely
// assign a new value using the assignment-operator. Setter.
            void x(value_type const&aValue);

// Return a CFieldAccessor for the stored y-component to safely
// assign a new value using the assignment-operator. Setter.
            void y(value_type const&aValue);

// Return a CFieldAccessor for the stored z-component to safely
// assign a new value using the assignment-operator. Setter.
            void z(value_type const&aValue);

            static CVectorImpl<T, 3> forward( );
            static CVectorImpl<T, 3> right( );
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
            CVectorImpl( );
            CVectorImpl(class_type const&aOther);
            CVectorImpl(base_type const&aOther);
            CVectorImpl(std::initializer_list<T> const aInitializer);
            CVectorImpl(
                value_type const &aX,
                value_type const &aY,
                value_type const &aZ,
                value_type const &aW
            );
            CVectorImpl(
                CVectorImpl<T, 3> const &aVector3D,
                T                 const &aW         = T(0)
            );
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

// Return a CFieldAccessor for the stored x-component to safely
// assign a new value using the assignment-operator. Setter.
            void x(value_type const&aValue);

// Return a CFieldAccessor for the stored y-component to safely
// assign a new value using the assignment-operator. Setter.
            void y(value_type const&aValue);

// Return a CFieldAccessor for the stored z-component to safely
// assign a new value using the assignment-operator. Setter.
            void z(value_type const&aValue);

// Return a CFieldAccessor for the stored z-component to safely
// assign a new value using the assignment-operator. Setter.
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
         * @brief
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
            CVector( );
            CVector(std::initializer_list<T> const);
            CVector(base_type const&);
            CVector(class_type const&cpy);

        public_operators:
            bool operator==(class_type const&);

        public_methods:
            class_type  scale(value_type const factor);
            value_type  length() const;
            value_type  squared_length() const;
            value_type  abs() const;
            class_type& normalize( );
        };
        
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        CVector<T, N, TDerived>::CVector( )
            : TDerived()
        {
        }

        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        CVector<T, N, TDerived>::CVector(std::initializer_list<T> const aInitializer)
            : TDerived(aInitializer)
        {
        }
        
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

        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        CVector<T, N, TDerived>::CVector(class_type const&aOther)
            : TDerived(aOther)
        {
        }
        
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
        
        template <
            typename    T,
            std::size_t N,
            typename    TDerived
        >
        typename CVector<T, N, TDerived>::class_type
        CVector<T, N, TDerived>::scale(value_type const aFactor)
        {
            this->operator*=(aFactor);

            return *this;
        }
        
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
        
        template <
            typename    T,
            std::size_t N,
            typename    TDerived = CVectorImpl<T, N>
        >
        CVector<T, N, TDerived> operator+(CVector<T, N, TDerived> const &aLHS,
                                          CVector<T, N, TDerived> const &aRHS)
        {
            using BaseType_t = typename CVector<T, N, TDerived>::base_type;

            BaseType_t lhs = static_cast<BaseType_t>(aLHS);
            BaseType_t rhs = static_cast<BaseType_t>(aRHS);

            BaseType_t vec = operator+(lhs, rhs);

            return CVector<T, N, TDerived>(vec);
        }
        
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

            BaseType_t vec = operator-(lhs, rhs);

            return CVector<T, N, TDerived>(vec);
        }
        
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
            BaseType_t vec = operator*(lhs, aFactor);

            return CVector<T, N, TDerived>(vec);
        }
        
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
            BaseType_t vec = operator/(lhs, aFactor);

            return CVector<T, N, TDerived>(vec);
        }
        
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
        
        template <typename T>
            using CVector1D = CVector<T, 1>;
        template <typename T>
            using CVector2D = CVector<T, 2>;
        template <typename T>
            using CVector3D = CVector<T, 3>;
        template <typename T>
            using CVector4D = CVector<T, 4>;

         // Returns a copy of the 2D dot product of two Vec2 instances.
        template <typename T>
        typename CVector2D<T>::value_type dot(CVector2D<T> const &aLHS,
                                              CVector2D<T> const &aRHS);

        // Returns a copy of the 3D dot product of two Vec3 instances.
        template <typename T>
        typename CVector3D<T>::value_type dot(CVector3D<T> const &aLHS,
                                              CVector3D<T> const &aRHS);

        // Returns a copy of the 4D dot product of two Vec4 instances.
        template <typename T>
        typename CVector4D<T>::value_type dot(CVector4D<T> const &aLHS,
                                              CVector4D<T> const &aRHS);

        // Returns a copy of the 3D cross product of two Vec3 instances.
        template <typename T>
        CVector3D<T> cross(
                CVector3D<T> const &aLHS,
                CVector3D<T> const &aRHS);

        // Returns a scaled copy of an arbitrary Vec2 instance.
        template <typename T>
        CVector2D<T> scale(CVector2D<T>                     const &aVector,
                           typename CVector2D<T>::value_type const&aFactor);

        // Returns a scaled copy of an arbitrary Vec3 instance.
        template <typename T>
        CVector3D<T> scale(CVector3D<T>                      const&aVector,
                           typename CVector3D<T>::value_type const&aFactor);

// Returns a scaled copy of an arbitrary Vec4 instance.
        template <typename T>
        CVector4D<T> scale(CVector4D<T>                      const&aVector,
                           typename CVector4D<T>::value_type const&aFactor);
        
        template <typename T, size_t N>
        CVector<T, N> normalize(CVector<T, N> const&vec);
        
        template <typename T>
        CVectorImpl<T, 1>::CVectorImpl( )
            : CField<T, sizeof(T), N, 1>({ 0 })
        {
        }
        
        template <typename T>
        CVectorImpl<T, 1>::CVectorImpl(value_type const aX)
            : CField<T, sizeof(T), 1, 1>({ aX })
        {
        }
        
        template <typename T>
        CVectorImpl<T, 1>::CVectorImpl(class_type const &aCopy)
            : CField<T, sizeof(T), 1, 1>(aCopy)
        {
        }
        
        template <typename T>
        CVectorImpl<T, 1>::CVectorImpl(std::initializer_list<T> const aInitializer)
            : CField<T, sizeof(T), 1, 1>(aInitializer)
        {
        }
        
        template <typename T>
        CVectorImpl<T, 1>::CVectorImpl(base_type const &aField)
            : CField<T, sizeof(T), 1, 1>(aField)
        {
        }
        
        DefineImmutableGetter(CVectorImpl, 1, x, 0);
        
        template <typename T>
        void CVectorImpl<T, 1>::x(value_type const&aValue)
        {
            this->m_field[ 0 ] = aValue;
        }
        
        template <typename T>
        CVectorImpl<T, 1> CVectorImpl<T, 1>::right()
        {
            return CVectorImpl<T, 1>({ static_cast<T>(1) });
        }
        
        template <typename T>
            CVectorImpl<T, 2>::CVectorImpl( )
            : CField<T, sizeof(T), 2, 1>({ T(0), T(0) })
        {
        }
        
        template <typename T>
        CVectorImpl<T, 2>::CVectorImpl(
                value_type const aX,
                value_type const aY)
            : CField<T, sizeof(T), 2, 1>({ aX, aY })
        {
        }

        template <typename T>
        CVectorImpl<T, 2>::CVectorImpl(class_type const &aOther)
            : CField<T, sizeof(T), 2, 1>(aOther)
        {
        }
        
        template <typename T>
        CVectorImpl<T, 2>::CVectorImpl(std::initializer_list<T> const aInitializer)
            : CField<T, sizeof(T), 2, 1>(aInitializer)
        {
        }
        
        template <typename T>
        CVectorImpl<T, 2>::CVectorImpl(base_type const &aField)
            : CField<T, sizeof(T), 2, 1>(aField)
        {
        }
        
        DefineImmutableGetter(CVectorImpl, 2, x, 0);        
        DefineImmutableGetter(CVectorImpl, 2, y, 1);
        
        template <typename T>
        void CVectorImpl<T, 2>::x(value_type const &aValue)
        {
            this->m_field[ 0 ] = aValue;
        }
        
        template <typename T>
        void CVectorImpl<T, 2>::y(value_type const &aValue)
        {
            this->m_field[ 1 ] = aValue;
        }
        
        template <typename T>
        CVectorImpl<T, 2> CVectorImpl<T, 2>::right()
        {
            return CVectorImpl<T, 2>({ 1, 0 });
        }
        
        template <typename T>
        CVectorImpl<T, 2> CVectorImpl<T, 2>::up()
        {
            return CVectorImpl<T, 2>({ 0, 1 });
        }
        
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl( )
            : CField<T, sizeof(T), 3, 1>({ T(0), T(0), T(0) })
        {
        }
        
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl(value_type const aX,
                                       value_type const aY,
                                       value_type const aZ)
            : CField<T, sizeof(T), 3, 1>({ aX, aY, aZ })
        {
        }
        
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl(class_type const&aOther)
            : CField<T, sizeof(T), 3, 1>(aOther)
        {
        }
        
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl(std::initializer_list<T> const aInitializer)
            : CField<T, sizeof(T), 3, 1>(aInitializer)
        {
        }
        
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl(base_type const &aField)
            : CField<T, sizeof(T), 3, 1>(aField)
        {
        }
        
        template <typename T>
        CVectorImpl<T, 3>::CVectorImpl(CVectorImpl<T, 2> const&aVector2D,
                                       T                 const&aZ
        )
            : CField<T, sizeof(T), 3, 1>({ aVector2D.x(), aVector2D.y(), aZ})
        {
        }

        // Return a copy of the stored x-component. Getter.
        
        DefineImmutableGetter(CVectorImpl, 3, x, 0);        
        DefineImmutableGetter(CVectorImpl, 3, y, 1);        
        DefineImmutableGetter(CVectorImpl, 3, z, 2);

        // Return a CFieldAccessor for the stored x-component to safely assign a
        // new value using the assignment-operator. Setter.
        template <typename T>
        void CVectorImpl<T, 3>::x(value_type const&aValue)
        {
            this->m_field[ 0 ] = aValue;
        }

        // Return a CFieldAccessor for the stored y-component to safely assign a
        // new value using the assignment-operator. Setter.
        template <typename T>
        void CVectorImpl<T, 3>::y(value_type const&aValue)
        {
            this->m_field[ 1 ] = aValue;
        }

        // Return a CFieldAccessor for the stored z-component to safely assign a
        // new value using the assignment-operator. Setter.
        template <typename T>
        void CVectorImpl<T, 3>::z(value_type const&aValue)
        {
            this->m_field[ 2 ] = aValue;
        }
        
        template <typename T>
        CVectorImpl<T, 3> CVectorImpl<T, 3>::forward( )
        {
            return CVectorImpl<T, 3>({ 1, 0, 0 });
        }
        
        template <typename T>
        CVectorImpl<T, 3> CVectorImpl<T, 3>::right( )
        {
            return CVectorImpl<T, 3>({ 0, 1, 0 });
        }
        
        template <typename T>
        CVectorImpl<T, 3> CVectorImpl<T, 3>::up( )
        {
            return CVectorImpl<T, 3>({ 0, 0, 1 });
        }
        
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl( )
            : CField<T, sizeof(T), 4, 1>({ T(0), T(0), T(0), T(0) })
        {
        }
        
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(value_type const &aX,
                                       value_type const &aY,
                                       value_type const &aZ,
                                       value_type const &aW)
            : CField<T, sizeof(T), 4, 1>({ aX, aY, aZ, aW })
        {
        }
        
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(class_type const&aOther)
            : CField<T, sizeof(T), 4, 1>(aOther)
        {
        }
        
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(std::initializer_list<T> const aInitializer)
            : CField<T, sizeof(T), 4, 1>(aInitializer)
        {
        }
        
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(base_type const&aField)
            : CField<T, sizeof(T), 4, 1>(aField)
        {
        }
        
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(CVectorImpl<T, 2> const&aVector,
                                       T                 const&aZ,
                                       T                 const&aW)
            : CField<T, sizeof(T), 4, 1>({ aVector.x(), aVector.y(), aZ, aW })
        {
        }
        
        template <typename T>
        CVectorImpl<T, 4>::CVectorImpl(CVectorImpl<T, 3> const &aVector,
                                       T                 const &aW)
            : CField<T, sizeof(T), 4, 1>({ aVector.x(), aVector.y(), aVector.z(), aW})
        {
        }
        
        DefineImmutableGetter(CVectorImpl, 4, x, 0);        
        DefineImmutableGetter(CVectorImpl, 4, y, 1);        
        DefineImmutableGetter(CVectorImpl, 4, z, 2);        
        DefineImmutableGetter(CVectorImpl, 4, w, 3);
        
        template <typename T>
        void CVectorImpl<T, 4>::x(value_type const &aValue)
        {
            this->mField[ 0 ] = aValue;
        }
        
        template <typename T>
        void CVectorImpl<T, 4>::y(value_type const&aValue)
        {
            this->mField[ 1 ] = aValue;
        }
        
        template <typename T>
        void CVectorImpl<T, 4>::z(value_type const &aValue)
        {
            this->mField[ 2 ] = aValue;
        }
        
        template <typename T>
        void CVectorImpl<T, 4>::w(value_type const &aValue)
        {
            this->mField[ 3 ] = aValue;
        }
        
        template <typename T>
        typename CVector2D<T>::value_type dot(CVector2D<T> const &aLHS,
                                              CVector2D<T> const &aRHS)
        {
            return (( aLHS.x() * aRHS.x()) + ( aLHS.y() * aRHS.y()));
        }
        
        template <typename T>
        typename CVector3D<T>::value_type dot(CVector3D<T> const &aLHS,
                                              CVector3D<T> const &aRHS)
        {
            return (( aLHS.x() * aRHS.x()) + ( aLHS.y() * aRHS.y()) + ( aLHS.z() * aRHS.z()));
        }
        
        template <typename T>
        typename CVector4D<T>::value_type dot(CVector4D<T> const &aLHS,
                                              CVector4D<T> const &aRHS)
        {
            return (( aLHS.x() * aRHS.x()) +
                    ( aLHS.y() * aRHS.y()) +
                    ( aLHS.z() * aRHS.z()) +
                    ( aLHS.w() + aRHS.w()));
        }
        
        template <typename T>
        CVector3D<T> cross(CVector3D<T> const &aLHS, CVector3D<T> const &aRHS)
        {
            return CVector3D<T>({ ( (aLHS.y() * aRHS.z()) - (aLHS.z() * aRHS.y()) ),
                                  ( (aLHS.z() * aRHS.x()) - (aLHS.x() * aRHS.z()) ),
                                  ( (aLHS.x() * aRHS.y()) - (aLHS.y() * aRHS.x()) ) });
        }
        
        template <typename T>
        CVector2D<T> scale(CVector2D<T>                      const &aVector,
                           typename CVector2D<T>::value_type const &aFactor)
        {
            return CVector2D<T>(aVector).scale(aFactor);
        }
        
        template <typename T>
        CVector3D<T> scale(CVector3D<T>                      const&aVector,
                           typename CVector3D<T>::value_type const&aFactor)
        {
            return CVector3D<T>(aVector).scale(aFactor);
        }
        
        template <typename T>
        CVector4D<T> scale(CVector4D<T>                      const&aVector,
                           typename CVector4D<T>::value_type const&aFactor)
        {
            return CVector4D<T>(aVector).scale(aFactor);
        }
        
        template <typename T, size_t N>
            CVector<T, N> normalize(CVector<T, N> const &aVector)
            {
                return CVector<T, N>(aVector).normalize();
            }
        
        using CVector1D_t = CVector1D<float>;
        using CVector2D_t = CVector2D<float>;
        using CVector3D_t = CVector3D<float>;
        using CVector4D_t = CVector4D<float>;
        
    } // namespace Math
} // namespace Engine

#endif
