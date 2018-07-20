#ifndef __SHIRABE_SHIRABE_MATH_MATRIX_H__
#define __SHIRABE_SHIRABE_MATH_MATRIX_H__

#include <iterator>

#include "math/field.h"
#include "math/matrixops.h"

namespace Engine
{
    namespace Math
    {

		// Use private matrix algorithms from MatrixOps.h
		using namespace Engine::Math::PRIVATE;

		/* NOTES
		 *
         * The matrix row iterator should encapsulate a well defined interval of a matrix-CField.
         * The interval is row := [i*_N, i*_n + j].
		 *
		 * Required operators:
		 *
		 * operator++/operator-- : move forward or backward in the iterator.
		 * begin()/end() : to determine, where to start and stop
		 * operator[] : for direct access, throws invalid_argument exception!
		 * operator do access the current value this way : value = *iterator;
		 *
		 * May derive from the STL-iterator.
		 */

        /**
         * @brief
         * @tparam T
         */
		template <typename T>
        class CMatrixRowIterator
		{
        public_typedefs:
            typedef CMatrixRowIterator<T>                                          class_type;
            typedef T                                                              value_type;
            typedef value_type const                                               const_value_type;
            typedef std::iterator<std::forward_iterator_tag, T, ptrdiff_t, T*, T&> iterator;
            typedef iterator const                                                 const_iterator;

        public_constructors:
            CMatrixRowIterator<T>() = default;

        public_methods:
            value_type       *ptr()             { return mRow; }
            const_value_type  const_ptr() const { return const_cast<const_value_type>(mRow); }

        public_operators:
            const_value_type operator[](size_t aIndex)
            {
                value_type *ptr = (mRow + aIndex);
                return const_cast<const_value_type>(*(mRow + index));
			}

        private_members:
            value_type* mRow;
            size_t      mRowOffset;
		};

