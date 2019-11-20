#ifndef __SHIRABE_BASE__STL_CONTAINER_HELPERS_H__
#define __SHIRABE_BASE__STL_CONTAINER_HELPERS_H__

#include "declaration.h"

namespace engine
{
    template<typename T>
    class CReverseIterationAdapter
    {
    public_constructors:
        CReverseIterationAdapter(T &aContainer)
            : mContainer(aContainer)
        {

        }

    public_methods:
        typename T::reverse_iterator begin()
        {
            return mContainer.rbegin();
        }

        typename T::reverse_iterator end()
        {
            return mContainer.rend();
        }

        typename T::const_reverse_iterator begin() const
        {
            return mContainer.rbegin();
        }

        typename T::const_reverse_iterator end() const
        {
            return mContainer.rend();
        }


    private_members:
        T &mContainer;
    };

    template<class TContainer>
    class CMapEmplacer
        : public std::iterator< std::output_iterator_tag, void, void, void, void >
    {

    public:
        typedef TContainer container_type;

        explicit CMapEmplacer(TContainer &aContainer)
            : mContainer(&aContainer)
        {}

        template<class T>
        CMapEmplacer<TContainer>& operator=(T &&t)
        {
            mContainer->emplace(std::forward<T>(t));
            return *this;
        }

        CMapEmplacer& operator*()     { return *this; }
        CMapEmplacer& operator++()    { return *this; }
        CMapEmplacer& operator++(int) { return *this; }

    protected_members:
        TContainer *mContainer;
    };

    template <class TContainer>
    inline CMapEmplacer<TContainer>
    getMapEmplacer(TContainer &aContainer)
    {
        return CMapEmplacer<TContainer>(aContainer);
    }
}
#endif // STL_CONTAINER_HELPERS_H
