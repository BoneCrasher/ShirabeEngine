#ifndef __SHIRABE_SHIRABE_MATH_MATRIX_OPS_H__
#define __SHIRABE_SHIRABE_MATH_MATRIX_OPS_H__

#include <stdint.h>
#include <exception>
#include <stdexcept>

#include "platform/platform.h"
#include "math/common.h"

namespace Engine
{
    namespace Math
    {
        namespace PRIVATE
        {

            #if defined(PLATFORM_WINDOWS)
            #pragma warning(push)
                        // Disable C4244 for warnings on multiplication of uint64_t with float/double
            #pragma warning(disable:4244)
            #endif

            // Implementations for the elemental row transformations.
			//
			// ElemT: Switch the i-th row with the j-th row
			// ElemR: Add the a-fold of the j-th row to the i-th row.
			// ElemS: Scale the i-th row by the factor lambda

            /**
             * Scale the row at 'aRowIndex' in 'aMatrix' by factor 'aScaleFactor'.
             *   --> row[i] *= s;
             *
             * @param aMatrix
             * @param aRowIndex
             * @param aScaleFactor
             * @param aOutInverse
             */
            template <uint64_t NRows, uint64_t NCols, typename TValue>
            static void __shirabe_math__matrix_elemental_row_transform_S(
                TValue         *aMatrix,
                uint64_t const &aRowIndex,
                TValue   const &aScaleFactor,
                TValue         *aOutInverse = nullptr);

            /**
             * Add the 'aSourceRowIndex' scaled by 'aScaleFactor' to 'aTargetRowIndex'.
             *   --> row[i] += row[j] * s.
             *
             * @param aMatrix
             * @param aTargetRowIndex
             * @param aSourceRowIndex
             * @param aScaleFactor
             * @param aOutInverse
             */
            template <uint64_t NRows, uint64_t NCols, typename TValue>
            static void __shirabe_math__matrix_elemental_row_transform_R(
                TValue         *aMatrix,
                uint64_t const &aTargetRowIndex,
                uint64_t const &aSourceRowIndex,
                TValue   const &aScaleFactor,
                TValue         *aOutInverse = nullptr);

            /**
             * Switches the rows at 'aSourceRowIndex' and 'aTargetRowIndex'.
             *   --> swap(row[i], row[j])
             *
             * @param aMatrix
             * @param aSourceRowIndex
             * @param aTargetRowIndex
             */
            template <uint64_t NRows, uint64_t NCols, typename TValue>
            static void __shirabe_math__matrix_elemental_row_transform_T(
                TValue         *aMatrix,
                uint64_t const &aSourceRowIndex,
                uint64_t const &aTargetRowIndex);

            /**
             * Multiplies this m*s-matrix with another s*n-matrix passed.
             * The multiplication will return a m*n-product-matrix.
             *
             * @param aLHS
             * @param aRHS
             * @param aInOutMultiplicationResult
             */
            template <typename TValue, uint64_t M, uint64_t S, uint64_t N>
            static void __shirabe_math__matrix_multiply(
                TValue const *aLHS,
                TValue const *aRHS,
                TValue       *aInOutMultiplicationResult);

            /**
             * Return a transposed copy of the matrix passed!
             * The original matrix remains unchanged.
             *
             * @param aMatrix
             * @param aInOutTransposedMatrix
             */
            template <uint64_t M, uint64_t N, typename TValue>
            static void __shirabe_math__matrix_transpose(
                TValue const *aMatrix,
                TValue       *aInOutTransposedMatrix);

            /**
             * Get the minor in m_mat for the element a(i, j).
             *
             * @param aMatrix
             * @param aRowCount
             * @param aColumnCount
             * @param aRowIndex
             * @param aColumnIndex
             * @param aOutMinor
             * @param aOutMinorRowCount
             * @param pOutMinorColumnCount
             */
            template <typename TValue>
            static void __shirabe_math__matrix_get_minor(
                TValue   const  *aMatrix,
                uint64_t const   aRowCount,
                uint64_t const   aColumnCount,
                uint64_t const   aRowIndex,
                uint64_t const   aColumnIndex,
                TValue         **aOutMinor,
                uint64_t        *aOutMinorRowCount,
                uint64_t        *pOutMinorColumnCount);