        /**
         * @brief               Matrix base class for arbitrary m x n matrices.
         *                      Defines basic initialization and operations such as:
         *                      - Addition
         *                      - Scalar-multiplication
         *                      - Matrix-multiplication
         *                      - Lerping
         * @tparam NRowCount
         * @tparam NColumnCount
         */
        template <size_t NRowCount = 4, size_t NColumnCount = 4>
        class CMatrix
            : public CField<float, sizeof(float), (NRowCount * NColumnCount), NColumnCount>
		{
        public_typedefs:
            typedef CField <float, sizeof(float), (NRowCount * NColumnCount), NColumnCount> base_type;
            typedef typename base_type::value_type                                          value_type;
            typedef CMatrix<NRowCount, NColumnCount>                                        class_type, matrix_type;
            typedef CMatrix<NColumnCount, NRowCount>                                        transposed_matrix_type;

        public_constructors:
            CMatrix<NRowCount, NColumnCount>()
                : CField<float, sizeof(float), (NRowCount * NColumnCount), NColumnCount>()
			{};

            CMatrix<NRowCount, NColumnCount>(std::initializer_list<value_type> aSource)
                : base_type(aSource)
			{};

            CMatrix<NRowCount, NColumnCount>(base_type const &aOther)
                : base_type(aOther)
			{};

            CMatrix<NRowCount, NColumnCount>(matrix_type const &aOther)
                : base_type(aOther)
			{};

        public_destructors:
            virtual ~CMatrix<NRowCount, NColumnCount>() = default;

        public_static_functions:
            static matrix_type identity()
            {
				matrix_type              identity = matrix_type();
				matrix_type::value_type *ptr      = identity.ptr();

                for (size_t i = 0; i < NColumnCount; ++i)
                {
                    ptr[i*NColumnCount + i] = 1;
                }

				return identity;
			}

            static matrix_type zero()
            {
				return matrix_type();
			}

        public_operators:
            operator base_type()
            {
                return static_cast<base_type>(*this);
            }

            class_type operator+(class_type const &r)
            {
				return ((class_type(*this)) += r);
			}

            class_type& operator +=(class_type const &r)
            {
				_matrix_op__add((*this).ptr(), r.const_ptr());

				return *this;
			}

            class_type operator-(class_type const &r)
            {
				return ((class_type(*this)) -= r);
			}

            class_type& operator -=(class_type const &r)
            {
				_matrix_op__subtract((*this).ptr(), r.const_ptr());

				return *this;
            }

        public_methods:
			// Transpose this matrix instance and return it's reference.
            // Opposite to the SMMatrixTranspose functioN,
			// the transposition is immediately applied to this instance.
            transposed_matrix_type transpose()
            {
				transposed_matrix_type mat;
                __shirabe_math__matrix_transpose<NRowCount, NColumnCount, value_type>(this->const_ptr(), mat.ptr());
				return mat;
			}

			// Return a copy of the adjoint-matrix of this instance.
			// The adjoint matrix is the transposed co-factor matrix.
            transposed_matrix_type adjoint()
            {
				return this->cofactor().transpose();
			}

            matrix_type cofactor()
            {
				matrix_type mat = matrix_type();

                __shirabe_math__matrix_get_cofactor_matrix<value_type, NRowCount, NColumnCount>(this->const_ptr(), mat.ptr());
			}

            virtual value_type const determinant() const
            {
				// dirrty test
                if(NColumnCount != NRowCount)
                    throw new std::runtime_error("Determinant is not properly defined for non-square matrices.");

				value_type det = 0;
                __shirabe_math__matrix_determinant_leibnitz_laplace<value_type>(this->const_ptr(), NRowCount, NColumnCount, &det);

                //__shirabe_math__matrix_determinant_gauss_jordan<NRowCount, NColumnCount, value_type>(this->const_ptr(), &det);

				return det;
			}

            matrix_type invert()
            {
				/* 'Treppennormalformgleichung' -> Gauss-Elimination with pivoting and identity matrix transformation! */
                matrix_type inv    = matrix_type::identity(); //matrix_type(*this);
                matrix_type cpy    = matrix_type(*this);
                char        parity = 1;

                __shirabe_math__matrix_gauss_jordan<NRowCount, NColumnCount, value_type>(cpy.ptr(), true, &parity, inv.ptr());

				return inv;
            }

        private_methods:
            // Algorithm: additions
            void _matrix_op__add(
                        value_type       *aLeftPointer,
                        value_type const *aRightPointer) const
            {
                size_t off = 0;

                for (size_t i = 0; i < NRowCount; ++i)
                {
                    for (size_t j = 0; j < NColumnCount; ++j)
                    {
                        off                = i*NColumnCount + j;
                        aLeftPointer[off] += aRightPointer[off];
                    }
                }
            }

            // Algorithm: inverse addition
            void _matrix_op__subtract(
                        value_type       *aLeftPointer,
                        value_type const *aRightPointer) const
            {
                size_t off = 0;

                for (size_t i = 0; i < NRowCount; ++i)
                {
                    for (size_t j = 0; j < NColumnCount; ++j)
                    {
                        off                = i*NColumnCount + j;
                        aLeftPointer[off] -= aRightPointer[off];
                    }
                }
            }

            // Algorithm: scalar multiplication
            void _matrix_op__scale(
                        value_type       *aLeftPointer,
                        value_type const  aFactor) const
            {
                size_t off = 0;

                for (size_t i = 0; i < NRowCount; ++i)
                {
                    for (size_t j = 0; j < NColumnCount; ++j)
                    {
                        off                = i*NColumnCount + j;
                        aLeftPointer[off] *= aFactor;
                    }
                }
            }
		};

        template <size_t N = 4>
        class CSquareMatrix
            : public CMatrix<N, N>
		{
        public_typedefs:
            typedef          CMatrix<N, N>         base_type;
            typedef          CSquareMatrix<N>      class_type;
            typedef typename base_type::value_type value_type;

        public_constructors:
            CSquareMatrix<N>() = default;

            CSquareMatrix<N>(std::initializer_list<value_type> aSource)
                : CMatrix<N, N>(aSource)
			{}

            CSquareMatrix<N>(base_type const &aOther)
                : CMatrix<N, N>(aOther)
			{}
            CSquareMatrix<N>(CSquareMatrix<N> const &aOther)
                : CMatrix<N, N>(aOther)
			{}

        public_destructors:
            virtual ~CSquareMatrix<N>() = default;

        public_methods:
            virtual value_type const determinant() const
            {
				return base_type::determinant();
			}
		};

