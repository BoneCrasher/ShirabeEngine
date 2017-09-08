#include "Renderer/DirectX/DX11/DX11Common.h"
#include "Renderer/DirectX/DX11/DX11DeviceCapabilities.h"

#include <functional>

namespace Engine {
	namespace DX {
		namespace _11 {
			DXGI_FORMAT DX11DeviceCapsHelper::convertFormatGAPI2DXGI(const GAPIFormat& fmt) {
				switch (fmt) {
				case GAPIFormat::RGBA_32_UINT:             return DXGI_FORMAT_R32G32B32A32_UINT;
				case GAPIFormat::RGBA_32_SINT:             return DXGI_FORMAT_R32G32B32A32_SINT;
				case GAPIFormat::RGBA_32_FLOAT:            return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case GAPIFormat::RGBA_32_TYPELESS:         return DXGI_FORMAT_R32G32B32A32_TYPELESS;
				case GAPIFormat::RGBA_16_UNORM:            return DXGI_FORMAT_R16G16B16A16_UNORM;
				case GAPIFormat::RGBA_16_SNORM:            return DXGI_FORMAT_R16G16B16A16_SNORM;
				case GAPIFormat::RGBA_16_UINT:             return DXGI_FORMAT_R16G16B16A16_UINT;
				case GAPIFormat::RGBA_16_SINT:             return DXGI_FORMAT_R16G16B16A16_SINT;
				case GAPIFormat::RGBA_16_FLOAT:            return DXGI_FORMAT_R16G16B16A16_FLOAT;
				case GAPIFormat::RGBA_16_TYPELESS:         return DXGI_FORMAT_R16G16B16A16_TYPELESS;
				case GAPIFormat::RGBA_8_UNORM:             return DXGI_FORMAT_R8G8B8A8_UNORM;
				case GAPIFormat::RGBA_8_UNORM_SRGB:        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				case GAPIFormat::RGBA_8_SNORM:             return DXGI_FORMAT_R8G8B8A8_SNORM;
				case GAPIFormat::RGBA_8_UINT:              return DXGI_FORMAT_R8G8B8A8_UINT;
				case GAPIFormat::RGBA_8_SINT:              return DXGI_FORMAT_R8G8B8A8_SINT;
				case GAPIFormat::RGBA_8_TYPELESS:          return DXGI_FORMAT_R8G8B8A8_TYPELESS;
				case GAPIFormat::RGB_32_UINT:              return DXGI_FORMAT_R32G32B32_UINT;
				case GAPIFormat::RGB_32_SINT:              return DXGI_FORMAT_R32G32B32_SINT;
				case GAPIFormat::RGB_32_FLOAT:             return DXGI_FORMAT_R32G32B32_FLOAT;
				case GAPIFormat::RGB_32_TYPELESS:          return DXGI_FORMAT_R32G32B32_TYPELESS; 
				case GAPIFormat::D24_UNORM_S8_UINT:        return DXGI_FORMAT_D24_UNORM_S8_UINT;
				case GAPIFormat::D32_FLOAT:                return DXGI_FORMAT_D32_FLOAT;
				case GAPIFormat::R32_FLOAT:                return DXGI_FORMAT_R32_FLOAT;
				case GAPIFormat::R32_UINT:                 return DXGI_FORMAT_R32_UINT;
				case GAPIFormat::R32_SINT:                 return DXGI_FORMAT_R32_SINT;
				case GAPIFormat::D32_FLOAT_S8X24_UINT:     return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
				case GAPIFormat::R32_FLOAT_X8X24_TYPELESS: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
				case GAPIFormat::BC1_UNORM:                return DXGI_FORMAT_BC1_UNORM;
				case GAPIFormat::BC1_UNORM_SRGB:           return DXGI_FORMAT_BC1_UNORM_SRGB;
				case GAPIFormat::BC1_TYPELESS:             return DXGI_FORMAT_BC1_TYPELESS;
				case GAPIFormat::BC2_UNORM:                return DXGI_FORMAT_BC2_UNORM;
				case GAPIFormat::BC2_UNORM_SRGB:           return DXGI_FORMAT_BC2_UNORM_SRGB;
				case GAPIFormat::BC2_TYPELESS:             return DXGI_FORMAT_BC2_TYPELESS;
				case GAPIFormat::BC3_UNORM:                return DXGI_FORMAT_BC3_UNORM;
				case GAPIFormat::BC3_UNORM_SRGB:           return DXGI_FORMAT_BC3_UNORM_SRGB;
				case GAPIFormat::BC3_TYPELESS:             return DXGI_FORMAT_BC3_TYPELESS;
				case GAPIFormat::BC4_UNORM:                return DXGI_FORMAT_BC4_UNORM;
				case GAPIFormat::BC4_SNORM:                return DXGI_FORMAT_BC4_SNORM;
				case GAPIFormat::BC4_TYPELESS:             return DXGI_FORMAT_BC4_TYPELESS;
				case GAPIFormat::BC5_UNORM:                return DXGI_FORMAT_BC5_UNORM;
				case GAPIFormat::BC5_SNORM:                return DXGI_FORMAT_BC5_SNORM;
				case GAPIFormat::BC5_TYPELESS:             return DXGI_FORMAT_BC5_TYPELESS;
				case GAPIFormat::BC6H_SF16:                return DXGI_FORMAT_BC6H_SF16;
				case GAPIFormat::BC6H_UF16:                return DXGI_FORMAT_BC6H_UF16;
				case GAPIFormat::BC6H_TYPELESS:            return DXGI_FORMAT_BC6H_TYPELESS;
				case GAPIFormat::BC7_UNORM:                return DXGI_FORMAT_BC7_UNORM;
				case GAPIFormat::BC7_UNORM_SRGB:           return DXGI_FORMAT_BC7_UNORM_SRGB;
				case GAPIFormat::BC7_TYPELESS:             return DXGI_FORMAT_BC7_TYPELESS;
				}
				return DXGI_FORMAT_UNKNOWN;
			}

