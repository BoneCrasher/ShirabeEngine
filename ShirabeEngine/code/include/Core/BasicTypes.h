#ifndef __SHIRABE_BASICTYPES_H__
#define __SHIRABE_BASICTYPES_H__

#include <array>
#include <math.h>
#include <cmath>

#include "Platform/Platform.h"

namespace Engine {

#ifdef min 
#undef min
#endif

#ifdef max
#undef max
#endif

  // #define DefineContainerSpecializations(name)         \
	// 	        typedef name<char>            name##c;   \
	// 	        typedef name<short>           name##s;   \
	// 	        typedef name<int>             name##i;   \
	// 	        typedef name<long>            name##l;   \
	// 	        typedef name<float>           name##f;	 \
	// 	        typedef name<double>          name##d;	 \
	// 	        typedef name<unsigned char>   name##uc;  \
	// 	        typedef name<unsigned short>  name##us;  \
	// 	        typedef name<unsigned int>    name##ui;  \
	// 	        typedef name<unsigned long>   name##ul;  

#define DefineContainerSpecializations(name)       \
	        using name##c  = name<char>;           \
	        using name##s  = name<short>;          \
	        using name##i  = name<int>;            \
	        using name##l  = name<long>;           \
	        using name##f  = name<float>;          \
	        using name##d  = name<double>;         \
	        using name##uc = name<unsigned char>;  \
	        using name##us = name<unsigned short>; \
	        using name##ui = name<unsigned int>;   \
	        using name##ul = name<unsigned long>;  

#define DefinePermutationAccessor1D(a)          inline Vec1D<T> a()          const { return Vec1D<T>(a());          }
#define DefinePermutationAccessor2D(a, b)       inline Vec2D<T> a##b()       const { return Vec2D<T>(a(), b());       }
#define DefinePermutationAccessor3D(a, b, c)    inline Vec3D<T> a##b##c()    const { return Vec3D<T>(a(), b(), c());    }
#define DefinePermutationAccessor4D(a, b, c, d) inline Vec4D<T> a##b##c##d() const { return Vec4D<T>(a(), b(), c(), d()); }

  template <typename T, std::size_t N>
  struct VecND {
  protected:
    std::array<T, N> m_values;

  public:
    VecND() { }

    VecND(const std::initializer_list<T>& init)
      : m_values() {

      int k=0;
      for(const T& v : init) {
        if(k >= N)
          break;

        m_values[k++] = v;
      }
    }

    template <typename U>
    VecND(const VecND<U, N>& other) {
      this->operator=<U, N>(other);
    }

    template <typename U, std::size_t NU>
    VecND(const VecND<U, NU>& other) {
      this->operator=<U, NU>(other);
    }

    template <typename U>
    inline VecND<T, N>& operator =(const VecND<typename std::enable_if<std::is_convertible<U, T>::value, U>::type, N>& other) {
      m_values.fill(0);
      std::copy(other.values().begin(), other.values().end(), m_values.begin());
      return *this;
    }


    template <typename U, std::size_t NU>
    inline VecND<T, N>& operator =(const VecND<typename std::enable_if<std::is_convertible<U, T>::value, U>::type, NU>& other) {
      m_values.fill(0);
      std::copy(other.values().begin(), other.values().begin() + (std::min(N, NU)), m_values.begin());
      return *this;
    }


    inline const T& operator[](const std::size_t& index) const { if(index < 0 || index >= N) throw EEngineStatus::OutOfBounds; return m_values[index]; }
    inline       T& operator[](const std::size_t& index) { if(index < 0 || index >= N) throw EEngineStatus::OutOfBounds; return m_values[index]; }
    inline bool     operator==(const VecND<T, N>& other) const { return (m_values == other.m_values); }

    inline const std::array<T, N>& values() const {
      return m_values;
    }
  };

  template <typename T>
  struct Vec1D
    : public VecND<T, 1> {

    explicit Vec1D()
      : VecND<T, 1>(T()) {}

    explicit Vec1D(const Vec1D& other)
      : VecND<T, 1>(other.x()) {}

    explicit Vec1D(const T &x)
      : VecND<T, 1>(x) {}

    inline const T& x() const {
      return m_values[0];
    }

    inline T& x() {
      return m_values[0];
    }
  };
  DefineContainerSpecializations(Vec1D);

