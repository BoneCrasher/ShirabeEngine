#ifndef __SHIRABE_DX11_COMMON_H__
#define __SHIRABE_DX11_COMMON_H__

#include <d3d.h>
#include <d3d11.h>

#include "DX11Linkage.h"
#include "DX11Types.h"

namespace Engine {
	namespace DX {
		namespace _11 {

			template <typename T>
			static void DxRelease(T** pDxRes) {
				if (pDxRes && *pDxRes)
					(*pDxRes)->Release();

				*pDxRes = nullptr;
			}

			template <typename T>
			struct DxResourceDeleter {
				void operator() (T* pDxRes) {
					DxRelease(&pDxRes);
				}
			};

		}
	}
}

#endif