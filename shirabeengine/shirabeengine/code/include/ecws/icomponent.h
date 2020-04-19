#ifndef __SHIRABE_COMPONENT_H__
#define __SHIRABE_COMPONENT_H__

#include <typeindex>
#include <core/enginetypehelper.h>
#include <core/enginestatus.h>
#include <core/benchmarking/timer/timer.h>

namespace engine::ecws
{
    class IComponent;

    template <typename TElement>
    class CBoundedCollection
    {
    public_typedefs:
        typedef typename Vector<TElement>::value_type value_type;

    public_constructors:
        SHIRABE_INLINE
        CBoundedCollection()
            : CBoundedCollection(0)
        {}

        SHIRABE_INLINE
        explicit CBoundedCollection(std::size_t const &aMaxElements)
            : mMaxElements(aMaxElements)
            , mElements()
        {}

        SHIRABE_INLINE CBoundedCollection(CBoundedCollection<TElement> const &aOther)
            : mMaxElements(aOther.mMaxElements)
            , mElements   (aOther.mElements)
        {}

        SHIRABE_INLINE CBoundedCollection(CBoundedCollection<TElement> &&aOther) noexcept
                : mMaxElements(aOther.mMaxElements)
                , mElements   (std::move(aOther.mElements))
        {}

        SHIRABE_INLINE
        CBoundedCollection<TElement> &operator=(CBoundedCollection<TElement> const &aOther)
        {
            mMaxElements = aOther.mMaxElements;
            mElements    = aOther.mElements;

            return *this;
        }

        [[nodiscard]]
        SHIRABE_INLINE
        std::size_t const capacity() const
        {
            return mMaxElements;
        }

        [[nodiscard]]
        SHIRABE_INLINE
        bool const empty() const
        {
            return mElements.empty();
        }

        template <typename TOtherElementType>
        SHIRABE_INLINE
        void assign(CBoundedCollection<TOtherElementType> const &aOther)
        {
            mElements.clear();
            mMaxElements = aOther.capacity();

            auto const converter = [] (TOtherElementType const &input) -> TElement
            {
                return std::dynamic_pointer_cast<typename TElement::element_type>(input);
            };

            std::transform(aOther.cbegin()
                    , aOther.cend()
                    , std::back_inserter(mElements)
                    , converter);
        }

        SHIRABE_INLINE
        EEngineStatus add(TElement const &aElement)
        {
            if(mElements.size() >= mMaxElements)
            {
                return EEngineStatus::Error;
            }

            if(mElements.end() != std::find(mElements.begin(), mElements.end(), aElement))
            {
                return EEngineStatus::Error;
            }

            mElements.push_back(aElement);
            return EEngineStatus::Ok;
        }

        SHIRABE_INLINE
        void remove(TElement const &aElement)
        {
            mElements.erase(aElement);
        }

        SHIRABE_INLINE
        bool contains(TElement const &aElement)
        {
            return (mElements.end() != std::find(mElements.cbegin(), mElements.cend(), aElement));
        }

        SHIRABE_INLINE
        void push_back(TElement const &aElement)
        {
            add(aElement);
        }

        SHIRABE_INLINE
        typename Vector<TElement>::iterator begin() { return mElements.begin(); }

        SHIRABE_INLINE
        typename Vector<TElement>::iterator end() { return mElements.end(); }

        SHIRABE_INLINE
        typename Vector<TElement>::const_iterator cbegin() const { return mElements.begin(); }

        SHIRABE_INLINE
        typename Vector<TElement>::const_iterator cend() const { return mElements.end(); }

        SHIRABE_INLINE
        Vector<TElement> const data() const { return mElements; }

    private_members:
        std::size_t       mMaxElements;
        Vector<TElement>  mElements;
    };

    /**
     * The IComponent interface describes the basic capabilitites of all system components.
     */
    class IComponent
    {
        SHIRABE_DECLARE_INTERFACE(IComponent);

    public_api:
        /**
         * Return the name of the component.
         *
         * @return See brief.
         */
         [[nodiscard]]
        virtual std::string const &name() const = 0;

        /**
         * Update the component with an optionally usable timer component.
         *
         * @param aTimer A timer providing the currently elapsed time.
         * @return       Return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        virtual EEngineStatus update(CTimer const &aTimer) = 0;

    protected_api:
        virtual void setParentEntity(class CEntity const *const aEntity) = 0;
    };

    SHIRABE_DECLARE_LIST_OF_TYPE(Shared<IComponent>, IComponent);
    SHIRABE_DECLARE_MAP_OF_TYPES(std::type_index, CBoundedCollection<Shared < IComponent>>, IComponent);
}

#endif