        class CMatrix2x2
            : public CSquareMatrix<2>
		{
        public_typedefs:
            typedef CSquareMatrix<2>               base_type;
            typedef CMatrix2x2                     class_type;
            typedef typename base_type::value_type value_type;

        public_constructors:
            CMatrix2x2() = default;

            CMatrix2x2(std::initializer_list<value_type> aSource)
                : CSquareMatrix<2>(aSource)
			{}

            CMatrix2x2(base_type const &aOther)
                : base_type(aOther)
			{}

            CMatrix2x2(CMatrix2x2 const &aOther)
                : CSquareMatrix<2>(aOther)
			{}

        public_destructors:
            virtual ~CMatrix2x2() = default;

        public_methods:
            value_type const &r00() const { return mField[0]; }
            value_type const &r01() const { return mField[1]; }
            value_type const &r10() const { return mField[2]; }
            value_type const &r11() const { return mField[3]; }

            value_type const &r00(value_type const &aValue) { return (mField[0] = aValue); }
            value_type const &r01(value_type const &aValue) { return (mField[1] = aValue); }
            value_type const &r10(value_type const &aValue) { return (mField[2] = aValue); }
            value_type const &r11(value_type const &aValue) { return (mField[3] = aValue); }

            virtual const_value_type determinant() const
            {
				value_type det = 0;
				__shirabe_math__matrix_determinant_fishrule<value_type>(this->const_ptr(), &det);

				return det;
			}
		};

        class CMatrix3x3
            : public CSquareMatrix<3>
		{
        public_typedefs:
            typedef CSquareMatrix<3>      base_type;
            typedef CMatrix3x3            class_type;
			typedef base_type::value_type value_type;

        public_constructors:
            CMatrix3x3() = default;

            CMatrix3x3(std::initializer_list<value_type> aSource)
                : CSquareMatrix<3>(aSource)
			{}

            CMatrix3x3(base_type const &aOther)
                : base_type(aOther)
			{}

            CMatrix3x3(CMatrix3x3 const &aOther)
                : CSquareMatrix<3>(aOther)
			{}
			
        public_destructors:
            virtual ~CMatrix3x3() = default;

        public_methods:
            value_type const & r00() const { return mField[0]; }
            value_type const & r01() const { return mField[1]; }
            value_type const & r02() const { return mField[2]; }
            value_type const & r10() const { return mField[3]; }
            value_type const & r11() const { return mField[4]; }
            value_type const & r12() const { return mField[5]; }
            value_type const & r20() const { return mField[6]; }
            value_type const & r21() const { return mField[7]; }
            value_type const & r22() const { return mField[8]; }

            value_type const & r00(value_type const &aValue) { return (mField[0] = aValue); }
            value_type const & r01(value_type const &aValue) { return (mField[1] = aValue); }
            value_type const & r02(value_type const &aValue) { return (mField[2] = aValue); }
            value_type const & r10(value_type const &aValue) { return (mField[3] = aValue); }
            value_type const & r11(value_type const &aValue) { return (mField[4] = aValue); }
            value_type const & r12(value_type const &aValue) { return (mField[5] = aValue); }
            value_type const & r20(value_type const &aValue) { return (mField[6] = aValue); }
            value_type const & r21(value_type const &aValue) { return (mField[7] = aValue); }
            value_type const & r22(value_type const &aValue) { return (mField[8] = aValue); }

            virtual const_value_type determinant() const
            {
				value_type det = 0;
				__shirabe_math__matrix_determinant_sarrus<value_type>(this->const_ptr(), &det);

				return det;
			}

		};

