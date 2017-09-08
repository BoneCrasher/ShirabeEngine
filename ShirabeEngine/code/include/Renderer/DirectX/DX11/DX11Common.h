#ifndef __SHIRABE_DX11_COMMON_H__
#define __SHIRABE_DX11_COMMON_H__

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