			GAPIFormat DX11DeviceCapsHelper::convertFormatDXGI2GAPI(const DXGI_FORMAT& fmt) {
				switch (fmt) {
				case DXGI_FORMAT_R32G32B32A32_UINT:		   return GAPIFormat::RGBA_32_UINT;
				case DXGI_FORMAT_R32G32B32A32_SINT:		   return GAPIFormat::RGBA_32_SINT;
				case DXGI_FORMAT_R32G32B32A32_FLOAT:	   return GAPIFormat::RGBA_32_FLOAT;
				case DXGI_FORMAT_R32G32B32A32_TYPELESS:	   return GAPIFormat::RGBA_32_TYPELESS;
				case DXGI_FORMAT_R16G16B16A16_UNORM:	   return GAPIFormat::RGBA_16_UNORM;
				case DXGI_FORMAT_R16G16B16A16_SNORM:	   return GAPIFormat::RGBA_16_SNORM;
				case DXGI_FORMAT_R16G16B16A16_UINT:		   return GAPIFormat::RGBA_16_UINT;
				case DXGI_FORMAT_R16G16B16A16_SINT:		   return GAPIFormat::RGBA_16_SINT;
				case DXGI_FORMAT_R16G16B16A16_FLOAT:	   return GAPIFormat::RGBA_16_FLOAT;
				case DXGI_FORMAT_R16G16B16A16_TYPELESS:	   return GAPIFormat::RGBA_16_TYPELESS;
				case DXGI_FORMAT_R8G8B8A8_UNORM:		   return GAPIFormat::RGBA_8_UNORM;
				case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:	   return GAPIFormat::RGBA_8_UNORM_SRGB;
				case DXGI_FORMAT_R8G8B8A8_SNORM:		   return GAPIFormat::RGBA_8_SNORM;
				case DXGI_FORMAT_R8G8B8A8_UINT:			   return GAPIFormat::RGBA_8_UINT;
				case DXGI_FORMAT_R8G8B8A8_SINT:			   return GAPIFormat::RGBA_8_SINT;
				case DXGI_FORMAT_R8G8B8A8_TYPELESS:		   return GAPIFormat::RGBA_8_TYPELESS;
				case DXGI_FORMAT_R32G32B32_UINT:		   return GAPIFormat::RGB_32_UINT;
				case DXGI_FORMAT_R32G32B32_SINT:		   return GAPIFormat::RGB_32_SINT;
				case DXGI_FORMAT_R32G32B32_FLOAT:		   return GAPIFormat::RGB_32_FLOAT;
				case DXGI_FORMAT_R32G32B32_TYPELESS:	   return GAPIFormat::RGB_32_TYPELESS;
				case DXGI_FORMAT_D24_UNORM_S8_UINT:        return GAPIFormat::D24_UNORM_S8_UINT;
				case DXGI_FORMAT_D32_FLOAT:				   return GAPIFormat::D32_FLOAT;
				case DXGI_FORMAT_R32_FLOAT:				   return GAPIFormat::R32_FLOAT;
				case DXGI_FORMAT_R32_UINT:				   return GAPIFormat::R32_UINT;
				case DXGI_FORMAT_R32_SINT:				   return GAPIFormat::R32_SINT;
				case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:	   return GAPIFormat::D32_FLOAT_S8X24_UINT;
				case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return GAPIFormat::R32_FLOAT_X8X24_TYPELESS;
				case DXGI_FORMAT_BC1_UNORM:				   return GAPIFormat::BC1_UNORM;
				case DXGI_FORMAT_BC1_UNORM_SRGB:		   return GAPIFormat::BC1_UNORM_SRGB;
				case DXGI_FORMAT_BC1_TYPELESS:			   return GAPIFormat::BC1_TYPELESS;
				case DXGI_FORMAT_BC2_UNORM:				   return GAPIFormat::BC2_UNORM;
				case DXGI_FORMAT_BC2_UNORM_SRGB:		   return GAPIFormat::BC2_UNORM_SRGB;
				case DXGI_FORMAT_BC2_TYPELESS:			   return GAPIFormat::BC2_TYPELESS;
				case DXGI_FORMAT_BC3_UNORM:				   return GAPIFormat::BC3_UNORM;
				case DXGI_FORMAT_BC3_UNORM_SRGB:		   return GAPIFormat::BC3_UNORM_SRGB;
				case DXGI_FORMAT_BC3_TYPELESS:			   return GAPIFormat::BC3_TYPELESS;
				case DXGI_FORMAT_BC4_UNORM:				   return GAPIFormat::BC4_UNORM;
				case DXGI_FORMAT_BC4_SNORM:				   return GAPIFormat::BC4_SNORM;
				case DXGI_FORMAT_BC4_TYPELESS:			   return GAPIFormat::BC4_TYPELESS;
				case DXGI_FORMAT_BC5_UNORM:				   return GAPIFormat::BC5_UNORM;
				case DXGI_FORMAT_BC5_SNORM:				   return GAPIFormat::BC5_SNORM;
				case DXGI_FORMAT_BC5_TYPELESS:			   return GAPIFormat::BC5_TYPELESS;
				case DXGI_FORMAT_BC6H_SF16:				   return GAPIFormat::BC6H_SF16;
				case DXGI_FORMAT_BC6H_UF16:				   return GAPIFormat::BC6H_UF16;
				case DXGI_FORMAT_BC6H_TYPELESS:			   return GAPIFormat::BC6H_TYPELESS;
				case DXGI_FORMAT_BC7_UNORM:				   return GAPIFormat::BC7_UNORM;
				case DXGI_FORMAT_BC7_UNORM_SRGB:		   return GAPIFormat::BC7_UNORM_SRGB;
				case DXGI_FORMAT_BC7_TYPELESS:			   return GAPIFormat::BC7_TYPELESS;
				}
				return GAPIFormat::UNKNOWN;
			}			

