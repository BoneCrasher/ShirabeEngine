#ifndef __SHIRABE_SHIRABE_MATH_MATRIX_H__
#define __SHIRABE_SHIRABE_MATH_MATRIX_H__

#include <iterator>

#include "Math/Field.h"
#include "Math/MatrixOps.h"

namespace Engine {
	namespace Math {

		// Use private matrix algorithms from MatrixOps.h
		using namespace Engine::Math::PRIVATE;

		/* NOTES
		 *
		 * The matrix row iterator should encapsulate a well defined interval of a matrix-field.
		 * The interval is row := [i*_n, i*_n + j].
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

		template <typename T>
		class MatrixRowIterator
		{
		public:
			typedef typename MatrixRowIterator<T> class_type;
			typedef typename T                    value_type;
			typedef typename const value_type     const_value_type;
			typedef typename std::iterator<std::forward_iterator_tag, T, ptrdiff_t, T*, T&> iterator;
			typedef typename const iterator       const_iterator;

			MatrixRowIterator<T>() {

			}

			inline iterator begin() {
				return iterator;

					// Shallow cpy only. Do not separately define!
					/*MatrixRowIterator<T, _n>(const class_type& cpy)
					{
					}*/
			}

			inline value_type *ptr() { return _row; }
			inline const_value_type const_ptr() const { return const_cast<const_value_type>(_row); }

