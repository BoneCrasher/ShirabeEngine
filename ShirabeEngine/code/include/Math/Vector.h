#ifndef __SHIRABE_SHIRABE_MATH_VECTOR_H__
#define __SHIRABE_SHIRABE_MATH_VECTOR_H__

#include "Math/Field.h"

namespace Engine {
	namespace Math {

		template <size_t N>
		class Vector
			: public Field<float, sizeof(float), N, 1> {
		public:
			typedef typename Field<float, sizeof(float), N, 1> base_type;

			Vector()
				: Field<float, sizeof(float), N, 1>()
			{
			}

			Vector(const value_type x,
				   const value_type y)
				: Field<float, sizeof(float), N, 1>()
			{
			}

			Vector(const class_type& cpy)
				: Field<float, sizeof(float), N, 1>(cpy) {}

		public:
			inline class_type scale(const value_type factor) {
				this->operator*=(factor);

				return *this;
			}

			inline value_type length() {
				return sqrt(this->squared_length());
			}

			inline value_type squared_length() {
				value_type        len = 0;
				const value_type *ptr = this->const_ptr();

				for (size_t i = 0; i < this->size(); ++i)
					len += (ptr[i] * ptr[i]);

				return len;
			}

			inline value_type abs() {
				return this->length();
			}

			inline class_type& normalize() {
				// For now use the "paper-version" of normalization!
				this->operator/=(this->length());

				return *this;
			}
		};

		class Vector2D
			: public Vector<2> {

		public:
			Vector2D()
				: Vector<2>()
			{
				value_type *ptr = this->ptr();
				ptr[0] = 0.0f;
				ptr[1] = 0.0f;
			}

			Vector2D(const value_type x,
				 const value_type y)
				: Vector<2>({ x, y })
			{ }

			Vector2D(const class_type& cpy)
				: Vector<2>(cpy) {}

			// Return a copy of the stored x-component. Getter.
			inline const value_type x() const {
				return this->_field[0];
			}

			// Return a copy of the stored y-component. Getter.
			inline const value_type y() const {
				return this->_field[1];
			}

			// Return a FieldAccessor for the stored x-component to safely assign a new value 
			// using the assignment-operator. Setter.
			inline void x(const value_type& val) { this->_field[0] = val; }
			// Return a FieldAccessor for the stored y-component to safely assign a new value 
			// using the assignment-operator. Setter.
			inline void y(const value_type& val) { this->_field[1] = val; }
			
			static inline Vector2D right() { return Vector2D({ 1, 0 }); }
			static inline Vector2D up()    { return Vector2D({ 0, 1 }); }
		};

		template <std::size_t N>
		Vector<N> operator +(const Vector<N>& l,
							 const Vector<N>& r) {
			return operator+(static_cast<typename Vector<N>::base_type>(l),
							 static_cast<typename Vector<N>::base_type>(r));
		}

		template <std::size_t N>
		Vector<N> operator -(const Vector<N>& l,
							 const Vector<N>& r) {
			return operator-(static_cast<typename Vector<N>::base_type>(l),
							 static_cast<typename Vector<N>::base_type>(r));
		}

		template <std::size_t N>
		Vector<N> operator *(const Vector<N>& l,
							 const typename Vector<N>::value_type &f) {
			return operator*(static_cast<typename Vector<N>::base_type>(l), f);
		}

		template <std::size_t N>
		Vector<N> operator *(const typename Vector<N>::value_type &f,
							 const Vector<N>& l) {
			return operator*(static_cast<typename Vector<N>::base_type>(l), f);
		}

		template <std::size_t N>
		Vector<N> operator /(const Vector<N>& l,
							 const typename Vector<N>::value_type &f) {
			return operator/(static_cast<typename Vector<N>::base_type>(l), f);
		}

		template <std::size_t N>
		Vector<N> operator /(const typename Vector<N>::value_type &f,
							 const Vector<N>& l) {
			return operator/(static_cast<typename Vector<N>::base_type>(l), f);
		}

