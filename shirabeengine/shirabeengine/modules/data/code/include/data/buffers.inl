//
// Created by dottideveloper on 25.12.21.
//
#ifndef __SHIRABEDEVELOPMENT_GPURESOURCE_BUFFERS_INL_H__
#define __SHIRABEDEVELOPMENT_GPURESOURCE_BUFFERS_INL_H__

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
#define GPUDATABUFFERGENERIC_DECL \
    template <typename TUnderlyingData, EGpuResourceUpdatePolicy UpdatePolicy>

#define GPUDATABUFFERGENERIC_IMPL \
    CGpuDataBufferGeneric<TUnderlyingData, UpdatePolicy>
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
GPUDATABUFFERGENERIC_DECL
GPUDATABUFFERGENERIC_IMPL::CGpuDataBufferGeneric()
    : AGpuResourceBase<vulkan::RHIBufferResourceState_t>()
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
GPUDATABUFFERGENERIC_DECL
EEngineStatus GPUDATABUFFERGENERIC_IMPL::initRHIResource()
{
    Shared<CRHILayer> rhi = util::CEngineLocator::get().template locateEngineComponentByType<CRHILayer>();
    if(not rhi)
    {
        return EEngineStatus::RHILayerNotAvailable;
    }

    // Do something.

    return EEngineStatus::NotImplemented;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
GPUDATABUFFERGENERIC_DECL
EEngineStatus GPUDATABUFFERGENERIC_IMPL::releaseRHIResource()
{
    Shared<CRHILayer> rhi = util::CEngineLocator::get().template locateEngineComponentByType<CRHILayer>();
    if(not rhi)
    {
        return EEngineStatus::RHILayerNotAvailable;
    }

    return EEngineStatus::NotImplemented;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
GPUDATABUFFERGENERIC_DECL
EEngineStatus GPUDATABUFFERGENERIC_IMPL::updateRHIResource()
{
    Shared<CRHILayer> rhi = util::CEngineLocator::get().template locateEngineComponentByType<CRHILayer>();
    if(not rhi)
    {
        return EEngineStatus::RHILayerNotAvailable;
    }

    return EEngineStatus::NotImplemented;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
#undef GPUDATABUFFERGENERIC_IMPL
#undef GPUDATABUFFERGENERIC_DECL
//<-----------------------------------------------------------------------------

#endif //__SHIRABEDEVELOPMENT_GPURESOURCE_BUFFERS_INL_H__
