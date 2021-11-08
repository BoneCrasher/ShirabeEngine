#ifndef __SHIRABE_COMPONENT_BASE_H__
#define __SHIRABE_COMPONENT_BASE_H__

#include <string>
#include <typeindex>
#include <base/declaration.h>
#include <core/enginestatus.h>
#include <core/enginetypehelper.h>

namespace engine::ecws
{
    template <typename TElement>
    class CBoundedCollection
    {
    public_typedefs:
        typedef typename Vector<TElement>::value_type value_type;

    public_constructors:
        SHIRABE_INLINE
        explicit CBoundedCollection()
            : mMaxElements(0)
            , mElements()
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

    struct SComponentEntry
    {
        std::type_index              typeIndex;
        Shared<class CComponentBase> component;

        template <typename TComponent>
        explicit SComponentEntry(Shared<TComponent> aComponent)
            : typeIndex(typeid(TComponent))
            , component(std::move(aComponent))
        { }
    };

    SHIRABE_DECLARE_LIST_OF_TYPE(SComponentEntry, Component);

	class CComponentBase
	{
	public_constructors:
	    explicit CComponentBase(std::string aName);

    public_destructors:
	    virtual ~CComponentBase() = default;

	public_methods:
	    [[nodiscard]]
	    SHIRABE_INLINE std::string const &name() const { return mName; }

        /**
         * Set the component name.
         *
         * @param aName The new name of the component.
         */
        SHIRABE_INLINE void setName(std::string const& aName) { mName = aName; }

        /**
         * Initialize this component and make it ready for use.
         *
         * @return EEngineStatus::Ok, if successful, an error code otherwise.
         */
        virtual EEngineStatus initialize();

        /**
         * Cleanup and deinitialize this components.
         * Will deinitialize and detach all child components!
         *
         * @return EEngineStatus::Ok, if successful, an error code otherwise.
         */
        virtual EEngineStatus deinitialize();

        /**
         * Update the component with an optionally usable timer component.
         *
         * @param aTimer A timer providing the currently elapsed time.
         * @return       Return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        virtual EEngineStatus update(class CTimer const &aTimer);

        /**
         * Add a component to the internal component collection.
         *
         * @param aComponent The component to add to the entity instance.
         * @return           EEngineStatus::Ok, if successful. An error code otherwise.
         */
        template <typename TComponent>
        SHIRABE_INLINE EEngineStatus addComponent(Shared<TComponent> aComponent)
        {
            SComponentEntry entry(aComponent);
            mChildren.push_back(entry);
            return EEngineStatus::Ok;
        }

        /**
         * Check, wether a specific component type is available in the internal component collection.
         *
         * @tparam TComponent The type of component to check for.
         */
        template<typename TComponent>
        [[nodiscard]] SHIRABE_INLINE
        bool hasComponentOfType() const
        {
            std::type_index const typeIndex = typeid(TComponent);

            auto const pred = [&] (SComponentEntry const &r) -> bool
            {
                return r.typeIndex == typeIndex;
            };

            return std::any_of(mChildren.begin(), mChildren.end(), pred);
        }

        /**
         * Fetch a list of components of specific type TComponent, if any.
         *
         * @tparam TComponent The type of component to enumerate.
         */
        template <typename TComponent>
        CBoundedCollection<Weak<TComponent>> getTypedComponentsOfType(bool const aRecurse = true) const
        {
            CBoundedCollection<Weak<TComponent>> matches;
            getTypedComponentsOfTypeImpl<TComponent>(matches, aRecurse);
            return matches;
        }

        /**
         * Remove a component from the internal component collection.
         *
         * @param aComponent The component to add to the entity instance.
         * @return           EEngineStatus::Ok, if successful. An error code otherwise.
         */
        template <typename TComponent>
        EEngineStatus removeComponent(Weak<TComponent> const &aComponent)
        {
            auto const pred = [&] (SComponentEntry const &r) -> bool
            {
                return r.component == aComponent;
            };

            return std::remove_if(mChildren.begin(), mChildren.end(), pred);
        }

    protected_methods:
	    void setParent(Weak<CComponentBase> aEntity);

	    [[nodiscard]]
	    SHIRABE_INLINE
	    Weak<CComponentBase> parentComponent() const { return mParent; }


        /**
         * Fetch a list of components of specific type TComponent, if any.
         *
         * @tparam TComponent The type of component to enumerate.
         */
        template <typename TComponent>
        void getTypedComponentsOfTypeImpl(CBoundedCollection<Weak<TComponent>> &aOutComponents, bool const aRecurse) const
        {
            std::type_index const typeIndex = typeid(TComponent);
            for(auto const &child : mChildren)
            {
                if(child.typeIndex == typeIndex)
                {
                    aOutComponents.push_back(std::static_pointer_cast<TComponent>(child.component));
                }

                if(aRecurse)
                {
                    child.component->template getTypedComponentsOfTypeImpl<TComponent>(aOutComponents, aRecurse);
                }
            }
        }

	private_members:
	    std::string          mName;
	    Weak<CComponentBase> mParent;
	    ComponentList        mChildren;
	};
}
#endif
