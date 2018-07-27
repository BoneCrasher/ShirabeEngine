namespace Engine {

  //EEngineStatus
  //  DX11ResourceTaskBackend::
  //  creationTask(
  //    SwapChain::CreationRequest   const&request,
  //    ResolvedDependencyCollection const&resolvedDependencies,
  //    ResourceTaskFn_t                  &outTask)
  //{
  //  EEngineStatus status = EEngineStatus::Ok;

  //  SwapChain::Descriptor const&desc = request.resourceDescriptor();

  //  DXGI_SWAP_CHAIN_DESC swapChainDescription ={};
  //  // Setup backbuffer
  //  swapChainDescription.BufferCount                 = desc.backBufferCount;
  //  swapChainDescription.BufferDesc.Width            = desc.texture.textureInfo.width;
  //  swapChainDescription.BufferDesc.Height           = desc.texture.textureInfo.height;
  //  swapChainDescription.BufferDesc.Format           = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.texture.textureInfo.format);
  //  swapChainDescription.BufferDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
  //  swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

  //  if(desc.vsyncEnabled) {
  //    swapChainDescription.BufferDesc.RefreshRate.Numerator   = desc.refreshRateNumerator;
  //    swapChainDescription.BufferDesc.RefreshRate.Denominator = desc.refreshRateDenominator;
  //  }
  //  else {
  //    swapChainDescription.BufferDesc.RefreshRate.Numerator   = 0;
  //    swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
  //  }
  //  swapChainDescription.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;

  //  // Multisampling: For now deactivate.
  //  swapChainDescription.SampleDesc.Count   = 1;
  //  swapChainDescription.SampleDesc.Quality = 0;
  //  // Fullscreen?
  //  swapChainDescription.Windowed = true; // Do not set immediately! desc.fullscreen;
  //                              // Attach to window
  //  swapChainDescription.OutputWindow = static_cast<HWND>(reinterpret_cast<void *>(desc.windowHandle));
  //  swapChainDescription.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;

  //  outTask = [this, &swapChainDescription] () -> GFXAPIResourceHandleAssignment
  //  {
  //    IDXGISwapChain *pSwapChainUnmanaged = nullptr;

  //    HRESULT hres = m_dx11Environment->getDxgiFactory()->CreateSwapChain(m_dx11Environment->getDevice().get(), &swapChainDescription, &pSwapChainUnmanaged);
  //    Platform::Windows::HandleWindowsError(hres, EEngineStatus::DXGI_SwapChainCreationFailed, "Failed to create swap chain resource.");

  //    GFXAPIResourceHandleAssignment assignment ={};
  //    assignment.publicHandle   = reinterpret_cast<GFXAPIResourceHandle_t>(pSwapChainUnmanaged); // Just abuse the pointer target address of the handle...
  //    assignment.internalHandle = MakeDxSharedPointer(pSwapChainUnmanaged);

  //    return assignment;
  //  };

  //  return status;
  //}

  //EEngineStatus
  //  DX11ResourceTaskBackend::
  //  updateTask(
  //    SwapChain::UpdateRequest     const&request,
  //    ResolvedDependencyCollection const&resolvedDependencies,
  //    ResourceTaskFn_t                  &outTask)
  //{
  //  EEngineStatus status = EEngineStatus::Ok;

  //  return status;
  //}

  //EEngineStatus
  //  DX11ResourceTaskBackend::
  //  destructionTask(
  //    SwapChain::DestructionRequest const&request,
  //    ResolvedDependencyCollection  const&resolvedDependencies,
  //    ResourceTaskFn_t                   &outTask)
  //{
  //  EEngineStatus status = EEngineStatus::Ok;

  //  return status;
  //}

  //EEngineStatus
  //  DX11ResourceTaskBackend::
  //  queryTask(
  //    SwapChain::Query const&request,
  //    ResourceTaskFn_t      &outTask)
  //{
  //  EEngineStatus status = EEngineStatus::Ok;

  //  return status;
  //}

  //EEngineStatus
  //  DX11ResourceTaskBackend::
  //  creationTask(
  //    SwapChainBuffer::CreationRequest const&request,
  //    ResolvedDependencyCollection     const&resolvedDependencies,
  //    ResourceTaskFn_t                      &outTask)
  //{
  //  EEngineStatus status = EEngineStatus::Ok;

  //  // Based on the provided descriptor, we created "n" backbuffers. 
  //  // Create resource handles for them.

  //  SwapChainBuffer::Descriptor const&desc = request.resourceDescriptor();

  //  CStdSharedPtr_t<void> privateDependencyHandle = resolvedDependencies.at(request.swapChainId());
  //  if(!privateDependencyHandle) {
  //    HandleEngineStatusError(EEngineStatus::DXDevice_CreateSwapChainBuffer_Failed, "Failed to create SRV due to missing dependency.");
  //  }

  //  CStdSharedPtr_t<IDXGISwapChain> swapChain = std::static_pointer_cast<IDXGISwapChain>(privateDependencyHandle);

  //  DXGI_SWAP_CHAIN_DESC  swapChainDescriptor ={};
  //  ID3D11Texture2D      *backBufferUnmanaged = nullptr;

  //  outTask = [&, this] () -> GFXAPIResourceHandleAssignment
  //  {
  //    HRESULT hres = swapChain->GetBuffer(desc.backBufferIndex, __uuidof(ID3D11Texture2D), (void **)&backBufferUnmanaged);
  //    Platform::Windows::HandleWindowsError(hres, EEngineStatus::DXDevice_CreateSwapChainBuffer_Failed, "Failed to acquire and wrap back buffer pointer.");

  //    GFXAPIResourceHandleAssignment assignment ={};
  //    assignment.publicHandle   = reinterpret_cast<GFXAPIResourceHandle_t>(backBufferUnmanaged); // Just abuse the pointer target address of the handle...
  //    assignment.internalHandle = MakeDxSharedPointer(backBufferUnmanaged);

  //    return assignment;
  //  };

  //  return status;
  //}

  //EEngineStatus
  //  DX11ResourceTaskBackend::
  //  updateTask(
  //    SwapChainBuffer::UpdateRequest     const&request,
  //    ResolvedDependencyCollection       const&resolvedDependencies,
  //    ResourceTaskFn_t                        &outTask)
  //{
  //  EEngineStatus status = EEngineStatus::Ok;

  //  return status;
  //}

  //EEngineStatus
  //  DX11ResourceTaskBackend::
  //  destructionTask(
  //    SwapChainBuffer::DestructionRequest const&request,
  //    ResolvedDependencyCollection        const&resolvedDependencies,
  //    ResourceTaskFn_t                         &outTask)
  //{
  //  EEngineStatus status = EEngineStatus::Ok;

  //  return status;
  //}

  //EEngineStatus
  //  DX11ResourceTaskBackend::
  //  queryTask(
  //    SwapChainBuffer::Query const&request,
  //    ResourceTaskFn_t            &outTask)
  //{
  //  EEngineStatus status = EEngineStatus::Ok;

  //  return status;
  //}


}