  template <typename T>
  struct Vec2D
    : VecND<T, 2> {

    Vec2D()
      : VecND<T, 2>({ T(), T() }) {}

    Vec2D(const Vec2D& other)
      : VecND<T, 2>({ other.x(), other.y() }) {}

    Vec2D(const T &x,
      const T &y)
      : VecND<T, 2>({ x, y }) {}

    Vec2D(const Vec1D<T>& other)
      : VecND<T, 2>({ other.x(), T() }) {}

    inline const T& x() const {
      return m_values[0];
    }

    inline T& x() {
      return const_cast<T&>(static_cast<const Vec2D<T>*>(this)->x());
    }

    inline const T& y() const {
      return m_values[1];
    }

    inline T& y() {
      return const_cast<T&>(static_cast<const Vec2D<T>*>(this)->y());
    }

    DefinePermutationAccessor2D(x, y);
    DefinePermutationAccessor2D(y, x);
  };
  DefineContainerSpecializations(Vec2D);

  template <typename T>
  struct Vec3D
    : public VecND<T, 3> {

    Vec3D()
      : VecND<T, 3>(T(), T(), T()) {}

    Vec3D(const T &x,
      const T &y,
      const T &z)
      : VecND<T, 3>({ x, y, z }) {}

    Vec3D(const Vec1D<T>& other)
      : VecND<T, 3>({ other.x(), T(), T() }) {}

    Vec3D(const Vec2D<T>& other)
      : VecND<T, 3>({ other.x(), other.y(), T() }) {}

    Vec3D(const Vec3D<T>& other)
      : VecND<T, 3>({ other.xy(), other.y(), other.z() }) {}

    inline const T& x() const {
      return m_values[0];
    }
    inline T& x() {
      return const_cast<T&>(static_cast<const Vec3D<T>*>(this)->x());
    }

    inline const T& y() const {
      return m_values[1];
    }
    inline T& y() {
      return const_cast<T&>(static_cast<const Vec3D<T>*>(this)->y());
    }

    inline const T& z() const {
      return m_values[2];
    }
    inline T& z() {
      return const_cast<T&>(static_cast<const Vec3D<T>*>(this)->z());
    }

    DefinePermutationAccessor2D(x, y);
    DefinePermutationAccessor2D(y, x);
    DefinePermutationAccessor2D(x, z);
    DefinePermutationAccessor2D(z, x);
    DefinePermutationAccessor2D(y, z);
    DefinePermutationAccessor2D(z, y);

    DefinePermutationAccessor3D(x, y, z);
    DefinePermutationAccessor3D(x, z, y);
    DefinePermutationAccessor3D(y, x, z);
    DefinePermutationAccessor3D(y, z, x);
    DefinePermutationAccessor3D(z, x, y);
    DefinePermutationAccessor3D(z, y, x);
  };
  DefineContainerSpecializations(Vec3D);