			DeclareSharedPointerType(IDXGIFactory);
			DeclareSharedPointerType(IDXGIAdapter);
			DeclareSharedPointerType(IDXGIOutput);
			DeclareListType(IDXGIAdapterPtr, IDXGIAdapterPtr);
			
			EEngineStatus DX11DeviceCapsHelper::GetDeviceCapabilities(
				const GAPIFormat       &inRequiredBackBufferFormat,
				GAPIDeviceCapabilities *pOutCapabilities) {
				if (!pOutCapabilities)
					return EEngineStatus::NullPointer;

				DXGI_FORMAT inDXRequiredBackBufferFormat = convertFormatGAPI2DXGI(inRequiredBackBufferFormat);

				HRESULT res = S_OK;
				
				IDXGIFactoryPtr pFactory = nullptr;
				{ // Try to create a DXGIFactory for device caps determination.
					IDXGIFactory *tmp  = nullptr;
					res = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&tmp);
					if (FAILED(res)) {
						Log::Error(logTag(), "Unable to create DXGIFactory.");
						return EEngineStatus::DXGI_FactoryCreationFailed;
					}
					pFactory = MakeSharedPointerTypeCustomDeleter(tmp, DxResourceDeleter<IDXGIFactory>());
					// Reset to avoid side effects.
					res = S_OK;
				}
				
