#include "math/matrix.h"

namespace Engine
{
    namespace Math
    {

		// Calculates the determinante of the 2x2 matrix.
        CMatrix2x2::value_type SMMatrixDeterminant(CMatrix2x2 const &aMatrix)
		{
            CMatrix2x2::value_type det = 0;

            __shirabe_math__matrix_determinant_fishrule<CMatrix2x2::value_type>(aMatrix.const_ptr(), &det);

			return det;
		}
		// Calculates the determinante of the 3x3 matrix.
        CMatrix3x3::value_type SMMatrixDeterminant(CMatrix3x3 const &aMatrix)
		{
            CMatrix3x3::value_type det = 0;

            __shirabe_math__matrix_determinant_sarrus<CMatrix3x3::value_type>(aMatrix.const_ptr(), &det);

			return det;
		}
		// Calculates the determinante of the 4x4 matrix.
        CMatrix4x4::value_type SMMatrixDeterminant(CMatrix4x4 const &aMatrix)
		{
            CMatrix4x4::value_type det = 0;

            __shirabe_math__matrix_determinant_gauss_jordan<4, 4, CMatrix4x4::value_type>(aMatrix.const_ptr(), &det);

			return det;
		}

		// Returns a transposed copy of the passed 2x2 matrix.
        CMatrix2x2 SMMatrixTranspose(CMatrix2x2 const &aMatrix)
		{
            CMatrix2x2 transposed;

            __shirabe_math__matrix_transpose<2, 2, CMatrix2x2::value_type>(aMatrix.const_ptr(), transposed.ptr());

			return transposed;
		}
		// Returns a transposed copy of the passed 3x3 matrix.
        CMatrix3x3 SMMatrixTranspose(CMatrix3x3 const &aMatrix)
		{
            CMatrix3x3 transposed;

            __shirabe_math__matrix_transpose<3, 3, CMatrix3x3::value_type>(aMatrix.const_ptr(), transposed.ptr());

			return transposed;
		}
		// Returns a transposed copy of the passed 4x4 matrix.
        CMatrix4x4 SMMatrixTranspose(CMatrix4x4 const &aMatrix)
		{
            CMatrix4x4 transposed;

            __shirabe_math__matrix_transpose<4, 4, CMatrix4x4::value_type>(aMatrix.const_ptr(), transposed.ptr());

			return transposed;
		}

        CMatrix2x2 SMMatrixMultiply(CMatrix2x2 const &aLHS,CMatrix2x2 const &aRHS)
		{
            CMatrix2x2 product;

            __shirabe_math__matrix_multiply<CMatrix2x2::value_type, 2, 2, 2>(aLHS.const_ptr(), aRHS.const_ptr(), product.ptr());

			return product;
		}
        CMatrix3x3 SMMatrixMultiply(CMatrix3x3 const &aLHS,CMatrix3x3 const &aRHS)
		{
            CMatrix3x3 product;

            __shirabe_math__matrix_multiply<CMatrix3x3::value_type, 3, 3, 3>(aLHS.const_ptr(), aRHS.const_ptr(), product.ptr());

			return product;
		}
        CMatrix4x4 SMMatrixMultiply(CMatrix4x4 const &aLHS,CMatrix4x4 const &aRHS)
		{
            CMatrix4x4 product;

            __shirabe_math__matrix_multiply<CMatrix4x4::value_type, 4, 4, 4>(aLHS.const_ptr(), aRHS.const_ptr(), product.ptr());

			return product;
		}
	}
}
