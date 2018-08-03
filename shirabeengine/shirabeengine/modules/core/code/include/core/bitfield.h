/*!
 * @file      CBitField.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */
#ifndef __SHIRABE_CORE_BITFIELD_H__
#define __SHIRABE_CORE_BITFIELD_H__

#include <type_traits>
#include <base/declaration.h>

namespace engine
{
    namespace Core
    {
        template <typename TEnum>
        class CBitField
        {
        public_typedefs:
            using Value_t = std::underlying_type_t<TEnum>;

        public_static_functions:
            /**
             * @brief getValue
             * @param e
             * @return
             */
            static Value_t getValue(TEnum const&e);

        public_constructors:
            /**
             * @brief CBitField
             */
            CBitField();

            /**
             * @brief CBitField
             * @param aInitial
             */

            CBitField(TEnum const &aInitial);

            /**
             * @brief CBitField
             * @param aInitial
             */
            CBitField(TEnum &&aInitial);

            /**
             * @brief CBitField
             * @param aInitial
             */
            CBitField(Value_t const&aInitial);

            /**
             * @brief CBitField
             * @param aInitial
             */
            CBitField(Value_t &&aInitial);

            /**
             * @brief CBitField
             * @param aInitial
             */
            CBitField(CBitField<TEnum> const &aInitial);

            /**
             * @brief CBitField
             * @param aInitial
             */
            CBitField(CBitField<TEnum> &&aInitial);

        public_operators:
            /**
             * @brief operator Value_t
             */
            operator Value_t();

            /**
             * @brief operator =
             * @param aOther
             * @return
             */
            CBitField<TEnum> &operator=(TEnum const&aOther);

            /**
             * @brief operator =
             * @param aOther
             * @return
             */
            CBitField<TEnum> &operator=(TEnum &&aOther);

            /**
             * @brief operator =
             * @param aOther
             * @return
             */
            CBitField<TEnum> &operator=(Value_t const&aOther);

            /**
             * @brief operator =
             * @param aOther
             * @return
             */
            CBitField<TEnum> &operator=(Value_t &&aOther);

            /**
             * @brief operator =
             * @param aOther
             * @return
             */
            CBitField<TEnum> &operator=(CBitField<TEnum> const&aOther);

            /**
             * @brief operator =
             * @param aOther
             * @return
             */
            CBitField<TEnum> &operator=(CBitField<TEnum> &&aOther);

            /**
             * @brief operator |
             * @param aValue
             * @return
             */
            CBitField<TEnum> operator|(TEnum const&aValue);

            /**
             * @brief operator &
             * @param aValue
             * @return
             */
            CBitField<TEnum> operator&(TEnum const&aValue);

        public_methods:
            /**
             * @brief set
             * @param aValue
             */
            void set(TEnum const&aValue);

            /**
             * @brief unset
             * @param aValue
             */
            void unset(TEnum const&aValue);

            /**
             * @brief check
             * @param aValue
             * @return
             */
            bool check(TEnum const&aValue) const;

            /**
             * @brief check
             * @param aCBitField
             * @return
             */
            bool check(CBitField<TEnum> const&aCBitField) const;

            /**
             * @brief value
             * @return
             */
            Value_t const&value() const;

        private_members:
            Value_t mCBitField;
        };

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        typename CBitField<TEnum>::Value_t
        CBitField<TEnum>::getValue(TEnum const&aValue)
        {
            return static_cast<Value_t>(aValue);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>::CBitField()
            : mCBitField(0)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>::CBitField(TEnum const &aInitial)
            : mCBitField(CBitField<TEnum>::getValue(aInitial))
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>::CBitField(TEnum &&aInitial)
            : mCBitField(CBitField<TEnum>::getValue(aInitial))
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>::CBitField(Value_t const &aInitial)
            : mCBitField(aInitial)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>::CBitField(Value_t &&aInitial)
            : mCBitField(aInitial)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>::CBitField(CBitField<TEnum> const &aInitial)
            : mCBitField(aInitial.value())
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>::CBitField(CBitField<TEnum> &&aInitial)
            : mCBitField(aInitial.value())
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>::operator Value_t()
        {
            return static_cast<std::underlying_type_t<TEnum>>(mCBitField);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        void
        CBitField<TEnum>::set(TEnum const&e)
        {
            mCBitField = (mCBitField | CBitField<TEnum>::getValue(e));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        void CBitField<TEnum>::unset(TEnum const&e)
        {
            mCBitField = (mCBitField ^ (mCBitField & CBitField<TEnum>::getValue(e)));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        bool CBitField<TEnum>::check(TEnum const&e) const
        {
            return ((mCBitField & CBitField<TEnum>::getValue(e)) == CBitField<TEnum>::getValue(e));
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        bool CBitField<TEnum>::check(CBitField<TEnum> const&bf) const
        {
            return ((mCBitField & bf.value()) == bf.value());
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        typename CBitField<TEnum>::Value_t const&
        CBitField<TEnum>::value() const
        {
            return mCBitField;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>& CBitField<TEnum>::operator=(TEnum const &aOther)
        {
            mCBitField = CBitField<TEnum>::getValue(aOther);
            return (*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>& CBitField<TEnum>::operator=(TEnum &&aOther)
        {
            mCBitField = CBitField<TEnum>::getValue(aOther);
            return (*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>& CBitField<TEnum>::operator=(Value_t const&aOther)
        {
            mCBitField = aOther;
            return (*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>& CBitField<TEnum>::operator=(Value_t &&aOther)
        {
            mCBitField = aOther;
            return (*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>& CBitField<TEnum>::operator=(CBitField<TEnum> const&aOther)
        {
            mCBitField = aOther.value();
            return (*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum>& CBitField<TEnum>::operator=(CBitField<TEnum> &&aOther)
        {
            mCBitField = aOther.value();
            return (*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum> CBitField<TEnum>::operator|(TEnum const &aValue)
        {
            CBitField<TEnum> c(value());
            c.set(aValue);
            return c;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TEnum>
        CBitField<TEnum> CBitField<TEnum>::operator&(TEnum const&aValue)
        {
            CBitField<TEnum> c(value() & CBitField<TEnum>::getValue(aValue));
            return c;
        }
        //<-----------------------------------------------------------------------------

        /**
         * @brief operator |
         * @param aLeft
         * @param aRight
         * @return
         */
        template <typename TEnum>
        CBitField<TEnum> operator|(TEnum const&aLeft, TEnum const&aRight)
        {
            return (CBitField<TEnum>(aLeft) | aRight);
        }
    }
}

#endif
