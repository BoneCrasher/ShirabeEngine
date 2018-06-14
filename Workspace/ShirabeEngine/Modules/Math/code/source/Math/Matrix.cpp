#include "Math/Matrix.h"

namespace Engine {
	namespace Math {

		// Calculates the determinante of the 2x2 matrix.
		Matrix2x2::value_type SMMatrixDeterminant( const Matrix2x2& matrix)
		{
			Matrix2x2::value_type det = 0;

			__shirabe_math__matrix_determinant_fishrule<Matrix2x2::value_type>(matrix.const_ptr(), &det);

			return det;
		}
		// Calculates the determinante of the 3x3 matrix.
		Matrix3x3::value_type SMMatrixDeterminant( const Matrix3x3& matrix)
		{
			Matrix3x3::value_type det = 0;

			__shirabe_math__matrix_determinant_sarrus<Matrix3x3::value_type>(matrix.const_ptr(), &det);

			return det;
		}
		// Calculates the determinante of the 4x4 matrix.
		Matrix4x4::value_type SMMatrixDeterminant( const Matrix4x4& matrix)
		{
			Matrix4x4::value_type det = 0;

			__shirabe_math__matrix_determinant_gauss_jordan<4, 4, Matrix4x4::value_type>(matrix.const_ptr(), &det);

			return det;
		}

		// Returns a transposed copy of the passed 2x2 matrix.
		Matrix2x2 SMMatrixTranspose( const Matrix2x2& matrix)
		{
			Matrix2x2 transposed;

			__shirabe_math__matrix_transpose<2, 2, Matrix2x2::value_type>(matrix.const_ptr(), transposed.ptr());

			return transposed;
		}
		// Returns a transposed copy of the passed 3x3 matrix.
		Matrix3x3 SMMatrixTranspose( const Matrix3x3& matrix)
		{
			Matrix3x3 transposed;

			__shirabe_math__matrix_transpose<3, 3, Matrix3x3::value_type>(matrix.const_ptr(), transposed.ptr());

			return transposed;
		}
		// Returns a transposed copy of the passed 4x4 matrix.
		Matrix4x4 SMMatrixTranspose( const Matrix4x4& matrix)
		{
			Matrix4x4 transposed;

			__shirabe_math__matrix_transpose<4, 4, Matrix4x4::value_type>(matrix.const_ptr(), transposed.ptr());

			return transposed;
		}

		Matrix2x2 SMMatrixMultiply( const Matrix2x2& l,  const Matrix2x2& r)
		{
			Matrix2x2 product;

			__shirabe_math__matrix_multiply<Matrix2x2::value_type, 2, 2, 2>(l.const_ptr(), r.const_ptr(), product.ptr());

			return product;
		}
		Matrix3x3 SMMatrixMultiply( const Matrix3x3& l,  const Matrix3x3& r)
		{
			Matrix3x3 product;

			__shirabe_math__matrix_multiply<Matrix3x3::value_type, 3, 3, 3>(l.const_ptr(), r.const_ptr(), product.ptr());

			return product;
		}
		Matrix4x4 SMMatrixMultiply( const Matrix4x4& l,  const Matrix4x4& r)
		{
			Matrix4x4 product;

			__shirabe_math__matrix_multiply<Matrix4x4::value_type, 4, 4, 4>(l.const_ptr(), r.const_ptr(), product.ptr());

			return product;
		}
	}
}