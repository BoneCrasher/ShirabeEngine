#ifndef __SHIRABE_SHIRABE_MATH_MATRIX_OPS_H__
#define __SHIRABE_SHIRABE_MATH_MATRIX_OPS_H__

#include "Platform/Platform.h"

namespace Engine {
	namespace Math {
		namespace PRIVATE {

#if defined(PLATFORM_WINDOWS)
#pragma warning(push)
			// Disable C4244 for warnings on multiplication of size_t with float/double
#pragma warning(disable:4244)
#endif
			// Implementations for the elemental row transformations.
			//
			// ElemT: Switch the i-th row with the j-th row
			// ElemR: Add the a-fold of the j-th row to the i-th row.
			// ElemS: Scale the i-th row by the factor lambda


			template <size_t rows, size_t cols, typename val_type>
			// Multiply the i-th row with lambda
			inline void __shirabe_math__matrix_elemental_row_transform_S(
				val_type       *pMat,
				size_t          i,
				const val_type  lambda,
				val_type       *pInv = NULL)
			{				
				val_type *ptr       = pMat;
				val_type *inv_ptr   = pInv;
				size_t    byte_size = cols * sizeof(val_type);
				size_t    i_off     = (i * cols);
				
				for (size_t i = 0; i < rows; ++i) {
					ptr[i_off + i] *= lambda;
					if (inv_ptr != NULL)
						inv_ptr[i_off + i] *= lambda;
				}
			}

			template <size_t rows, size_t cols, typename val_type>
			// Add the a-fold of the j-th row to the i-th
			inline void __shirabe_math__matrix_elemental_row_transform_R(
				val_type       *pMat,
				size_t          i,
				size_t          j,
				const val_type  a,
				val_type       *pInv = NULL)
			{
				val_type *ptr     = pMat;
				val_type *inv_ptr = pInv;
				val_type  tmp     = 0;
				size_t    i_off   = (i * cols);
				size_t    j_off   = (j * cols);

				for (size_t i = 0; i < rows; ++i) {
					ptr[i_off + i] += (ptr[j_off + i] * a);
					if (inv_ptr != NULL)
						inv_ptr[i_off + i] += (inv_ptr[j_off + i] * a);
				}
			}

			template <size_t rows, size_t cols, typename val_type>
			// Switch the i-th row with the j-th
			inline void __shirabe_math__matrix_elemental_row_transform_T(
				val_type *pMat, 
				size_t    i,
				size_t    j)
			{
				if (i == j)
					return;

				val_type *ptr       = pMat;
				size_t    byte_size = cols * sizeof(val_type);
				size_t    i_off     = (i * cols);
				size_t    j_off     = (j * cols);
				val_type  tmp[cols];

				memcpy_s(tmp, byte_size, (ptr + i_off), byte_size);
				memcpy_s((ptr + i_off), byte_size, (ptr + j_off), byte_size);
				memcpy_s((ptr + j_off), byte_size, tmp, byte_size);
			}

			template <typename val_type, size_t m, size_t s, size_t n>
			// Multiplies this m*s-matrix with another s*n-matrix passed.
			// The multiplication will return a m*n-product-matrix.
			inline void __shirabe_math__matrix_multiply(
				const val_type *l, 
				const val_type *r, 
				val_type       *productPtr)
			{
				// Implement asserts!

				// Get immediate matrix value storage pointers for both matrices.
				const val_type *l_ptr = l;
				const val_type *r_ptr = r;

				// Get immediate matrix value storage pointer for the result matrix.
				val_type *val_ptr     = NULL;
				val_type *product_ptr = productPtr;

				// Algorithm:
				// -> c[i, j] = a[i, s]*b[s, j]
				// Since the matrix buffers are plain blocks of m*n*sizeof(value_type) bytes
				// data access must be offset by using iteration indices!

				for (size_t i = 0; i < m; ++i) {
					for (size_t j = 0; j < n; ++j) {
						val_ptr = (product_ptr + (i*n + j));
						*val_ptr = 0;

						for (size_t sidx = 0; sidx < s; ++sidx)
							*val_ptr += (l_ptr[i*s + sidx] * r_ptr[sidx*n + j]);
					}
				}

				l_ptr = r_ptr = val_ptr = product_ptr = NULL;
			}

			template <size_t m, size_t n, typename val_type>
			// Return a transposed copy of the matrix passed!
			// The original matrix remains unchanged.
			inline void __shirabe_math__matrix_transpose(
				const val_type *pMat, 
				val_type       *transposedPtr)
			{
				/* a11 a12 ... a1n      a11 a21 ... am1
				*  a21 a22 ... a2n  ->  a12 a22 ... am2
				*  ... ... ... ...      ... ... ... ...
				*  am1 am2 ... amn      a1n a2n ... amn
				*/
				val_type *transpPtr = transposedPtr;
				const val_type *ptr = pMat;

				for (size_t i = 0; i < m; ++i) {
					for (size_t j = 0; j < n; ++j) {
						transpPtr[j*n + i] = ptr[j + i*n];
					}
				}
			}


