#ifndef __SHIRABE_COMPONENT_BASE_H__
#define __SHIRABE_COMPONENT_BASE_H__

#include <string>
#include <base/declaration.h>
#include <core/enginetypehelper.h>

namespace engine::ecws
{
    class CEntity;

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


    SHIRABE_DECLARE_LIST_OF_TYPE(Shared<class CComponentBase>, Component);
    SHIRABE_DECLARE_MAP_OF_TYPES(std::type_index, CBoundedCollection<Shared<class CComponentBase>>, Component);

	class CComponentBase
	{
	    friend class CEntity;

	public_constructors:
	    SHIRABE_INLINE
	    CComponentBase(std::string const &aName)
	        : mName(aName)
        {}

    public_destructors:
		SHIRABE_INLINE
	    ~CComponentBase()
        {
	        mParentEntity = nullptr;
        };

	public_methods:
	    [[nodiscard]]
	    SHIRABE_INLINE
	    std::string const &name() const { return mName; }

        /**
         * Set the component name.
         *
         * @param aName The new name of the component.
         */
        SHIRABE_INLINE void setName(std::string const& aName) { mName = aName; }

        /**
         * Update the component with an optionally usable timer component.
         *
         * @param aTimer A timer providing the currently elapsed time.
         * @return       Return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        EEngineStatus update(CTimer const &aTimer);

        /**
         * Add a component to the internal component collection.
         *
         * @param aComponent The component to add to the entity instance.
         * @return           EEngineStatus::Ok, if successful. An error code otherwise.
         */
        template <typename TComponent>
        EEngineStatus addComponent(Shared<TComponent> const &aComponent);

        /**
         * Check, wether a specific component type is available in the internal component collection.
         *
         * @tparam TComponent The type of component to check for.
         */
        template<typename TComponent>
        [[nodiscard]]
        bool hasComponentOfType() const;

        /**
         * Fetch a list of components of specific type TComponent, if any.
         *
         * @tparam TComponent The type of component to enumerate.
         */
        template <typename TComponent>
        CBoundedCollection<Shared<TComponent>> const getTypedComponentsOfType() const;

        /**
         * Remove a component from the internal component collection.
         *
         * @param aComponent The component to add to the entity instance.
         * @return           EEngineStatus::Ok, if successful. An error code otherwise.
         */
        template <typename TComponent>
        EEngineStatus removeComponent(Shared<TComponent> const &aComponent);

    protected_methods:
	    SHIRABE_INLINE
        void setParentEntity(class CEntity const *const aEntity)
        {
	        mParentEntity = aEntity;
        }

	    [[nodiscard]]
	    SHIRABE_INLINE
	    CEntity const *const getParentEntity() const
        {
	        return mParentEntity;
        }

	private_members:
	    std::string mName;
	};
}
#endif
