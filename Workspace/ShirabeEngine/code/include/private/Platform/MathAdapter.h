#ifndef __SHIRABE_MATHADAPTER_H__
#define __SHIRABE_MATHADAPTER_H__

#include "Platform/Platform.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"

#ifdef PLATFORM_WINDOWS
#include <DirectXMath.h>
#endif

namespace Platform {
	namespace Math {
		using namespace Engine::Math;

#ifdef PLATFORM_WINDOWS
		using namespace DirectX;
		const constexpr unsigned int SZ_VEC = 4 * sizeof(float);
		const constexpr unsigned int SZ_MAT = 4 * SZ_VEC;

		static inline bool XMVectorFromVector4D(const Vector4D& v, XMVECTOR& out) {
			return (memcpy_s(out.m128_f32, SZ_VEC, v.const_ptr(), SZ_VEC) == 0);
		}

		static inline bool Vector4DToXMVector(const XMVECTOR& v, Vector4D& out) {
			return (memcpy_s(out.ptr(), SZ_VEC, v.m128_f32, SZ_VEC) == 0);
		}

		static inline bool XMMatrixFromMatrix4x4(const Matrix4x4& m, XMMATRIX& out) {
			return (memcpy_s(out.r, SZ_MAT, m.const_ptr(), SZ_MAT) == 0);
		}

		static inline bool Matrix4x4FromXMMatrix(const XMMATRIX& m, Matrix4x4& out) {
			return (memcpy_s(out.ptr(), SZ_MAT, m.r, SZ_MAT) == 0);
		}

#endif

	}
}

#endif