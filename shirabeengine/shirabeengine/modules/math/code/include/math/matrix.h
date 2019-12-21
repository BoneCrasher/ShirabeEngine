#ifndef __SHIRABE_SHIRABE_MATH_MATRIX_H__
#define __SHIRABE_SHIRABE_MATH_MATRIX_H__

#include <iterator>
#include <array>

#include "math/field.h"
#include "math/matrixops.h"

namespace engine
{
    namespace math
    {

		// Use private matrix algorithms from MatrixOps.h
        using namespace engine::math::PRIVATE;

		/* NOTES
		 *
         * The matrix row Iterator_t should encapsulate a well defined interval of a matrix-CField.
         * The interval is row := [i*_N, i*_n + j].
		 *
		 * Required operators:
		 *
         * operator++/operator-- : move forward or backward in the Iterator_t.
		 * begin()/end() : to determine, where to start and stop
		 * operator[] : for direct access, throws invalid_argument exception!
         * operator do access the current value this way : value = *Iterator_t;
		 *
         * May derive from the STL-Iterator_t.
		 */

        /**
         * The CMatrixRowIterator_t permits iterating a matrix row-by-row.
         *
         * @tparam T
         */
		template <typename T>
        class CMatrixRowIterator_t
		{
        public_typedefs:
            using ClassType_t = CMatrixRowIterator_t<T>                                       ;
            using ValueType_t = T                                                             ;
            using Iterator_t  = std::iterator<std::forward_iterator_tag, T, ptrdiff_t, T*, T&>;

        public_constructors:
            CMatrixRowIterator_t<T>() = default;

        public_methods:
            ValueType_t       *ptr()             { return mRow; }
            ValueType_t const  const_ptr() const { return const_cast<ValueType_t const>(mRow); }

        public_operators:
            ValueType_t const operator[](size_t aIndex)
            {
                ValueType_t *ptr = (mRow + aIndex);
                return const_cast<ValueType_t const>(*(mRow + aIndex));
			}

        private_members:
            ValueType_t* mRow;
            size_t       mRowOffset;
		};

