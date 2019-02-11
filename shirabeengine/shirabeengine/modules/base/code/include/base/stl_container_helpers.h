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

        typename T::reverse_const_iterator begin() const
        {
            return mContainer.rbegin();
        }

        typename T::reverse_const_iterator end() const
        {
            return mContainer.rend();
        }


    private_members:
        T &mContainer;
    };
}
#endif // STL_CONTAINER_HELPERS_H