		class Vector3D
			: public Vector<3> {
		public:
			Vector3D() 
				: Vector<3>({ 0.0f, 0.0f, 0.0f })
			{
				value_type *ptr = this->ptr();
				ptr[2] = 0.0f;
			}

			Vector3D(const value_type x,
				 const value_type y,
				 const value_type z)
				: Vector<3>({ x, y, z })
			{
			}

			Vector3D(const class_type& cpy)
				: Vector<3>(cpy) {}

			// Return a copy of the stored x-component. Getter.
			inline const value_type x() const {
				return this->_field[0];
			}
			// Return a copy of the stored y-component. Getter.
			inline const value_type y() const {
				return this->_field[1];
			}
			// Return a copy of the stored z-component. Getter.
			inline const value_type z() const {
				return this->_field[2];
			}

			// Return a FieldAccessor for the stored x-component to safely assign a new value 
			// using the assignment-operator. Setter.
			inline void x(const value_type& val) { this->_field[0] = val; }
			// Return a FieldAccessor for the stored y-component to safely assign a new value 
			// using the assignment-operator. Setter.
			inline void y(const value_type& val) { this->_field[1] = val; }
			// Return a FieldAccessor for the stored z-component to safely assign a new value 
			// using the assignment-operator. Setter.
			inline void z(const value_type& val) { this->_field[2] = val; }

			static inline Vector3D forward() { return Vector3D({ 1, 0, 0 }); }
			static inline Vector3D right()   { return Vector3D({ 0, 1, 0 }); }
			static inline Vector3D up()      { return Vector3D({ 0, 0, 1 }); }
		};

		class Vector4D
			: public Vector<4> {
		public:
		public:
			Vector4D()
				: Vector<4>({ 0.0f, 0.0f, 0.0f, 0.0f})
			{
			}

			Vector4D(const value_type x,
				 const value_type y,
				 const value_type z,
				 const value_type w)
				: Vector<4>({ x, y, z, w })
			{
			}

			Vector4D(const class_type& cpy)
				: Vector<4>(cpy) {}


			Vector4D(const Vector3D& other,
				 const float& w = 0.0f)
				: Vector<4>({ other.x(), other.y(), other.z(), w }) {

			}

			Vector4D(const Vector2D& other,
				 const float& z = 0.0f,
				 const float& w = 0.0f)
				: Vector<4>({ other.x(), other.y(), z, w }) {

			}


			// Return a copy of the stored x-component. Getter.
			inline const value_type x() const {
				return this->_field[0];
			}
			// Return a copy of the stored y-component. Getter.
			inline const value_type y() const {
				return this->_field[1];
			}
			// Return a copy of the stored z-component. Getter.
			inline const value_type z() const {
				return this->_field[2];
			}
			// Return a copy of the stored z-component. Getter.
			inline const value_type w() const {
				return this->_field[3];
			}

			// Return a FieldAccessor for the stored x-component to safely assign a new value 
			// using the assignment-operator. Setter.
			inline void x(const value_type& val) { this->_field[0] = val; }
			// Return a FieldAccessor for the stored y-component to safely assign a new value 
			// using the assignment-operator. Setter.
			inline void y(const value_type& val) { this->_field[1] = val; }
			// Return a FieldAccessor for the stored z-component to safely assign a new value 
			// using the assignment-operator. Setter.
			inline void z(const value_type& val) { this->_field[2] = val; }
			// Return a FieldAccessor for the stored z-component to safely assign a new value 
			// using the assignment-operator. Setter.
			inline void w(const value_type& val) { this->_field[3] = val; }
		};


		// Returns a copy of the 2D dot product of two Vec2 instances.
		inline Vector2D::value_type dot(const Vector2D l, const Vector2D r)
		{
			return ((l.x() * r.x()) + (l.y() * r.y()));
		}

		// Returns a copy of the 3D dot product of two Vec3 instances.
		inline Vector3D::value_type dot(const Vector3D l, const Vector3D r)
		{
			return ((l.x() * r.x())
					+ (l.y() * r.y())
					+ (l.z() * r.z()));
		}

		// Returns a copy of the 4D dot product of two Vec4 instances.
		inline Vector4D::value_type dot(const Vector4D l, const Vector4D r)
		{
			return ((l.x() * r.x()) +
					(l.y() * r.y()) +
					(l.z() * r.z()) +
					(l.w() + r.w()));
		}

		// Returns a copy of the 3D cross product of two Vec3 instances.
		inline Vector3D cross(const Vector3D l, const Vector3D r)
		{
			return Vector3D((l.y()*r.z() - l.z()*r.y()),
				            (l.z()*r.x() - l.x()*r.z()),
							(l.x()*r.y() - l.y()*r.x()));
		}

		// Returns a scaled copy of an arbitrary Vec2 instance.
		inline Vector2D scale(const Vector2D&  vec, const Vector2D::value_type factor)
		{
			return Vector2D(vec).scale(factor);
		}

		// Returns a scaled copy of an arbitrary Vec3 instance.
		inline Vector3D scale(const Vector3D&  vec, const Vector3D::value_type factor)
		{
			return Vector3D(vec).scale(factor);
		}

		// Returns a scaled copy of an arbitrary Vec4 instance.
		inline Vector4D scale(const Vector4D&  vec, const Vector4D::value_type factor)
		{
			return Vector4D(vec).scale(factor);
		}

		template <size_t N>
		inline Vector<N> normalize(const Vector<N>& vec)
		{
			return *(Vector<N>(vec).normalize());
		}

	} // ::Math
} // ::Engine

#endif