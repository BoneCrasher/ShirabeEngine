//
// Created by dottideveloper on 25.12.21.
//
#ifndef __SHIRABEDEVELOPMENT_GPURESOURCE_BASE_INL_H__
#define __SHIRABEDEVELOPMENT_GPURESOURCE_BASE_INL_H__

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
#define GPURESOURCEBASE_DECL \
        template <typename TRHIResource>

#define GPURESOURCEBASE_IMPL \
        AGpuResourceBase<TRHIResource>
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
GPURESOURCEBASE_DECL
GPURESOURCEBASE_IMPL::AGpuResourceBase()
    : mRHIResourceDescriptor()
    , mRHIResourceState(nullptr)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
// 
//<-----------------------------------------------------------------------------
#undef GPURESOURCEBASE_IMPL
#undef GPURESOURCEBASE_DECL
//<-----------------------------------------------------------------------------

#endif //__SHIRABEDEVELOPMENT_GPURESOURCE_BASE_INL_H__