            /**
             * Get the cofactor for the element a(i, j) of m_mat.
             *
             * @param aMatrix
             * @param aRowCount
             * @param aColumnCount
             * @param aRowIndex
             * @param aColumnIndex
             * @param aOutCofactor
             */
            template <typename TValue>
            static void __shirabe_math__matrix_get_cofactor(
                TValue   const *aMatrix,
                uint64_t const  aRowCount,
                uint64_t const  aColumnCount,
                uint64_t const  aRowIndex,
                uint64_t const  aColumnIndex,
                TValue         *aOutCofactor);

            /**
             * Get the cofactor matrix of m_mat.
             *
             * @param aMatrix
             * @param aOutCofactorMatrix
             */
            template <typename TValue, uint64_t M, uint64_t N>
            static void __shirabe_math__matrix_get_cofactor_matrix(
                TValue const *aMatrix,
                TValue       *aOutCofactorMatrix);

            /**
             * Apply the gauss-jordan-elimination-algorithm to aMatrix.
             * By default the algorithm returns the echelon form of the upper triangle matrix.
             * Optionally the parity caused by elemental row transforms can be returned.
             * If a pointer is passed for mInverse, returning the inverse matrix of aMatrix,
             * aReducedEchelon must be set to true, as well as a pointer passed for aOutParity.
             *
             * @param aMatrix
             * @param aReducedEchelon To return the reduced echelon form set aReducedEchelon. to true
             * @param aOutParity
             * @param aOutInverse
             */
            template <uint64_t m, uint64_t n, typename TValue>
            static void __shirabe_math__matrix_gauss_jordan(
                TValue *aMatrix,
                bool    aReducedEchelon = false,
                char   *aOutParity      = nullptr,
                TValue *aOutInverse     = nullptr);

            /**
             * Determine the determinant of an arbitrary 3x3-matrix applying sarrus' rule.
             *
             * @param aMatrix
             * @param aOutDeterminant
             */
            template <typename TValue>
            static void __shirabe_math__matrix_determinant_sarrus(
                TValue const *aMatrix,
                TValue       *aOutDeterminant);

            /**
             * Determine the determinant of an arbitrary 2x2-matrix applying the fish rule.
             *
             * @param aMatrix
             * @param aOutDeterminant
             */
            template<typename TValue>
            static void __shirabe_math__matrix_determinant_fishrule(
                TValue const *aMatrix,
                TValue       *aOutDeterminant);

            /**
             * Applies the gauss-jordan-elimination algorithm to this nxn-matrix to calulcate it's determinant.
             *
             * @param aMatrix
             * @param aOutDeterminant
             */
            template <uint64_t M, uint64_t N, typename TValue>
            static void __shirabe_math__matrix_determinant_gauss_jordan(
                    TValue const *aMatrix,
                    TValue       *aOutDeterminant);