				{ // Try to retrieve all adapters (graphics cards) and their output (monitors) and modes.
					GAPIAdapterList outAdapters;
					UINT k=0;
					IDXGIAdapter *tmpAdapter = nullptr;
					while ((res = pFactory->EnumAdapters(k, &tmpAdapter)) != DXGI_ERROR_NOT_FOUND) {
						GAPIAdapter     outAdapter ={};
						IDXGIAdapterPtr pAdapter   = MakeSharedPointerTypeCustomDeleter(tmpAdapter, DxResourceDeleter<IDXGIAdapter>());

						// Get adapter information
						{
							DXGI_ADAPTER_DESC tmpAdapterDesc ={};
							res = pAdapter->GetDesc(&tmpAdapterDesc);
							if (FAILED(res)) {
								Log::Error(logTag(), String::format("Unable to fetch adapter description data for adapter %0", k));
								return EEngineStatus::DXGI_AdapterGetDescFailed;
							}

							GAPIAdapterDescription outAdapterDesc ={};
							outAdapterDesc._name               = String::toNarrowString(tmpAdapterDesc.Description);
							outAdapterDesc._dedicatedGPUMemory = tmpAdapterDesc.DedicatedVideoMemory;
							outAdapterDesc._dedicatedSysMemory = tmpAdapterDesc.DedicatedSystemMemory;

							outAdapter._description = outAdapterDesc;
						}

						// Enumerate outputs (monitors)
						GAPIOutputList outAdapterOutputs;
						UINT j=0;
						IDXGIOutput *tmpOutput = nullptr;
						while ((res = pAdapter->EnumOutputs(j, &tmpOutput)) != DXGI_ERROR_NOT_FOUND) {
							GAPIOutput outAdapterOutput ={};

							IDXGIOutputPtr pOutput = MakeSharedPointerTypeCustomDeleter(tmpOutput, DxResourceDeleter<IDXGIOutput>());

							// Get the number of output modes that match the required backbuffer format
							do {
								UINT nOutputModes = 0;
								DXGI_MODE_DESC *tmpOutputModes = nullptr;
								res = pOutput->GetDisplayModeList(inDXRequiredBackBufferFormat, DXGI_ENUM_MODES_INTERLACED, &nOutputModes, nullptr);
								if (!FAILED(res)) {
									// Now get 'em modes...
									tmpOutputModes = new DXGI_MODE_DESC[nOutputModes];
									res = pOutput->GetDisplayModeList(inDXRequiredBackBufferFormat, DXGI_ENUM_MODES_INTERLACED, &nOutputModes, tmpOutputModes);
									if (FAILED(res)) { // Did the data change again? This is too unstable... Abort gracefully.
										Log::Error(logTag(), String::format("Failed to determine the output modes of 'device %0|adapter %1'", k, j));
										
										if(outAdapters.size() == 1) 
											// Primary adapter, which should be the main graphics card with at least one output and mode.
											// Bail out...
											return EEngineStatus::DXGI_PrimaryAdapterGetOutputModesFailed;
									}

									outAdapterOutput._outputIndex = j;
									
									// Now process all modes and store them
									for (unsigned int i=0; i < nOutputModes; ++i) {
										GAPIOutputMode outAdapterOutputMode ={};

										DXGI_MODE_DESC desc = tmpOutputModes[i];
										outAdapterOutputMode._size        = Vec2Dui(desc.Width, desc.Height);
										outAdapterOutputMode._format      = convertFormatDXGI2GAPI(desc.Format);
										outAdapterOutputMode._refreshRate = Vec2Dui(desc.RefreshRate.Numerator, desc.RefreshRate.Denominator);

										outAdapterOutput._outputModes.push_back(outAdapterOutputMode);
									};

									// DON'T FORGET TO CLEAN UP UNMANAGED DATA.
									delete[] tmpOutputModes; tmpOutputModes = nullptr;
								}
							} while (res == DXGI_ERROR_MORE_DATA); // Repeat, if the mode data has changed immediately after querying the platform.

							outAdapterOutputs.push_back(outAdapterOutput);

							++j;
						}		
						// Check and report, if no output was found for the specific adapter. 
						// This however is not reason to drop out, UNLESS this is the primary adapter.
						if (FAILED(res) && outAdapterOutputs.empty()) {							
							if (outAdapters.size() < 1) {
								Log::Error(logTag(), String::format("Failed to get a valid output for adapter %0.", k));
								return EEngineStatus::DXGI_PrimaryAdapterGetOutputFailed;
							}
						}

						outAdapter._outputs = outAdapterOutputs;
						outAdapters.push_back(outAdapter);

						++k;
					}
					// Check, if we were able to fetch at least one adapter successfully.
					if (FAILED(res) && outAdapters.empty()) {
						Log::Error(logTag(), "Failed to determine a valid adapter.");
						return EEngineStatus::DXGI_FactoryGetAdapterFailed;
					}

					pOutCapabilities->_adapters = outAdapters;
				}

				return EEngineStatus::Ok;
			}
		}
	}
}