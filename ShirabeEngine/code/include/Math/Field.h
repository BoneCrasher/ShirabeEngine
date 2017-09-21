#ifndef __SHIRABE_SHIRABE_MATH_FIELD_H__
#define __SHIRABE_SHIRABE_MATH_FIELD_H__

#include <iterator>
#include <initializer_list>

#include "Core/EngineTypeHelper.h"

namespace Engine {
	namespace Math {

#define FIELD_DEFAULT_SIZE 1

		template <
			typename T,
			std::size_t bytesize = sizeof(T),
			std::size_t N = FIELD_DEFAULT_SIZE,
			std::size_t S = 1
		>
			// Defines a templated, non-growable field-type to hold certain type of data as 
			// a vector-structure, internally stored as an array.
			class Field
			: public std::_Container_base 
		{
			public:
				typedef typename Field<T, bytesize, N, S> class_type;
				typedef typename T                        value_type;
				typedef typename const value_type         const_value_type;

				Field<T, bytesize, N, S>
					(const std::size_t size  = N,
					 const std::size_t stride = S)
				{
					//_field = new T[size];
					memset(_field, 0, (bytesize * size));
				};

				Field<T, bytesize, N, S>
					(std::initializer_list<T> source,
					 const std::size_t             size  = N,
					 const std::size_t             stride = S)
				{
					//_field = new T[size];

					size_t i = 0;
					std::initializer_list<T>::iterator it;
					for (it = source.begin(); it != source.end(); ++it) {
						_field[i] = *it;
						++i;
					}
				};

				Field<T, bytesize, N, S>
					(const Field<T, bytesize, N, S>& cpy)
				{
					//this->_field = NULL;

					this->assign(cpy);
				}

				~Field<T, bytesize, N, S>() {
					//SAFE_DELETE_ARRAY<T>(&(this->_field));
				}

			public:
				// Assign another field and overwrite contained values.
				class_type& operator= (const class_type& right) {
					this->assign(right);

					return *this;
				}

				// Access the element at the given index.
				const T& operator[] (const std::size_t i) const {
					if (N > 0) {
						if (N > i) {
							return *(_field + i);
						}
					}

					throw std::exception("Out of range");
				}

				T& operator[] (const std::size_t i) {
					return const_cast<T&>(static_cast<const class_type *>(this)->operator[](i));
				}
				
				// Add another vector to this instance.
				// There are no range checks and no clamping applied.
				void operator+=(const class_type& r) {
					for (size_t i = 0; i < N; ++i)
						this->_field[i] += r[i];
				}

				// Subtract another vector from this instance.
				// There are no range checks and no clamping applied.
				void operator-=(const class_type& r) {
					for (size_t i = 0; i < N; ++i)
						this->_field[i] -= r[i];
				}

				// Multiply this vector with the passed factor.
				// There are no range checks and no clamping applied.
				void operator*=(const T factor) {
					for (size_t i = 0; i < N; ++i)
						this->_field[i] *= factor;
				}


				// Dividy this vector by the passed factor.
				// There are no range checks and no clamping applied.
				void operator/=(const T factor) {
					this->operator*=((1 / factor));
				}

			protected:
				// Assign another matrix to this instance.
				// Internally theres only a copy operation taking place 
				// overriding the old data of this instance, if any!
				inline void assign(const class_type& r) {
					// Only recreate if NULL. Use available allocated memory instead!
					//if (this->_field == NULL)
						//this->_field = new T[N];

					memcpy_s(this->_field, (N * bytesize), r.const_ptr(), (N * bytesize));
				}

			public:
				// Return a const pointer to the internal data array.
				// Used for read-only access.
				inline const T *const_ptr() const { return &this->_field[0]; };

				// Return a pointer to the internal data array.
				// Used for read-write access.
				inline T *ptr() { return this->_field; };

				// Return the total number of elements in the field.
				inline const std::size_t size() const { return N; };

				// Return the size of a single element of the field in bytes.
				inline const std::size_t byte_size() const { return bytesize; };

				// Return the number of (imaginary) columns in the field.
				// Dividing the size by the byte_stride should return the number of (imaginary) rows!
				// If the field should contain an arbitrary number of elements, pass 1 for the byte_stride.
				inline const std::size_t byte_stride() const { return S; };

			public:
				std::string toString() {
					std::stringstream ss;
					for (size_t i = 0; i < N; i += S) {
						for (size_t j = 0; j < S; ++j) {
							ss << ((j) == 0) ? "" : ", ";
							ss << _field[i + j];
						}

						ss << ",\n";
					}

					ss << "\n";

					return ss.str();
				}

			protected:
				T _field[N * bytesize];
		};

