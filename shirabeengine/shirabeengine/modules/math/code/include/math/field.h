/*!
 * @file      field.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#ifndef __SHIRABE_MATH_FIELD_H__
#define __SHIRABE_MATH_FIELD_H__

#include <exception>
#include <sstream>
#include <iterator>
#include <initializer_list>
#include <cstring>
#include <cassert>

#include "platform/platform.h"
#include "base/declaration.h"

namespace Engine
{
    namespace Math
    {

        #define D_FIELD_DEFAULT_SIZE   1
        #define D_FIELD_DEFAULT_STRIDE 1
        #define D_ASSERT_FIELD_SIZE_AND_STRIDE() \
            static_assert(((TN % TStride) == 0), "Invalid TN and TStride combination.");


        /**
         * Defines a templated, non-growable field-type to hold certain type of data as
         * a vector-structure, internally stored as an array.
         * A field can be two dimensional, i.e. the stride parameter is greater than 1,
         * but has to obey the constraint: (TN % TStride) == 0
         *
         * @tparam T,        Underlying meet type of the field.
         * @tparam TByteSize Bytesize of the data type.
         *                   Default: sizeof(T)
         * @tparam TN        Number of elements in field.
         *                   Default: 1
         * @tparam TStride   Number of elements per row, in case of a two dimensional field.
         *                   Default: 1
         */
        template <
                typename    T,
                std::size_t TByteSize = sizeof(T),
                std::size_t TN        = D_FIELD_DEFAULT_SIZE,
                std::size_t TStride   = D_FIELD_DEFAULT_STRIDE
                >
        class SHIRABE_TEST_EXPORT CField
        {
        public_typedefs:
            typedef CField<T, TByteSize, TN, TStride> class_type;
            typedef T                                 value_type;
            typedef value_type const                  const_value_type;

        public_constructors:
            /**
             * Default initializes a field.
             */
            CField() = default;

            /**
             * Initialize a field from an initializer list.
             *
             * @param aSource An initializer list containing at least N values.
             */
            CField(std::initializer_list<T> const &aSource);

            /**
             * Initialize a field from another field.
             *
             * @param aCopy The other instance to copy from.
             */
            CField(CField<T, TByteSize, TN, TStride> const &aCopy);

        public_destructors:
            /**
             * Destroy this field instance
             */
            virtual ~CField() = default;

        public_operators:

            /**
             * Assign another field and overwrite contained values.
             *
             * @param right Field to assign.
             * @return      Self-Reference
             */
            class_type&operator= (class_type const&aOther);

            /**
             * Returns an immutable value reference to an element in the field at index 'aIndex'.
             *
             * @param  aIndex 0-based position in the field.
             * @return        The value contained at 'aIndex' as const-ref, if the index is in bounds.
             * @throws        std::out_of_range if: 0 < aIndex < TN.
             */
            T const &operator[](std::size_t const aIndex) const;

            /**
             * Returns a mutable value reference to an element in the field at index 'aIndex'.
             *
             * @param  aIndex 0-based position in the field.
             * @return        The value contained at 'aIndex' as const-ref, if the index is in bounds.
             * @throws        std::out_of_range if: 0 < aIndex < TN.
             */
            T &operator[](std::size_t const aIndex);

            /**
             * Compares this instance to another for bitwise equality.
             *
             * @param aOther The other instance to be compared with.
             * @return       True, if bitwise equal. False otherwise.
             */
            bool operator==(class_type const&aOther);

            /**
             * Add another field to this instance.
             *
             * @param aOther The field to be added.
             */
            void operator+=(class_type const& aRight);

            /**
             * Subtract another field from this instance.
             *
             * @param aOther The field to subtract.
             */
            void operator-=(class_type const& aRight);

            /**
             * Multiply this field instance with a given factor.
             *
             * @param aFactor The factor to multiply with.
             */
            void operator*=(T const aFactor);

            /**
             * Divide this field instance by the passed factor.
             *
             * @param aFactor The factor to divide by.
             */
            void operator/=(T const aFactor);

        public_methods:
            /**
             * Return a const pointer to the internal data array.
             * Used for read-only access.
             *
             * @return A const pointer to the internal data array.
             */
            T const*const const_ptr() const;

            /**
             * Return a pointer to the internal data array.
             * Used for read-write access.
             *
             * @return A pointer to the internal data array.
             */
            T *const ptr();

            /**
             * Return the total number of elements in the field.
             *
             * @return See above...
             */
            std::size_t const size() const;

            /**
             * Return the size of a single element of the field in bytes.
             *
             * @return See also above...
             */
            std::size_t const byte_size() const;

            /**
             * Return the number of (imaginary) columns in the field.
             * Dividing the size by the byte_stride should return the number of (imaginary) rows!
             * If the field should contain an arbitrary number of elements, pass 1 for the byte_stride.
             *
             * @return  See again... you've guessed it...
             */
            std::size_t const byte_stride() const;

            /**
             * Serialize this field to a comma-delimited string.
             *
             * @return Stringized field.
             */
            std::string toString();

        protected_methods:


            /**
             * Assign another matrix to this instance.
             * Internally theres only a copy operation taking place
             * overriding the old data of this instance, if any!
             *
             * @param aOther The field instance to assign.
             */
            void assign(class_type const& aOther);

        protected_members:
            T mField[TN * TByteSize];
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        #define D_FIELD_TEMPLATE_DECL \
                    template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        #define D_FIELD \
                    CField<T, TByteSize, TN, TStride>
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        D_FIELD_TEMPLATE_DECL
        D_FIELD::CField(
                std::initializer_list<T> const &aSource)
        {
            D_ASSERT_FIELD_SIZE_AND_STRIDE();

            std::size_t i = 0;

            for(typename std::initializer_list<T>::value_type const&v : aSource)
                if(i < TN)
                    mField[i++] = v;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        D_FIELD_TEMPLATE_DECL
        D_FIELD::CField(
                class_type const&aCopy)
        {
            D_ASSERT_FIELD_SIZE_AND_STRIDE();

            assign(aCopy);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        typename D_FIELD::class_type &D_FIELD::operator=(class_type const& right)
        {
            D_ASSERT_FIELD_SIZE_AND_STRIDE();

            assign(right);
            return *this;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        T const &D_FIELD::operator[] (std::size_t const aIndex) const
        {
            D_ASSERT_FIELD_SIZE_AND_STRIDE();

            if (TN > 0 && TN > aIndex)
                return *(mField + aIndex);

            throw std::out_of_range("Index out of field bounds.");
        }        
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        T& D_FIELD::operator[] (std::size_t const aIndex)
        {
            // Cast to const this, so that we can reuse the const operator[].
            class_type const*const cthis  = static_cast<class_type const*const>(this);
            value_type const&      cvalue = cthis->operator[](aIndex);
            value_type      &      value  = const_cast<value_type&>(cvalue);
            return value;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        bool D_FIELD::operator==(class_type const&aOther)
        {
            int32_t result = memcmp(mField, aOther.mField, (TByteSize * TN));
            return (result == 0);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        void D_FIELD::operator+=(class_type const& aOther)
        {
            for(size_t i = 0; i < TN; ++i)
                mField[i] += aOther[i];
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        void D_FIELD::operator-=(class_type const& r)
        {
            for(size_t i = 0; i < TN; ++i)
                mField[i] -= r[i];
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        void D_FIELD::operator*=(T const aFactor)
        {
            for(size_t i = 0; i < TN; ++i)
                mField[i] *= aFactor;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        void D_FIELD::operator/=(T const aFactor)
        {
            operator*=((1 / aFactor));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        void D_FIELD::assign(class_type const& aOther)
        {
            memcpy(mField, aOther.const_ptr(), (TN * TByteSize));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        T const* const D_FIELD::const_ptr() const
        {
            return &mField[0];
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        T * const D_FIELD::ptr()
        {
            return mField;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        std::size_t const D_FIELD::size() const
        {
            return TN;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        std::size_t const D_FIELD::byte_size() const
        {
            return TByteSize;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        std::size_t const D_FIELD::byte_stride() const
        {
            return TStride;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template<typename T, std::size_t TByteSize, std::size_t TN, std::size_t TStride>
        std::string D_FIELD::toString()
        {
            std::stringstream ss;
            for (size_t i = 0; i < TN; i += TStride)
            {
                for (size_t j = 0; j < TStride; ++j)
                {
                    ss << (((j) == 0) ? "" : ", ");
                    ss << mField[i + j];
                }

                ss << ",\n";
            }

            ss << "\n";

            return ss.str();
        }

        /**
         * Accumulate two field instances and return the reuslt of the
         * operation as a copy.
         *
         * @param  aLHS Left operand
         * @param  aRHS Right operand
         * @return A new field instance with the result of addition.
         */
        template <
                typename    T,
                std::size_t TByteSize = sizeof(T),
                std::size_t TN        = D_FIELD_DEFAULT_SIZE,
                std::size_t TStride   = D_FIELD_DEFAULT_STRIDE
                >
        typename D_FIELD::class_type
        operator+(
                typename D_FIELD::class_type const& aLHS,
                typename D_FIELD::class_type const& aRHS)
        {
            CField<T, TByteSize, TN, TStride> copy(aLHS);

            copy += aRHS;
            return copy;
        }

        /**
         * Subtract the right operand from the left and return the reuslt of the operation as a copy.
         *
         * @param  aLHS Left operand
         * @param  aRHS Right operand
         * @return A new field instance with the result of subtraction.
         */
        template <
                typename    T,
                std::size_t TByteSize = sizeof(T),
                std::size_t TN        = D_FIELD_DEFAULT_SIZE,
                std::size_t TStride   = D_FIELD_DEFAULT_STRIDE
                >
        typename D_FIELD::class_type
        operator-(
                typename D_FIELD::class_type const& aLHS,
                typename D_FIELD::class_type const& aRHS)
        {
            D_FIELD copy(aLHS);

            copy -= aRHS;
            return copy;
        }

        /**
         * Multiply a field instance with a factor and return the result of the operation as a copy.
         *
         * @param  aLHS    Field instance to multiply with.
         * @param  aFactor Factor to multiply.
         * @return A new field instance with the result of multiplication.
         */
        template <
                typename    T,
                std::size_t TByteSize = sizeof(T),
                std::size_t TN        = D_FIELD_DEFAULT_SIZE,
                std::size_t TStride   = D_FIELD_DEFAULT_STRIDE
                >
        typename D_FIELD::class_type
        operator*(
                typename D_FIELD::class_type const &aLHS,
                typename D_FIELD::value_type const &aFactor)
        {
            D_FIELD copy = D_FIELD(aLHS);

            copy *= aFactor;
            return copy;
        }

        /**
         * Multiply a field instance with a factor and return the result of the operation as a copy.
         *
         * @param  aFactor Factor to multiply.
         * @param  aLHS    Field instance to multiply with.
         * @return A new field instance with the result of multiplication.
         */
        template <
                typename    T,
                std::size_t TByteSize = sizeof(T),
                std::size_t TN        = D_FIELD_DEFAULT_SIZE,
                std::size_t TStride   = D_FIELD_DEFAULT_STRIDE
                >
        typename D_FIELD::class_type
        operator*(
                typename D_FIELD::value_type const &aFactor,
                typename D_FIELD::class_type const &aLHS)
        {
            return operator*<T, TByteSize, TN, TStride>(aLHS, aFactor);
        }

        /**
         * Divide a field instance by a factor and return the result of the operation as a copy.
         *
         * @param  aLHS    Field instance to multiply with.
         * @param  aFactor Factor to multiply.
         * @return A new field instance with the result of division.
         */
        template <
                typename    T,
                std::size_t TByteSize = sizeof(T),
                std::size_t TN        = D_FIELD_DEFAULT_SIZE,
                std::size_t TStride   = D_FIELD_DEFAULT_STRIDE
                >
        typename D_FIELD::class_type
        operator/(
                typename D_FIELD::class_type const &aLHS,
                typename D_FIELD::value_type const &aFactor)
        {
            return operator*<T, TByteSize, TN, TStride>(aLHS, (T(1.0) / aFactor));
        }

    }
}
#endif