			inline const_value_type operator[](size_t index) {
				if (index >= _n)
					throw new std::invalid_argument("Index out of accessor bounds.");

				return const_cast<const_value_type>(*(_row + index));
			}
		private:
			value_type* _row;
			size_t      _row_off;
		};

		template <size_t row_count = 4, size_t col_count = 4>
		// Matrix base class for arbitrary m x n matrices.
		// Defines basic initialization and operations such as:
		// - Addition
		// - Scalar-multiplication
		// - Matrix-multiplication
		// - Lerping
		class Matrix 
			: public Field<float, sizeof(float), (row_count * col_count), col_count>
		{
		public:
			typedef typename Field <float, sizeof(float), (row_count * col_count), col_count> base_type;
			typedef typename Matrix<row_count, col_count> class_type, matrix_type;
			typedef typename Matrix<col_count, row_count> transposed_matrix_type;
		public:
			Matrix<row_count, col_count>() 
				: Field < float, sizeof(float), (row_count*col_count), col_count >() 
			{};

			Matrix<row_count, col_count>(std::initializer_list<value_type> source) 
				: base_type(source) 
			{};

			Matrix<row_count, col_count>(const base_type& cpy_fld) 
				: base_type(cpy_fld)
			{};

			Matrix<row_count, col_count>(const matrix_type& cpy) 
				: base_type(cpy) 
			{};

			virtual ~Matrix<row_count, col_count>() {}

		public:
			static matrix_type identity() {
				matrix_type              identity = matrix_type();
				matrix_type::value_type *ptr      = identity.ptr();

				for (size_t i = 0; i < col_count; ++i) {
					ptr[i*col_count + i] = 1;
				}

				return identity;
			}

			static matrix_type zero() {
				return matrix_type();
			}

		public:
			operator base_type() { return static_cast<base_type>(*this); }

			class_type operator+(const class_type& r) {
				return ((class_type(*this)) += r);
			}

			class_type& operator +=(const class_type& r) {
				_matrix_op__add((*this).ptr(), r.const_ptr());

				return *this;
			}

			class_type operator-(const class_type& r) {
				return ((class_type(*this)) -= r);
			}

			class_type& operator -=(const class_type& r) {
				_matrix_op__subtract((*this).ptr(), r.const_ptr());

				return *this;
			}

		private:
			// Algorithm: additions
			void _matrix_op__add(value_type       *lptr, 
								 const value_type *rptr) const {
				size_t off = 0;

				for (size_t i = 0; i < row_count; ++i) {
					for (size_t j = 0; j < col_count; ++j) {
						off = i*col_count + j;
						lptr[off] += rptr[off];
					}
				}
			}

			// Algorithm: inverse addition
			void _matrix_op__subtract(value_type       *lptr, 
									  const value_type *rptr) const
			{
				size_t off = 0;

				for (size_t i = 0; i < row_count; ++i) {
					for (size_t j = 0; j < col_count; ++j)
					{
						off = i*col_count + j;
						lptr[off] -= rptr[off];
					}
				}
			}

			// Algorithm: scalar multiplication
			void _matrix_op__scale(value_type       *lptr, 
								   const value_type  rfactor) const
			{
				size_t off = 0;

				for (size_t i = 0; i < row_count; ++i)
				for (size_t j = 0; j < col_count; ++j)
				{
					off = i*col_count + j;
					lptr[off] *= rfactor;
				}
			}

		public:
			// Transpose this matrix instance and return it's reference.
			// Opposite to the SMMatrixTranspose function, 
			// the transposition is immediately applied to this instance.
			transposed_matrix_type transpose() {
				transposed_matrix_type mat;
				__shirabe_math__matrix_transpose<row_count, col_count, value_type>(this->const_ptr(), mat.ptr());
				return mat;
			}

			// Return a copy of the adjoint-matrix of this instance.
			// The adjoint matrix is the transposed co-factor matrix.
			transposed_matrix_type adjoint() {
				return this->cofactor().transpose();
			}

			matrix_type cofactor() {
				matrix_type mat = matrix_type();

				__shirabe_math__matrix_get_cofactor_matrix<value_type, row_count, col_count>(this->const_ptr(), mat.ptr());
			}

			virtual const_value_type determinant() const {
				// dirrty test
				if (col_count != row_count)
					throw new std::exception("Determinant is not properly defined for non-square matrices.");

				value_type det = 0;
				__shirabe_math__matrix_determinant_leibnitz_laplace<value_type>(this->const_ptr(), row_count, col_count, &det);

				//__shirabe_math__matrix_determinant_gauss_jordan<row_count, col_count, value_type>(this->const_ptr(), &det);

				return det;
			}

			matrix_type invert() {
				/* 'Treppennormalformgleichung' -> Gauss-Elimination with pivoting and identity matrix transformation! */
				matrix_type inv = matrix_type::identity(); //matrix_type(*this);
				matrix_type cpy = matrix_type(*this);
				char parity = 1;

				__shirabe_math__matrix_gauss_jordan<row_count, col_count, value_type>(cpy.ptr(), true, &parity, inv.ptr());

				return inv;
			}
		protected:
		};

		template <size_t n = 4>
		class SquareMatrix 
			: public Matrix<n, n>
		{
		public:
			typedef typename Matrix<n, n>          base_type;
			typedef typename SquareMatrix<n>       class_type;
			typedef typename base_type::value_type value_type;
		public:
			SquareMatrix<n>() 
				: Matrix<n, n>()
			{}

			SquareMatrix<n>(std::initializer_list<value_type> source)
				: Matrix<n, n>(source) 
			{}

			SquareMatrix<n>(const base_type& basecpy)
				: base_type(basecpy)
			{}
			SquareMatrix<n>(const SquareMatrix<n>& cpy) 
				: Matrix<n, n>(cpy) 
			{}

			virtual ~SquareMatrix<n>() {}

			virtual inline const_value_type determinant() const {
				return base_type::determinant();
			}
		};

		class Matrix2x2 
			: public SquareMatrix<2>
		{
		public:
			typedef SquareMatrix<2>       base_type;
			typedef Matrix2x2             class_type;
			typedef base_type::value_type value_type;
		public:
			Matrix2x2() 
				: SquareMatrix<2>() 
			{}

			Matrix2x2(std::initializer_list<value_type> source)
				: SquareMatrix<2>(source)
			{}

			Matrix2x2(const base_type& _basecpy) 
				: base_type(_basecpy) 
			{}

			Matrix2x2(const Matrix2x2& cpy)
				: SquareMatrix<2>(cpy)
			{}

			virtual ~Matrix2x2() {}

			inline const value_type& r00() const { return _field[0]; }
			inline const value_type& r01() const { return _field[1]; }
			inline const value_type& r10() const { return _field[2]; }
			inline const value_type& r11() const { return _field[3]; }

			inline const value_type& r00(const value_type& v) { return (_field[0] = v); }
			inline const value_type& r01(const value_type& v) { return (_field[1] = v); }
			inline const value_type& r10(const value_type& v) { return (_field[2] = v); }
			inline const value_type& r11(const value_type& v) { return (_field[3] = v); }
		public:
			virtual inline const_value_type determinant() const	{
				value_type det = 0;
				__shirabe_math__matrix_determinant_fishrule<value_type>(this->const_ptr(), &det);
				return det;
			}
		};

		class Matrix3x3 
			: public SquareMatrix<3>
		{
		public:
			typedef SquareMatrix<3> base_type;
			typedef Matrix3x3 class_type;
			typedef base_type::value_type value_type;
		public:
			Matrix3x3() 
				: SquareMatrix<3>() 
			{}

			Matrix3x3(std::initializer_list<value_type> source) 
				: SquareMatrix<3>(source) 
			{}

			Matrix3x3(const base_type& basecpy)
				: base_type(basecpy)
			{}

			Matrix3x3(const Matrix3x3& cpy)
				: SquareMatrix<3>(cpy)
			{}
			
			virtual ~Matrix3x3() {}

			inline const value_type& r00() const { return _field[0]; }
			inline const value_type& r01() const { return _field[1]; }
			inline const value_type& r02() const { return _field[2]; }
			inline const value_type& r10() const { return _field[3]; }
			inline const value_type& r11() const { return _field[4]; }
			inline const value_type& r12() const { return _field[5]; }
			inline const value_type& r20() const { return _field[6]; }
			inline const value_type& r21() const { return _field[7]; }
			inline const value_type& r22() const { return _field[8]; }

			inline const value_type& r00(const value_type& v) { return (_field[0] = v); }
			inline const value_type& r01(const value_type& v) { return (_field[1] = v); }
			inline const value_type& r02(const value_type& v) { return (_field[2] = v); }
			inline const value_type& r10(const value_type& v) { return (_field[3] = v); }
			inline const value_type& r11(const value_type& v) { return (_field[4] = v); }
			inline const value_type& r12(const value_type& v) { return (_field[5] = v); }
			inline const value_type& r20(const value_type& v) { return (_field[6] = v); }
			inline const value_type& r21(const value_type& v) { return (_field[7] = v); }
			inline const value_type& r22(const value_type& v) { return (_field[8] = v); }

		public:
			virtual inline const_value_type determinant() const {
				value_type det = 0;
				__shirabe_math__matrix_determinant_sarrus<value_type>(this->const_ptr(), &det);
				return det;
			}

		};

		class Matrix4x4 
			: public SquareMatrix<4>
		{
		public:
			typedef SquareMatrix<4>       base_type;
			typedef Matrix4x4             class_type;
			typedef base_type::value_type value_type;
		public:
			Matrix4x4()
				: SquareMatrix<4>() 
			{}

			Matrix4x4(std::initializer_list<value_type> source) 
				: SquareMatrix<4>(source) 
			{}

			Matrix4x4(const Matrix<4, 4>& basecpy) 
				: base_type(basecpy)
			{}

			Matrix4x4(const Matrix4x4& cpy) 
				: base_type(cpy) 
			{}

			virtual ~Matrix4x4() {}

			inline const value_type& r00() const { return _field[0];  }
			inline const value_type& r01() const { return _field[1];  }
			inline const value_type& r02() const { return _field[2];  }
			inline const value_type& r03() const { return _field[3];  }
			inline const value_type& r10() const { return _field[4];  }
			inline const value_type& r11() const { return _field[5];  }
			inline const value_type& r12() const { return _field[6];  }
			inline const value_type& r13() const { return _field[7];  }
			inline const value_type& r20() const { return _field[8];  }
			inline const value_type& r21() const { return _field[9];  }
			inline const value_type& r22() const { return _field[10]; }
			inline const value_type& r23() const { return _field[11]; }
			inline const value_type& r30() const { return _field[12]; }
			inline const value_type& r31() const { return _field[13]; }
			inline const value_type& r32() const { return _field[14]; }
			inline const value_type& r33() const { return _field[15]; }

			inline const value_type& r00(const value_type& v) { return (_field[0]  = v); }
			inline const value_type& r01(const value_type& v) { return (_field[1]  = v); }
			inline const value_type& r02(const value_type& v) { return (_field[2]  = v); }
			inline const value_type& r03(const value_type& v) { return (_field[3]  = v); }
			inline const value_type& r10(const value_type& v) { return (_field[4]  = v); }
			inline const value_type& r11(const value_type& v) { return (_field[5]  = v); }
			inline const value_type& r12(const value_type& v) { return (_field[6]  = v); }
			inline const value_type& r13(const value_type& v) { return (_field[7]  = v); }
			inline const value_type& r20(const value_type& v) { return (_field[8]  = v); }
			inline const value_type& r21(const value_type& v) { return (_field[9]  = v); }
			inline const value_type& r22(const value_type& v) { return (_field[10] = v); }
			inline const value_type& r23(const value_type& v) { return (_field[11] = v); }
			inline const value_type& r30(const value_type& v) { return (_field[12] = v); }
			inline const value_type& r31(const value_type& v) { return (_field[13] = v); }
			inline const value_type& r32(const value_type& v) { return (_field[14] = v); }
			inline const value_type& r33(const value_type& v) { return (_field[15] = v); }
		};

		// Transposition functions. Will always return copies and leave the passed matrix unchanged.
		Matrix2x2 SMMatrixTranspose(const Matrix2x2& matrix);
		Matrix3x3 SMMatrixTranspose(const Matrix3x3& matrix);
		Matrix4x4 SMMatrixTranspose(const Matrix4x4& matrix);

		// Determinant calculation. 
		// Will use fish-rule, sarrus' rule, gauss-jordan elimination or leibniz-formula/laplace'sche Entwicklungssatz 
		Matrix2x2::value_type SMMatrixDeterminant(const Matrix2x2& matrix);
		Matrix3x3::value_type SMMatrixDeterminant(const Matrix3x3& matrix);
		Matrix4x4::value_type SMMatrixDeterminant(const Matrix4x4& matrix);


		template<size_t m, size_t s, size_t n, typename val_type = Matrix<m, n>::value_type>
		Matrix<m, n> SMMatrixMultiply(const Matrix<m, s>& l, const Matrix<s, n>& r) {
			Matrix<m, n> product = Matrix<m, n>();

			__shirabe_math__matrix_multiply<val_type, m, s, n>(l.const_ptr(), r.const_ptr(), product.ptr());

			return (product);
		};
		Matrix2x2 SMMatrixMultiply(const Matrix2x2& l, const Matrix2x2& r);
		Matrix3x3 SMMatrixMultiply(const Matrix3x3& l, const Matrix3x3& r);
		Matrix4x4 SMMatrixMultiply(const Matrix4x4& l, const Matrix4x4& r);
	}
}

#endif