		template <
			typename T,
			size_t bytesize = sizeof(T),
			size_t N = FIELD_DEFAULT_SIZE,
			size_t S = 1
		>
		// Return a copy of this vector which the passed vector was added to.
		// There are no range checks and no clamping applied.
		Field<T, bytesize, N, S>
			operator+(const Field<T, bytesize, N, S>& l,
					  const Field<T, bytesize, N, S>& r) {

			Field<T, bytesize, N, S> cpy
				= Field<T, bytesize, N, S>(l);
		
			T *field = cpy.ptr();

			for (size_t i = 0; i < N; ++i)
				field[i] += r[i];

			field = NULL;

			return cpy;
		}

		template <
			typename T,
			size_t bytesize = sizeof(T),
			size_t N = FIELD_DEFAULT_SIZE,
			size_t S = 1
		>
		// Return a copy of this vector which the passed vector was subtracted from.
		// There are no range checks and no clamping applied.
			Field<T, bytesize, N, S>
			operator-(const Field<T, bytesize, N, S>& l,
					  const Field<T, bytesize, N, S>& r) {

			Field<T, bytesize, N, S> cpy
				= Field<T, bytesize, N, S>(l);

			T *field = cpy.ptr();

			for (size_t i = 0; i < N; ++i)
				field[i] -= r[i];

			field = NULL;

			return cpy;
		}

		template <
			typename T,
			size_t bytesize = sizeof(T),
			size_t N = FIELD_DEFAULT_SIZE,
			size_t S = 1
		>
		// Return a multiplied copy of this vector applying the passed factor.
		// There are no range checks and no clamping applied.
			Field<T, bytesize, N, S>
			operator*(const Field<T, bytesize, N, S>& l,
					  const T factor) {

			Field<T, bytesize, N, S> cpy
				= Field<T, bytesize, N, S>(l);

			T *field = cpy.ptr();
		
			for (size_t i = 0; i < N; ++i)
				field[i] *= factor;

			field = NULL;

			return cpy;
		}

		template <
			typename T,
			size_t bytesize = sizeof(T),
			size_t N = FIELD_DEFAULT_SIZE,
			size_t S = 1
		>
			// Return a multiplied copy of this vector applying the passed factor.
			// There are no range checks and no clamping applied.
			Field<T, bytesize, N, S>
			operator*(const T factor, 
					  const Field<T, bytesize, N, S>& l) {

			return operator*(l, factor);
		}

		template <
			typename T,
			size_t bytesize = sizeof(T),
			size_t N = FIELD_DEFAULT_SIZE,
			size_t S = 1
		>
		// Return a divided copy of this vector applying the passed factor.
		// There are no range checks and no clamping applied.
			Field<T, bytesize, N, S>
			operator/(const Field<T, bytesize, N, S>& field,
					  const T factor) {
			return operator*(field, (1 / factor));
		}

		template <
			typename T,
			size_t bytesize = sizeof(T),
			size_t N = FIELD_DEFAULT_SIZE,
			size_t S = 1
		>
			// Return a divided copy of this vector applying the passed factor.
			// There are no range checks and no clamping applied.
			Field<T, bytesize, N, S>
		operator/(const T factor,
			      const Field<T, bytesize, N, S>& field) {
			return operator*(field, (1 / factor));
		}

	}
}
#endif