        /**
         * Matrix base class for arbitrary m x n matrices.
         * Defines basic initialization and operations such as:
         * - Addition
         * - Scalar-multiplication
         * - Matrix-multiplication
         * - Lerping
         *
         * @tparam NRowCount
         * @tparam NColumnCount
         */
        template <
                size_t NRowCount = 4,
                size_t NColumnCount = 4
                >
        class CMatrix
            : public CField<float, sizeof(float), (NRowCount * NColumnCount), NColumnCount>
		{
        public_typedefs:
            using BaseType_t            = CField <float, sizeof(float), (NRowCount * NColumnCount), NColumnCount>;
            using ValueType_t           = typename BaseType_t::ValueType_t                                       ;
            using ClassType_t           = CMatrix<NRowCount, NColumnCount>                                       ;
            using MatrixType_t          = ClassType_t                                                            ;
            using TransposeMatrixType_t = CMatrix<NColumnCount, NRowCount>                                       ;
            using MatrixData_t          = typename BaseType_t::FieldDataType_t                                   ;

        public_constructors:
            /**
             * Default construct a matrix of size NRowCount * NColumnCount.
             */
            CMatrix<NRowCount, NColumnCount>()
                : CField<float, sizeof(float), (NRowCount * NColumnCount), NColumnCount>()
            {}

            /**
             * Construct a matrix of size NRowCount * NColumnCount from a list of values.
             * @param aSource
             */
            CMatrix<NRowCount, NColumnCount>(std::array<ValueType_t, (NRowCount * NColumnCount)> aSource)
                : BaseType_t(aSource)
            {}

            /**
             * Construct a matrix of size NRowCount * NColumnCount from a field of equal size.
             *
             * @param aOther
             */
            CMatrix<NRowCount, NColumnCount>(BaseType_t const &aOther)
                : BaseType_t(aOther)
            {}

            /**
             * Copy-Construct matrix of size NRowCount * NColumnCount from another equally sized matrix.
             *
             * @param aOther
             */
            CMatrix<NRowCount, NColumnCount>(MatrixType_t const &aOther)
                : BaseType_t(aOther)
            {}

        public_destructors:
            /**
             * Default destroy this matrix instance.
             */
            virtual ~CMatrix<NRowCount, NColumnCount>() = default;

        public_static_functions:
            /**
             * Return an identity matrix.
             * @return
             */
            static MatrixType_t identity()
            {
                std::array<ValueType_t, (NRowCount * NColumnCount)> array =
                       { 1.0, 0.0, 0.0, 0.0
                       , 0.0, 1.0, 0.0, 0.0
                       , 0.0, 0.0, 1.0, 0.0
                       , 0.0, 0.0, 0.0, 1.0 };
                return array;
			}

            /**
             * Return a fully zero-initialized matrix.
             *
             * @return
             */
            static MatrixType_t zero()
            {
                return MatrixType_t();
			}

        public_operators:
            /**
             * Conversion-Constructor: Convert to CField representation.
             */
            operator BaseType_t()
            {
                return static_cast<BaseType_t>(*this);
            }

            /**
             * Add another matrix to this one. Return the result as a copy.
             *
             * @param aOther
             * @return
             */
            ClassType_t operator+(ClassType_t const &aOther)
            {
                return ((ClassType_t(*this)) += aOther);
			}

            /**
             * Add another matrix to this one.
             *
             * @param aOther
             * @return
             */
            ClassType_t& operator +=(ClassType_t const &aOther)
            {
                _matrix_op__add((*this).ptr(), aOther.const_ptr());

				return *this;
			}

            /**
             * Subtract another matrix from this one. Return the result as a copy.
             *
             * @param aOther
             * @return
             */
            ClassType_t operator-(ClassType_t const &aOther)
            {
                return ((ClassType_t(*this)) -= aOther);
			}

            /**
             * Subtract another matrix from this one.
             *
             * @param aOther
             * @return
             */
            ClassType_t& operator -=(ClassType_t const &aOther)
            {
                _matrix_op__subtract((*this).ptr(), aOther.const_ptr());

				return *this;
            }

        public_methods:
            /**
             * Transpose this matrix instance and return it's reference.
             * Opposite to the SMMatrixTranspose functioN, the transposition is immediately applied to this instance.
             *
             * @return
             */
            TransposeMatrixType_t transpose()
            {
                TransposeMatrixType_t mat = {};

                __shirabe_math__matrix_transpose<NRowCount, NColumnCount, ValueType_t>(this->const_ptr(), mat.ptr());

				return mat;
			}

            /**
             * Return a copy of the adjoint-matrix of this instance.
             * The adjoint matrix is the transposed co-factor matrix.
             *
             * @return
             */
            TransposeMatrixType_t adjoint()
            {
                TransposeMatrixType_t const mat = this->cofactor().transpose();

                return mat;
			}

            /**
             * Calculate the cofactor representation of this matrix.
             *
             * @return
             */
            MatrixType_t cofactor()
            {
                MatrixType_t mat = MatrixType_t();

                __shirabe_math__matrix_get_cofactor_matrix<ValueType_t, NRowCount, NColumnCount>(this->const_ptr(), mat.ptr());
			}

            /**
             * Calculate the determinant of this matrix.
             *
             * @return
             */
            virtual ValueType_t const determinant() const
            {
                ValueType_t det = 0;

                if constexpr (NColumnCount == NRowCount)
                {
                    __shirabe_math__matrix_determinant_leibnitz_laplace<ValueType_t>(this->const_ptr(), NRowCount, NColumnCount, &det);
                }

                return det;
			}

            /**
             * Invert this matrix, using the Gauss-Elimination algorithm.
             *
             * @return
             */
            MatrixType_t invert()
            {
				/* 'Treppennormalformgleichung' -> Gauss-Elimination with pivoting and identity matrix transformation! */
                MatrixType_t const inv    = MatrixType_t::identity(); //MatrixType_t(*this);
                MatrixType_t const cpy    = MatrixType_t(*this);
                char               parity = 1;

                __shirabe_math__matrix_gauss_jordan<NRowCount, NColumnCount, ValueType_t>(cpy.ptr(), true, &parity, inv.ptr());

				return inv;
            }

        private_methods:
            /**
             * Algorithm: Matrix addition
             *
             * @param aLeftPointer
             * @param aRightPointer
             */
            void _matrix_op__add(
                        ValueType_t       *aLeftPointer,
                        ValueType_t const *aRightPointer) const
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

            /**
             * Algorithm: Matrix subtraction
             *
             * @param aLeftPointer
             * @param aRightPointer
             */
            void _matrix_op__subtract(
                        ValueType_t       *aLeftPointer,
                        ValueType_t const *aRightPointer) const
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

            /**
             * Algorithm: Scalar-Product
             *
             * @param aLeftPointer
             * @param aFactor
             */
            void _matrix_op__scale(
                        ValueType_t       *aLeftPointer,
                        ValueType_t const  aFactor) const
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

        /**
         * Square-Matrix specialication for N = M.
         *
         * @tparam M
         */
        template <size_t N = 4>
        class CSquareMatrix
            : public CMatrix<N, N>
		{
        public_typedefs:
            using BaseType_t   =          CMatrix<N, N>          ;
            using ClassType_t  =          CSquareMatrix<N>       ;
            using ValueType_t  = typename BaseType_t::ValueType_t;
            using MatrixData_t = typename BaseType_t::MatrixData_t;

        public_constructors:
            /**
             * Default construct a zero-square matrix.
             */
            CSquareMatrix<N>() = default;

            /**
             * Construct a square matrix from a list of values.
             *
             * @param aSource
             */
            CSquareMatrix<N>(std::array<ValueType_t, (N * N)> const &aSource)
                : CMatrix<N, N>(aSource)
			{}

            /**
             * Construct a square matrix from an equally sized regular matrix.
             *
             * @param aOther
             */
            CSquareMatrix<N>(BaseType_t const &aOther)
                : CMatrix<N, N>(aOther)
			{}

            /**
             * Construct a square matrix from another, equally sized square matrix.
             *
             * @param aOther
             */
            CSquareMatrix<N>(CSquareMatrix<N> const &aOther)
                : CMatrix<N, N>(aOther)
			{}

        public_destructors:
            /**
             * Cleanly destroy this matrix instance.
             */
            virtual ~CSquareMatrix<N>() = default;
		};

        /**
         * Square matrix specialization for N = 2.
         * Implemented, as we can use some more efficient algorithms and provide per component access.
         */
        class CMatrix2x2
            : public CSquareMatrix<2>
		{
        public_typedefs:
            using BaseType_t  = CSquareMatrix<2>                ;
            using ClassType_t = CMatrix2x2                      ;
            using ValueType_t = typename BaseType_t::ValueType_t;
            using MatrixData_t = typename BaseType_t::MatrixData_t;

        public_constructors:
            /**
             * Default construct an empty 2x2 matrix.
             */
            CMatrix2x2() = default;

            /**
             * Construct a 2x2 matrix from a list of values.
             *
             * @param aSource
             */
            CMatrix2x2(std::array<ValueType_t, 4> const &aSource)
                : CSquareMatrix<2>(aSource)
			{}

            /**
             * Construct a 2x2 matrix from another, equally sized square matrix.
             *
             * @param aOther
             */
            CMatrix2x2(BaseType_t const &aOther)
                : BaseType_t(aOther)
			{}

            /**
             * Construct a 2x2 matrix from another 2x2 matrix.
             *
             * @param aOther
             */
            CMatrix2x2(CMatrix2x2 const &aOther)
                : CSquareMatrix<2>(aOther)
			{}

        public_destructors:
            /**
             * Destroy and run...
             */
            virtual ~CMatrix2x2() = default;

        public_methods:
            // Per-Component read access.
            ValueType_t const &r00() const { return const_data().field[0]; }
            ValueType_t const &r01() const { return const_data().field[1]; }
            ValueType_t const &r10() const { return const_data().field[2]; }
            ValueType_t const &r11() const { return const_data().field[3]; }

            // Per-Component write access.
            ValueType_t const &r00(ValueType_t const &aValue) { return (data().field[0] = aValue); }
            ValueType_t const &r01(ValueType_t const &aValue) { return (data().field[1] = aValue); }
            ValueType_t const &r10(ValueType_t const &aValue) { return (data().field[2] = aValue); }
            ValueType_t const &r11(ValueType_t const &aValue) { return (data().field[3] = aValue); }

            /**
             * Calculate the 2x2 matrix's determinant applying the fish-rule.
             *
             * @return
             */
            virtual ValueType_t const determinant() const
            {
                ValueType_t det = 0;
                __shirabe_math__matrix_determinant_fishrule<ValueType_t>(this->const_ptr(), &det);

				return det;
			}
		};

        /**
         * Square matrix specialization for N = 3
         * Implemented, as we can use some more efficient algorithms and provide per component access.
         */
        class CMatrix3x3
            : public CSquareMatrix<3>
		{
        public_typedefs:
            using BaseType_t  = CSquareMatrix<3>       ;
            using ClassType_t = CMatrix3x3             ;
            using ValueType_t = BaseType_t::ValueType_t;
            using MatrixData_t = typename BaseType_t::MatrixData_t;

        public_constructors:
            /**
             * Default construct a 3x3 matrix.
             */
            CMatrix3x3() = default;

            /**
             * Construct a 3x3 matrix from a list of values.
             *
             * @param aSource
             */
            CMatrix3x3(std::array<ValueType_t, 9> aSource)
                : CSquareMatrix<3>(aSource)
			{}

            /**
             * Construct a 3x3 matrix from an equally sized square matrix.
             *
             * @param aOther
             */
            CMatrix3x3(BaseType_t const &aOther)
                : BaseType_t(aOther)
			{}

            /**
             * Copy-Construct a 3x3 matrix from another 3x3 matrix.
             *
             * @param aOther
             */
            CMatrix3x3(CMatrix3x3 const &aOther)
                : CSquareMatrix<3>(aOther)
			{}
			
        public_destructors:
            /**
             * Destroy and run
             */
            virtual ~CMatrix3x3() = default;

        public_methods:
            // Per-Component read access.
            ValueType_t const &r00() const { return const_data().field[0]; }
            ValueType_t const &r01() const { return const_data().field[1]; }
            ValueType_t const &r02() const { return const_data().field[2]; }
            ValueType_t const &r10() const { return const_data().field[3]; }
            ValueType_t const &r11() const { return const_data().field[4]; }
            ValueType_t const &r12() const { return const_data().field[5]; }
            ValueType_t const &r20() const { return const_data().field[6]; }
            ValueType_t const &r21() const { return const_data().field[7]; }
            ValueType_t const &r22() const { return const_data().field[8]; }

            // Per-Component write access.
            ValueType_t const &r00(ValueType_t const &aValue) { return (data().field[0] = aValue); }
            ValueType_t const &r01(ValueType_t const &aValue) { return (data().field[1] = aValue); }
            ValueType_t const &r02(ValueType_t const &aValue) { return (data().field[2] = aValue); }
            ValueType_t const &r10(ValueType_t const &aValue) { return (data().field[3] = aValue); }
            ValueType_t const &r11(ValueType_t const &aValue) { return (data().field[4] = aValue); }
            ValueType_t const &r12(ValueType_t const &aValue) { return (data().field[5] = aValue); }
            ValueType_t const &r20(ValueType_t const &aValue) { return (data().field[6] = aValue); }
            ValueType_t const &r21(ValueType_t const &aValue) { return (data().field[7] = aValue); }
            ValueType_t const &r22(ValueType_t const &aValue) { return (data().field[8] = aValue); }

            /**
             * Calculate the determinant of this matrix using sarrus' rule.
             *
             * @return
             */
            virtual ValueType_t const determinant() const
            {
                ValueType_t det = 0;
                __shirabe_math__matrix_determinant_sarrus<ValueType_t>(this->const_ptr(), &det);

				return det;
			}
		};

        /**
         * Square matrix specialization for N = 4
         * Implemented, as we can use some more efficient algorithms and provide per component access.
         */
        class CMatrix4x4
            : public CSquareMatrix<4>
		{
        public_typedefs:
            using BaseType_t  = CSquareMatrix<4>       ;
            using ClassType_t = CMatrix4x4             ;
            using ValueType_t = BaseType_t::ValueType_t;
            using MatrixData_t = BaseType_t::MatrixData_t;


        public_constructors:
            /**
             * Default construct a 4x4 matrix.
             */
            CMatrix4x4() = default;

            /**
             * Construct a 4x4 matrix from a list of values.
             *
             * @param aSource
             */
            CMatrix4x4(std::array<ValueType_t, 16> aSource)
                : CSquareMatrix<4>(aSource)
			{}

            /**
             * Construct a 4x4 matrix from another, equally sized square matrix.
             *
             * @param aOther
             */
            CMatrix4x4(BaseType_t const &aOther)
                : BaseType_t(aOther)
			{}

            /**
             * Construct a 4x4 for matrix from another 4x4 matrix.
             *
             * @param aOther
             */
            CMatrix4x4(CMatrix4x4 const &aOther)
                : BaseType_t(aOther)
			{}

            /**
             * Construct a 4x4 for matrix from another 4x4 matrix.
             *
             * @param aOther
             */
            CMatrix4x4(CMatrix4x4::MatrixType_t const &aOther)
                    : BaseType_t(aOther)
            {}


        public_destructors:
            /**
             * Destroy and run
             */
            virtual ~CMatrix4x4() = default;

        public_methods:
            // Per-Component read only access.
            ValueType_t const &r00() const { return const_data().field[0];  }
            ValueType_t const &r01() const { return const_data().field[1];  }
            ValueType_t const &r02() const { return const_data().field[2];  }
            ValueType_t const &r03() const { return const_data().field[3];  }
            ValueType_t const &r10() const { return const_data().field[4];  }
            ValueType_t const &r11() const { return const_data().field[5];  }
            ValueType_t const &r12() const { return const_data().field[6];  }
            ValueType_t const &r13() const { return const_data().field[7];  }
            ValueType_t const &r20() const { return const_data().field[8];  }
            ValueType_t const &r21() const { return const_data().field[9];  }
            ValueType_t const &r22() const { return const_data().field[10]; }
            ValueType_t const &r23() const { return const_data().field[11]; }
            ValueType_t const &r30() const { return const_data().field[12]; }
            ValueType_t const &r31() const { return const_data().field[13]; }
            ValueType_t const &r32() const { return const_data().field[14]; }
            ValueType_t const &r33() const { return const_data().field[15]; }

            // Per-Component write access.
            ValueType_t const &r00(ValueType_t const &aValue) { return (data().field[0]  = aValue); }
            ValueType_t const &r01(ValueType_t const &aValue) { return (data().field[1]  = aValue); }
            ValueType_t const &r02(ValueType_t const &aValue) { return (data().field[2]  = aValue); }
            ValueType_t const &r03(ValueType_t const &aValue) { return (data().field[3]  = aValue); }
            ValueType_t const &r10(ValueType_t const &aValue) { return (data().field[4]  = aValue); }
            ValueType_t const &r11(ValueType_t const &aValue) { return (data().field[5]  = aValue); }
            ValueType_t const &r12(ValueType_t const &aValue) { return (data().field[6]  = aValue); }
            ValueType_t const &r13(ValueType_t const &aValue) { return (data().field[7]  = aValue); }
            ValueType_t const &r20(ValueType_t const &aValue) { return (data().field[8]  = aValue); }
            ValueType_t const &r21(ValueType_t const &aValue) { return (data().field[9]  = aValue); }
            ValueType_t const &r22(ValueType_t const &aValue) { return (data().field[10] = aValue); }
            ValueType_t const &r23(ValueType_t const &aValue) { return (data().field[11] = aValue); }
            ValueType_t const &r30(ValueType_t const &aValue) { return (data().field[12] = aValue); }
            ValueType_t const &r31(ValueType_t const &aValue) { return (data().field[13] = aValue); }
            ValueType_t const &r32(ValueType_t const &aValue) { return (data().field[14] = aValue); }
            ValueType_t const &r33(ValueType_t const &aValue) { return (data().field[15] = aValue); }
		};

		// Transposition functions. Will always return copies and leave the passed matrix unchanged.
        CMatrix2x2 SMMatrixTranspose(CMatrix2x2 const &aMatrix);
        CMatrix3x3 SMMatrixTranspose(CMatrix3x3 const &aMatrix);
        CMatrix4x4 SMMatrixTranspose(CMatrix4x4 const &aMatrix);

		// Determinant calculation. 
		// Will use fish-rule, sarrus' rule, gauss-jordan elimination or leibniz-formula/laplace'sche Entwicklungssatz 
        CMatrix2x2::ValueType_t SMMatrixDeterminant(CMatrix2x2 const &aMatrix);
        CMatrix3x3::ValueType_t SMMatrixDeterminant(CMatrix3x3 const &aMatrix);
        CMatrix4x4::ValueType_t SMMatrixDeterminant(CMatrix4x4 const &aMatrix);

        /**
         * Multiply a MxS matrix with a SxN matrix.
         *
         * @param aLHS
         * @param aRHS
         * @return
         */
        template<
                size_t   M,
                size_t   S,
                size_t   N,
                typename val_type = typename CMatrix<M, N>::ValueType_t
                >
        CMatrix<M, N> SMCMatrixMultiply(
                CMatrix<M, S> const &aLHS,
                CMatrix<S, N> const &aRHS)
        {
            CMatrix<M, N> product = CMatrix<M, N>();

            __shirabe_math__matrix_multiply<val_type, M, S, N>(aLHS.const_ptr(), aRHS.const_ptr(), product.ptr());

			return (product);
		};

        // Square matrix multiplications.
        CMatrix2x2 SMMatrixMultiply(CMatrix2x2 const &aLHS, CMatrix2x2 const &aRHS);
        CMatrix3x3 SMMatrixMultiply(CMatrix3x3 const &aLHS, CMatrix3x3 const &aRHS);
        CMatrix4x4 SMMatrixMultiply(CMatrix4x4 const &aLHS, CMatrix4x4 const &aRHS);

        static CMatrix2x2 operator *(CMatrix2x2 const &aLHS, CMatrix2x2 const &aRHS)
        {
            return SMMatrixMultiply(aLHS, aRHS);
        }
        static CMatrix3x3 operator *(CMatrix3x3 const &aLHS, CMatrix3x3 const &aRHS)
        {
            return SMMatrixMultiply(aLHS, aRHS);
        }
        static CMatrix4x4 operator *(CMatrix4x4 const &aLHS, CMatrix4x4 const &aRHS)
        {
            return SMMatrixMultiply(aLHS, aRHS);
        }
	}
}

#endif
