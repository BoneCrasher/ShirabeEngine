#ifndef __SHIRABE_DX11_COMMON_H__
#define __SHIRABE_DX11_COMMON_H__

#define _USE_MATH_DEFINES_ // M_PI, etc...
#include <cmath>
#include <math.h>

#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

#define VEC_X(v) v.vector4_f32[0]
#define VEC_Y(v) v.vector4_f32[1]
#define VEC_Z(v) v.vector4_f32[2]
#define VEC_W(v) v.vector4_f32[3]

#include <dxgi.h>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>

#include "Linkage.h"

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