			template <typename val_type>
			// Get the minor in m_mat for the element a(i, j).
			inline void __shirabe_math__matrix_get_minor(
				const val_type  *pMat,
				const size_t     m,
				const size_t     n,
				const size_t     i,
				const size_t     j,
				val_type       **pMinor,
				size_t          *pMinor_m,
				size_t          *pMinor_n)
			{
				if (m < 2 || n < 2)
					return;

				/*
				* a11 ... a1n                             a22 ... a2n
				* ... ... ...  --> i = j = 1 --> minor =  ... ... ...
				* am1 ... amn                             am2 ... amn
				*/

				if (*pMinor == NULL)
					// Minor dimensionNb are each reduced by 1!
					*pMinor = new val_type[(m - 1) * (n - 1)];

				*pMinor_m = m - 1;
				*pMinor_n = n - 1;

				size_t i_off = 0, j_off = 0;

				for (size_t iidx = 0; iidx < m; ++iidx) {
					if (iidx == i)
						continue;

					i_off = (iidx + ((iidx > i)*(-1)));

					for (size_t jidx = 0; jidx < n; ++jidx) {
						if (jidx == j)
							continue;

						j_off = (jidx + ((jidx > j)*(-1)));

						(*pMinor)[i_off*(n - 1) + j_off] = pMat[iidx*n + jidx];
					}
				}
			};

			template <typename val_type>
			// Get the cofactor for the element a(i, j) of m_mat.
			inline void __shirabe_math__matrix_get_cofactor(
				const val_type *pMat,
				const size_t    m,
				const size_t    n,
				const size_t    i,
				const size_t    j,
				val_type       *pCofactor)
			{
				val_type  det    = 0;
				size_t    out_m  = 0, out_n = 0;
				val_type *pMinor = NULL;

				__shirabe_math__matrix_get_minor<val_type>(pMat, m, n, i, j, &pMinor, &out_m, &out_n);

				if (out_m > 3)
					__shirabe_math__matrix_determinant_leibnitz_laplace<val_type>(pMinor, m - 1, n - 1, &det);
				else
					__shirabe_math__matrix_determinant_sarrus<val_type>(pMinor, &det);

				//__shirabe_math__matrix_determinant_gauss_jordan<_m - 1, m_n - 1, val_type>(minor, &det);

				*pCofactor = powf(-1, (i + j))*det;

			};

			template <typename val_type, size_t m, size_t n>
			// Get the cofactor matrix of m_mat.
			inline void __shirabe_math__matrix_get_cofactor_matrix(
				const val_type *pMat,
				val_type       *pCofactor_mat)
			{
				val_type cofactor = 0;

				if (pCofactor_mat == NULL)
					pCofactor_mat = new val_type[m * n];

				for (size_t i = 0; i < m; ++i) {
					for (size_t j = 0; j < n; ++j) {
						__shirabe_math__matrix_get_cofactor<val_type>(pMat, m, n, i, j, &cofactor);

						pCofactor_mat[i*n + j] = cofactor;
					}
				}
			};