        class CMatrix4x4
            : public CSquareMatrix<4>
		{
        public_typedefs:
            typedef CSquareMatrix<4>      base_type;
            typedef CMatrix4x4            class_type;
			typedef base_type::value_type value_type;

        public_constructors:
            CMatrix4x4() = default;

            CMatrix4x4(std::initializer_list<value_type> aSource)
                : CSquareMatrix<4>(aSource)
			{}

            CMatrix4x4(const CMatrix<4, 4>&aOther)
                : base_type(aOther)
			{}

            CMatrix4x4(CMatrix4x4 const &aOther)
                : base_type(aOther)
			{}

        public_destructors:
            virtual ~CMatrix4x4() = default;

        public_methods:
            value_type const & r00() const { return mField[0];  }
            value_type const & r01() const { return mField[1];  }
            value_type const & r02() const { return mField[2];  }
            value_type const & r03() const { return mField[3];  }
            value_type const & r10() const { return mField[4];  }
            value_type const & r11() const { return mField[5];  }
            value_type const & r12() const { return mField[6];  }
            value_type const & r13() const { return mField[7];  }
            value_type const & r20() const { return mField[8];  }
            value_type const & r21() const { return mField[9];  }
            value_type const & r22() const { return mField[10]; }
            value_type const & r23() const { return mField[11]; }
            value_type const & r30() const { return mField[12]; }
            value_type const & r31() const { return mField[13]; }
            value_type const & r32() const { return mField[14]; }
            value_type const & r33() const { return mField[15]; }

            value_type const & r00(value_type const &aValue) { return (mField[0]  = aValue); }
            value_type const & r01(value_type const &aValue) { return (mField[1]  = aValue); }
            value_type const & r02(value_type const &aValue) { return (mField[2]  = aValue); }
            value_type const & r03(value_type const &aValue) { return (mField[3]  = aValue); }
            value_type const & r10(value_type const &aValue) { return (mField[4]  = aValue); }
            value_type const & r11(value_type const &aValue) { return (mField[5]  = aValue); }
            value_type const & r12(value_type const &aValue) { return (mField[6]  = aValue); }
            value_type const & r13(value_type const &aValue) { return (mField[7]  = aValue); }
            value_type const & r20(value_type const &aValue) { return (mField[8]  = aValue); }
            value_type const & r21(value_type const &aValue) { return (mField[9]  = aValue); }
            value_type const & r22(value_type const &aValue) { return (mField[10] = aValue); }
            value_type const & r23(value_type const &aValue) { return (mField[11] = aValue); }
            value_type const & r30(value_type const &aValue) { return (mField[12] = aValue); }
            value_type const & r31(value_type const &aValue) { return (mField[13] = aValue); }
            value_type const & r32(value_type const &aValue) { return (mField[14] = aValue); }
            value_type const & r33(value_type const &aValue) { return (mField[15] = aValue); }
		};

		// Transposition functions. Will always return copies and leave the passed matrix unchanged.
        CMatrix2x2 SMMatrixTranspose(CMatrix2x2 const &aMatrix);
        CMatrix3x3 SMMatrixTranspose(CMatrix3x3 const &aMatrix);
        CMatrix4x4 SMMatrixTranspose(CMatrix4x4 const &aMatrix);

		// Determinant calculation. 
		// Will use fish-rule, sarrus' rule, gauss-jordan elimination or leibniz-formula/laplace'sche Entwicklungssatz 
        CMatrix2x2::value_type SMMatrixDeterminant(CMatrix2x2 const &aMatrix);
        CMatrix3x3::value_type SMMatrixDeterminant(CMatrix3x3 const &aMatrix);
        CMatrix4x4::value_type SMMatrixDeterminant(CMatrix4x4 const &aMatrix);


        template<size_t M, size_t S, size_t N, typename val_type = typename CMatrix<M, N>::value_type>
        CMatrix<M, N> SMCMatrixMultiply(
                CMatrix<M, S> const &aLHS,
                CMatrix<S, N> const &aRHS)
        {
            CMatrix<M, N> product = CMatrix<M, N>();

            __shirabe_math__matrix_multiply<val_type, M, S, N>(aLHS.const_ptr(), aRHS.const_ptr(), product.ptr());

			return (product);
		};

        CMatrix2x2 SMMatrixMultiply(CMatrix2x2 const &aLHS, CMatrix2x2 const &aRHS);
        CMatrix3x3 SMMatrixMultiply(CMatrix3x3 const &aLHS, CMatrix3x3 const &aRHS);
        CMatrix4x4 SMMatrixMultiply(CMatrix4x4 const &aLHS, CMatrix4x4 const &aRHS);
	}
}

#endif