            /**
             * Determine the determinant of an arbitrary MxN matrix using the leibnitz-laplace method.
             *
             * @param pMat
             * @param m
             * @param n
             * @param pDet
             */
            template <typename TValue>
            static void __shirabe_math__matrix_determinant_leibnitz_laplace(
                TValue   const *aMatrix,
                uint64_t const &aRowCount,
                uint64_t const &aColumnCount,
                TValue         *aOutDeterminant = nullptr);
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <uint64_t NRows, uint64_t NCols, typename TValue>
            void __shirabe_math__matrix_elemental_row_transform_S(
                    TValue         *aMatrix,
                    uint64_t const &aRowIndex,
                    TValue   const &aScaleFactor,
                    TValue         *aOutInverse)
			{				
                TValue         *ptr       = aMatrix;
                TValue         *inv_ptr   = aOutInverse;
                uint64_t  const byte_size = NCols * sizeof(TValue);
                uint64_t  const i_off     = (aRowIndex * NCols);
				
                for (uint64_t i = 0; i < NRows; ++i)
                {
                    ptr[i_off + i] *= aScaleFactor;

                    if (nullptr != inv_ptr)
                        inv_ptr[i_off + i] *= aScaleFactor;
				}
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <uint64_t NRows, uint64_t NCols, typename TValue>
            void __shirabe_math__matrix_elemental_row_transform_R(
                    TValue         *aMatrix,
                    uint64_t const &aTargetRowIndex,
                    uint64_t const &aSourceRowIndex,
                    TValue   const &aScaleFactor,
                    TValue         *aOutInverse)
			{
                TValue         *ptr    = aMatrix;
                TValue        *inv_ptr = aOutInverse;
                TValue         tmp     = 0;
                uint64_t const i_off   = (aTargetRowIndex * NCols);
                uint64_t const j_off   = (aSourceRowIndex * NCols);

                for (uint64_t i = 0; i < NRows; ++i)
                {
                    ptr[i_off + i] += (ptr[j_off + i] * aScaleFactor);

                    if (inv_ptr != nullptr)
                        inv_ptr[i_off + i] += (inv_ptr[j_off + i] * aScaleFactor);
				}
			}
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <uint64_t NRows, uint64_t NCols, typename TValue>
            void __shirabe_math__matrix_elemental_row_transform_T(
                    TValue         *aMatrix,
                    uint64_t const &aSourceRowIndex,
                    uint64_t const &aTargetRowIndex)
			{
                if (aSourceRowIndex == aTargetRowIndex)
					return;

                TValue        *ptr        = aMatrix;
                uint64_t const byte_size  = NCols * sizeof(TValue);
                uint64_t const i_off      = (aSourceRowIndex * NCols);
                uint64_t const j_off      = (aTargetRowIndex * NCols);
                TValue         tmp[NCols] = {};

                memcpy(tmp,           (ptr + i_off), byte_size);
                memcpy((ptr + i_off), (ptr + j_off), byte_size);
                memcpy((ptr + j_off), tmp,           byte_size);
			}
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <typename TValue, uint64_t M, uint64_t S, uint64_t N>
			// Multiplies this m*s-matrix with another s*n-matrix passed.
			// The multiplication will return a m*n-product-matrix.
            void __shirabe_math__matrix_multiply(
                    TValue const *aLHS,
                    TValue const *aRHS,
                    TValue       *aInOutMultiplicationResult)
			{
				// Implement asserts!

				// Get immediate matrix value storage pointers for both matrices.
                TValue const *l_ptr = aLHS;
                TValue const *r_ptr = aRHS;

				// Get immediate matrix value storage pointer for the result matrix.
                TValue *val_ptr     = nullptr;
                TValue *product_ptr = aInOutMultiplicationResult;

				// Algorithm:
				// -> c[i, j] = a[i, s]*b[s, j]
				// Since the matrix buffers are plain blocks of m*n*sizeof(value_type) bytes
				// data access must be offset by using iteration indices!

                for (uint64_t i = 0; i < M; ++i)
                {
                    for (uint64_t j = 0; j < N; ++j)
                    {
                        val_ptr = (product_ptr + ((i * N) + j));
						*val_ptr = 0;

                        for (uint64_t sidx = 0; sidx < S; ++sidx)
                            *val_ptr += (l_ptr[(i * S) + sidx] * r_ptr[(sidx * N) + j]);
					}
				}

                l_ptr = r_ptr = val_ptr = product_ptr = nullptr;
			}
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <uint64_t M, uint64_t N, typename TValue>
            void __shirabe_math__matrix_transpose(
                    TValue const *aMatrix,
                    TValue       *aInOutTransposedMatrix)
			{
				/* a11 a12 ... a1n      a11 a21 ... am1
				*  a21 a22 ... a2n  ->  a12 a22 ... am2
				*  ... ... ... ...      ... ... ... ...
				*  am1 am2 ... amn      a1n a2n ... amn
				*/
                TValue       *transpPtr = aInOutTransposedMatrix;
                TValue const *ptr       = aMatrix;

                for (uint64_t i = 0; i < M; ++i)
                {
                    for (uint64_t j = 0; j < N; ++j)
                    {
                        transpPtr[(j * N) + i] = ptr[j + (i * N)];
					}
				}
			}
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <typename TValue>
            void __shirabe_math__matrix_get_minor(
                    TValue   const  *aMatrix,
                    uint64_t const   aRowCount,
                    uint64_t const   aColumnCount,
                    uint64_t const   aRowIndex,
                    uint64_t const   aColumnIndex,
                    TValue         **aOutMinor,
                    uint64_t        *aOutMinorRowCount,
                    uint64_t        *aOutMinorColumnCount)
			{
                if(aRowCount >= 2 || aColumnCount >= 2)
                {
                    /*
                    * a11 ... a1n                             a22 ... a2n
                    * ... ... ...  --> i = j = 1 --> minor =  ... ... ...
                    * am1 ... amn                             am2 ... amn
                    */

                    if (*aOutMinor == nullptr)
                        throw std::exception(); //"No out pointer provided for calculation.");

                    *aOutMinorRowCount    = aRowCount    - 1;
                    *aOutMinorColumnCount = aColumnCount - 1;

                    uint64_t
                            i_off = 0,
                            j_off = 0;

                    for (uint64_t iidx = 0; iidx < aRowCount; ++iidx)
                    {
                        if (iidx == aRowIndex)
                            continue;

                        i_off = (iidx + ((iidx > aRowIndex) * (-1)));

                        for (uint64_t jidx = 0; jidx < aColumnCount; ++jidx)
                        {
                            if (jidx == aColumnIndex)
                                continue;

                            j_off = (jidx + ((jidx > aColumnIndex) * (-1)));

                            (*aOutMinor)[(i_off * (aColumnCount - 1)) + j_off] = aMatrix[(iidx * aColumnCount) + jidx];
                        }
                    }
                }
			};
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <typename TValue>
			// Get the cofactor for the element a(i, j) of m_mat.
            void __shirabe_math__matrix_get_cofactor(
                    TValue   const *aMatrix,
                    uint64_t const  aRowCount,
                    uint64_t const  aColumnCount,
                    uint64_t const  aRowIndex,
                    uint64_t const  aColumnIndex,
                    TValue         *aOutCofactor)
			{
                TValue   determinant      = 0;
                uint64_t minorRowCount    = 0,
                         minorColumnCount = 0;
                TValue  *minorMatrix      = nullptr;

                __shirabe_math__matrix_get_minor<TValue>(
                             aMatrix,
                             aRowCount,
                             aColumnCount,
                             aRowIndex,
                             aColumnIndex,
                            &minorMatrix,
                            &minorRowCount,
                            &minorColumnCount);

                if (minorRowCount > 3)
                {
                    __shirabe_math__matrix_determinant_leibnitz_laplace<TValue>(
                                minorMatrix,
                                minorRowCount,
                                minorColumnCount,
                                &determinant);
                }
				else
                {
                    __shirabe_math__matrix_determinant_sarrus<TValue>(minorMatrix, &determinant);
                }

                //__shirabe_math__matrix_determinant_gauss_jordan<_m - 1, m_n - 1, TValue>(minor, &det);

                *aOutCofactor = powf(-1, (aRowIndex + aColumnIndex)) * determinant;
			};
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <typename TValue, uint64_t M, uint64_t N>
			// Get the cofactor matrix of m_mat.
            void __shirabe_math__matrix_get_cofactor_matrix(
                    TValue const *aMatrix,
                    TValue       *aOutCofactorMatrix)
			{
                TValue cofactor = 0;

                if (aOutCofactorMatrix == nullptr)
                    throw std::runtime_error("Invalid cofactor matrix out pointer.");

                for (uint64_t i = 0; i < M; ++i) {
                    for (uint64_t j = 0; j < N; ++j) {
                        __shirabe_math__matrix_get_cofactor<TValue>(aMatrix, M, N, i, j, &cofactor);

                        aOutCofactorMatrix[(i * N) + j] = cofactor;
					}
				}
			};
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <uint64_t M, uint64_t N, typename TValue>
			void __shirabe_math__matrix_gauss_jordan(
                    TValue *aMatrix,
                    bool    aReducedEchelon,
                    char   *aOutParity,
                    TValue *aOutInverse)
			{
				// ASSERTION REQUIRED

                TValue *ptr     = aMatrix, *a_ij = 0;
                TValue *inv_ptr = aOutInverse;

                char     parity = 1;
                uint64_t row    = 0;

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
                // required or not. If not, only the NRows below the non-normalized
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
                for (uint64_t j = 0; j < N; ++j)
                {
                    a_ij = (ptr + ((row * N) + j));

                    // Switch NRows if a_ij is 0!
                    if (*a_ij == 0)
                    {
                        bool nullptr_row = true;

						// Find row to replace! If none is found, nothing happens an 
						// the algorithm moves on with the next row.
                        for (uint64_t k = (row + 1); k < M; ++k)
                        {
                            if (ptr[(k * M) + j] != 0)
							{
                                __shirabe_math__matrix_elemental_row_transform_T<M, N, TValue>(aMatrix, row, k);

                                if (aOutInverse != nullptr)
                                    __shirabe_math__matrix_elemental_row_transform_T<M, N, TValue>(aOutInverse, row, k);

                                parity      = parity > 0 ? -1 : 1;
                                nullptr_row = false;
								break;
							}
						}

                        row -= nullptr_row;
					}

					// Pivotize a_ij
                    if (aReducedEchelon)
                    {
                        if (*a_ij != 0)
                        {
							// a_ij is pivot
                            TValue const scaleFactor = (1.0f / (*a_ij));
                            __shirabe_math__matrix_elemental_row_transform_S<M, N, TValue>(aMatrix, row, scaleFactor, aOutInverse);
						}
					}

					// For all i != j, make a_ij above and below pivot to become 0
                    uint64_t initialIndex = ((aReducedEchelon) ? 0 : (row + 1));
                    for (uint64_t i = initialIndex; i < M; ++i)
                    {
						if (i == j)
							continue;

                        TValue const scaleFactor = -(ptr[(i * N) + j] / ptr[(row * N) + j]);
                        if (ptr[(i * N) + j] != 0)
                            __shirabe_math__matrix_elemental_row_transform_R<M, N, TValue>(aMatrix, i, j, scaleFactor, aOutInverse);
					}

					++row;
				}

                *aOutParity = parity;
			}
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <typename TValue>
            void __shirabe_math__matrix_determinant_sarrus(
                    TValue const *aMatrix,
                    TValue       *aOutDeterminant)
			{
				// Regel von Sarrus!
                uint64_t col_off = 0;
                TValue
                        det     = 0,
                        tmp_pos = 0,
                        tmp_neg = 0;
                TValue const
                        *ptr = aMatrix;

                for (uint64_t j = 0; j < 3; ++j)
                {
					tmp_pos = tmp_neg = 1;
                    for (uint64_t i = 0; i < 3; ++i)
                    {
						col_off  = ((i + j) % 3);
                        tmp_pos *= ptr[(i * 3) + col_off];
                        tmp_neg *= ptr[((3 - i - 1) * 3) + col_off];
					}

					det += tmp_pos - tmp_neg;
				}

                *aOutDeterminant = det;
			}
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template<typename TValue>
            void __shirabe_math__matrix_determinant_fishrule(
                    TValue const *aMatrix,
                    TValue       *aOutDeterminant)
            {
                // Fischregel
                *aOutDeterminant = ((aMatrix[0] * aMatrix[3]) - (aMatrix[2] * aMatrix[1]));
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <uint64_t M, uint64_t N, typename TValue>
            void __shirabe_math__matrix_determinant_gauss_jordan(
                TValue const *aMatrix,
                TValue       *aOutDeterminant)
			{
                TValue det = 1;

				// container for triangle matrix!
                TValue *tri = new TValue[M * N];

                uint64_t const size = (M * N * sizeof(TValue));
                memcpy(tri, aMatrix, size);

                TValue const *ptr    = nullptr;
                char          parity = 1;

                __shirabe_math__matrix_gauss_jordan<M, N, TValue>(tri, false, &parity, nullptr);

				// Determinant is product of triangulated 
				// matrix diagonal elements!
                for (uint64_t i = 0; i < M; ++i)
                    det *= tri[(i * N) + i];

				delete[] tri;
                tri = nullptr;

                // Apply parity if NRows had to be switched,
				// since this inverts the sign.
                *aOutDeterminant = (det * parity);
			}
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            template <typename TValue>
            void __shirabe_math__matrix_determinant_leibnitz_laplace(
                    TValue   const *aMatrix,
                    uint64_t const &aRowCount,
                    uint64_t const &aColumnCount,
                    TValue         *aOutDeterminant)
			{
                if (!(aMatrix && aOutDeterminant))
					return;

                TValue cofactor = 0;

                *aOutDeterminant = 0;

                for (uint64_t j = 0; j < aColumnCount; ++j)
                {
					cofactor = 0;

                    __shirabe_math__matrix_get_cofactor<TValue>(aMatrix, aRowCount, aColumnCount, 0, j, &cofactor);
                    *aOutDeterminant += (aMatrix[j] * cofactor);
				}
			};
            //<-----------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)
// Reenable C4244 for further code
#pragma warning(pop)
#endif

		}
	}
}

#endif