			template <size_t m, size_t n, typename val_type>
			// Apply the gauss-jordan-elimination-algorithm to m_mat.
			//
			// By default the algorithm returns the echelon form of the upper triangle matrix.
			// To return the reduced echelon form set m_pivot to true.
			//
			// Optionally the parity caused by elemental row transforms can be returned.
			// If a pointer is passed for m_inv, returning the inverse matrix of m_mat, m_pivot must be 
			// set to true, as well as a pointer passed for m_parity.
			void __shirabe_math__matrix_gauss_jordan(
				val_type *pMat, 
				bool      pivot   = false,
				char     *pParity = NULL, 
				val_type *pInv    = NULL)
			{
				// ASSERTION REQUIRED

				val_type *ptr = pMat, *a_ij = 0;
				val_type *inv_ptr = pInv;

				char parity = 1;
				size_t row = 0;

				// Gauﬂ-Elimination Algorithm
				//
				// Basically there are 3 operations performed:
				// -> S: Skalar multiplication of row i with factor a
				// -> R: Addition of row j scaled by factor a to row i
				// -> T: Exchange of row i with row j
				//
				// Since the algorithm is forward progressive and processes 
				// column by column, the precedence order T -> S -> R can be 
				// used to sufficiently apply the gauﬂ algorithm without 
				// redundancies!
				// The S-operation is optional depending on if pivotization is
				// required or not. If not, only the rows below the non-normalized
				// pivot point will be set to 0 with the R-op putting the
				// result matrix into the echelon form.
				//
				// Echelon form (= Stufenform):
				// a11 a12 ... a1n
				// 0   a22 ... a2n
				// ... ... ... ...
				// 0   0   0   amn
				// 
				// If pivotization is required, the result matrix will result in the 
				// reduced echelon form.
				//
				// Reduced echelon form (= Treppennormalform):
				// 1   a12 0 a13 ... a1n
				// 0   0   1 a23 ... a2n
				// ... ... 0 1   ... ... 
				// 0   0   0 0   ... 1
				//
				// The pivotization can be used for the calculation of the inverse matrix
				// but may not used for determinant-calculation!

				// Process column by column
				for (size_t j = 0; j < n; ++j) {
					a_ij = (ptr + (row*n + j));

					// Switch rows if a_ij is 0!
					if (*a_ij == 0) {
						bool null_row = true;

						// Find row to replace! If none is found, nothing happens an 
						// the algorithm moves on with the next row.
						for (size_t k = (row + 1); k < m; ++k) {
							if (ptr[k*m + j] != 0)
							{
								__shirabe_math__matrix_elemental_row_transform_T<m, n, val_type>(pMat, row, k);
								if (pInv != NULL)
									__shirabe_math__matrix_elemental_row_transform_T<m, n, val_type>(pInv, row, k);

								parity   = parity > 0 ? -1 : 1;
								null_row = false;
								break;
							}
						}

						row -= null_row;
					}

					// Pivotize a_ij
					if (pivot) {
						if (*a_ij != 0) {
							// a_ij is pivot
							__shirabe_math__matrix_elemental_row_transform_S<m, n, val_type>(pMat, row, (1.0f / (*a_ij)), pInv);
						}
					}

					// For all i != j, make a_ij above and below pivot to become 0
					for (size_t i = ((pivot) ? 0 : (row + 1)); i < m; ++i) {
						if (i == j)
							continue;

						if (ptr[i*n + j] != 0)
							__shirabe_math__matrix_elemental_row_transform_R<m, n, val_type>(pMat, i, j, -(ptr[i*n + j] / ptr[row*n + j]), pInv);
					}

					++row;
				}

				*pParity = parity;
			}

			template<typename val_type>
			// Determine the determinant of an arbitrary 2x2-matrix applying the fish rule.
			inline void __shirabe_math__matrix_determinant_fishrule(
				const val_type *pMat, 
				val_type       *det)
			{
				// Fischregel
				// INSERT AASSERT!

				*det = ((pMat[0] * pMat[3]) - (pMat[2] * pMat[1]));
			}

			template <typename val_type>
			// Determine the determinant of an arbitrary 3x3-matrix applying sarrus' rule.
			inline void __shirabe_math__matrix_determinant_sarrus(
				const val_type *pMat,
				val_type       *pDet)
			{
				// Regel von Sarrus!
				size_t col_off = 0;
				val_type det = 0, tmp_pos = 0, tmp_neg = 0;
				const val_type *ptr = pMat;

				for (size_t j = 0; j < 3; ++j) {
					tmp_pos = tmp_neg = 1;
					for (size_t i = 0; i < 3; ++i) {
						col_off  = ((i + j) % 3);
						tmp_pos *= ptr[i * 3 + col_off];
						tmp_neg *= ptr[(3 - i - 1) * 3 + col_off];
					}
					det += tmp_pos - tmp_neg;
				}

				*pDet = det;
			}

			template <size_t m, size_t n, typename val_type>
			// Applies the gauss-jordan-elimination algorithm to this nxn-matrix to calulcate it's determinant.
			inline void __shirabe_math__matrix_determinant_gauss_jordan(
				const val_type *pMat, 
				val_type       *pDet)
			{
				val_type det = 1;

				// container for triangle matrix!
				val_type *tri = new val_type[m * n];

				size_t size = (m * n * sizeof(val_type));
				memcpy_s(tri, size, pMat, size);

				const val_type *ptr = NULL;
				char parity = 1;

				__shirabe_math__matrix_gauss_jordan<m, n, val_type>(tri, false, &parity, NULL);

				// Determinant is product of triangulated 
				// matrix diagonal elements!
				for (size_t i = 0; i < m; ++i)
					det *= tri[i * n + i];

				delete[] tri;
				tri = NULL;

				// Apply parity if rows had to be switched, 
				// since this inverts the sign.
				*pDet = det * parity;
			}

			template <typename val_type>
			inline void __shirabe_math__matrix_determinant_leibnitz_laplace(
				const val_type *pMat,
				const size_t    m,
				const size_t    n,
				val_type       *pDet = NULL)
			{
				if (!(pMat && pDet))
					return;

				val_type cofactor = 0;
				*pDet = 0;

				for (size_t j = 0; j < n; ++j) {
					cofactor = 0;
					__shirabe_math__matrix_get_cofactor<val_type>(pMat, m, n, 0, j, &cofactor);
					*pDet += (pMat[j] * cofactor);
				}
			};

#if defined(PLATFORM_WINDOWS)
// Reenable C4244 for further code
#pragma warning(pop)
#endif

		}
	}
}

#endif