  template <typename T>
  struct Vec4D
    : public VecND<T, 4> {
    Vec4D()
      : VecND<T, 4>({ T(), T(), T(), T() }) {}

    Vec4D(const T &x,
      const T &y,
      const T &z,
      const T &w)
      : VecND<T, 4>({ x, y, z, T() }) {}
    Vec4D(const Vec1D<T>& other)
      : VecND<T, 4>({ other.x(), T(), T(), T() }) {}

    Vec4D(const Vec2D<T>& other)
      : VecND<T, 4>({ other.x(), other.y(), T(), T() }) {}

    Vec4D(const Vec3D<T>& other)
      : VecND<T, 4>({ other.x(), other.y(), other.z(), T() }) {}

    inline const T& x() const {
      return m_values[0];
    }
    inline T& x() {
      return const_cast<T&>(static_cast<const Vec4D<T>*>(this)->x());
    }

    inline const T& y() const {
      return m_values[1];
    }
    inline T& y() {
      return const_cast<T&>(static_cast<const Vec4D<T>*>(this)->y());
    }

    inline const T& z() const {
      return m_values[2];
    }
    inline T& z() {
      return const_cast<T&>(static_cast<const Vec4D<T>*>(this)->z());
    }

    inline const T& w() const {
      return m_values[3];
    }
    inline T& w() {
      return const_cast<T&>(static_cast<const Vec4D<T>*>(this)->w());
    }

    DefinePermutationAccessor2D(x, y);
    DefinePermutationAccessor2D(y, x);
    DefinePermutationAccessor2D(x, z);
    DefinePermutationAccessor2D(z, x);
    DefinePermutationAccessor2D(y, z);
    DefinePermutationAccessor2D(z, y);
    DefinePermutationAccessor2D(x, w);
    DefinePermutationAccessor2D(w, x);
    DefinePermutationAccessor2D(y, w);
    DefinePermutationAccessor2D(w, y);
    DefinePermutationAccessor2D(z, w);
    DefinePermutationAccessor2D(w, z);

    DefinePermutationAccessor3D(x, y, z);
    DefinePermutationAccessor3D(x, z, y);
    DefinePermutationAccessor3D(y, x, z);
    DefinePermutationAccessor3D(y, z, x);
    DefinePermutationAccessor3D(z, x, y);
    DefinePermutationAccessor3D(z, y, x);
    DefinePermutationAccessor3D(x, y, w);
    DefinePermutationAccessor3D(x, w, y);
    DefinePermutationAccessor3D(x, z, w);
    DefinePermutationAccessor3D(x, w, z);
    DefinePermutationAccessor3D(y, x, w);
    DefinePermutationAccessor3D(y, w, x);
    DefinePermutationAccessor3D(y, z, w);
    DefinePermutationAccessor3D(y, w, z);
    DefinePermutationAccessor3D(z, x, w);
    DefinePermutationAccessor3D(z, w, x);
    DefinePermutationAccessor3D(z, y, w);
    DefinePermutationAccessor3D(z, w, y);
    DefinePermutationAccessor3D(w, x, y);
    DefinePermutationAccessor3D(w, y, x);
    DefinePermutationAccessor3D(w, x, z);
    DefinePermutationAccessor3D(w, z, x);
    DefinePermutationAccessor3D(w, y, z);
    DefinePermutationAccessor3D(w, z, y);

    DefinePermutationAccessor4D(x, y, z, w);
    DefinePermutationAccessor4D(x, y, w, z);
    DefinePermutationAccessor4D(x, z, y, w);
    DefinePermutationAccessor4D(x, z, w, y);
    DefinePermutationAccessor4D(x, w, y, z);
    DefinePermutationAccessor4D(x, w, z, y);
    DefinePermutationAccessor4D(y, x, z, w);
    DefinePermutationAccessor4D(y, x, w, z);
    DefinePermutationAccessor4D(y, z, x, w);
    DefinePermutationAccessor4D(y, z, w, x);
    DefinePermutationAccessor4D(y, w, x, z);
    DefinePermutationAccessor4D(y, w, z, x);
    DefinePermutationAccessor4D(z, x, y, w);
    DefinePermutationAccessor4D(z, x, w, y);
    DefinePermutationAccessor4D(z, y, x, w);
    DefinePermutationAccessor4D(z, y, w, x);
    DefinePermutationAccessor4D(z, w, x, y);
    DefinePermutationAccessor4D(z, w, y, x);
    DefinePermutationAccessor4D(w, x, y, z);
    DefinePermutationAccessor4D(w, x, z, y);
    DefinePermutationAccessor4D(w, y, x, z);
    DefinePermutationAccessor4D(w, y, z, x);
    DefinePermutationAccessor4D(w, z, x, y);
    DefinePermutationAccessor4D(w, z, y, x);
  };
  DefineContainerSpecializations(Vec4D);

  struct Rect {
    Vec2Dl m_position;
    Vec2Dl size;

    Rect()
      : m_position(0, 0)
      , size(0, 0)
    {}

    Rect(const long &x,
      const long &y,
      const long &width,
      const long &height)
      : m_position(x, y)
      , size(width, height)
    {
    }

    explicit Rect(const Vec2Dl& pos,
      const Vec2Dl& sz)
      : m_position(pos),
      size(sz)
    {
    }
  };

#define FRAMEGRAPH_RESOURCERANGE_REMAINING -1

  struct SHIRABE_LIBRARY_EXPORT Range {
    inline
      Range()
      : offset(0)
      , length(0)
    {}

    inline
      Range(
        uint32_t inOffset,
        int32_t  inLength)
      : offset(inOffset)
      , length(inLength)
    {}

    inline
      bool overlapsWith(Range const&other) const
    {
      // Pretest: Both ranges must be at least 1 unit in length to check anything...
      if(!(length && other.length))
        return false;

      bool overlap = false;

      if(offset == other.offset)
        overlap = true;
      else if(offset < other.offset)
        overlap = ( (((int32_t)(offset + length)) - ((int32_t)other.offset)) > 0);
      else
        overlap = ( (((int32_t)(other.offset + other.length)) - ((int32_t)offset)) > 0);

      return overlap;
    }

    uint32_t
      offset;
    int32_t
      length;
  };

}

#endif