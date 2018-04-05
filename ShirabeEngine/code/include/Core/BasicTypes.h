#ifndef __SHIRABE_BASICTYPES_H__
#define __SHIRABE_BASICTYPES_H__

#include <array>
#include <math.h>
#include <cmath>
#include <stdint.h>

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

  #define DefinePermutationAccessor1D(a)          inline TVector1D<T> a()          const { return TVector1D<T>(a());          }
  #define DefinePermutationAccessor2D(a, b)       inline TVector2D<T> a##b()       const { return TVector2D<T>(a(), b());       }
  #define DefinePermutationAccessor3D(a, b, c)    inline TVector3D<T> a##b##c()    const { return TVector3D<T>(a(), b(), c());    }
  #define DefinePermutationAccessor4D(a, b, c, d) inline TVector4D<T> a##b##c##d() const { return TVector4D<T>(a(), b(), c(), d()); }

  template <typename T, std::size_t N>
  struct TVectorND {
  protected:
    std::array<T, N> m_values;

  public:
    TVectorND();
    TVectorND(const std::initializer_list<T>& init);

    template <typename U>
    TVectorND(TVectorND<U, N> const& other);

    template <typename U, std::size_t NU>
    TVectorND(TVectorND<U, NU> const& other);

    template <typename U>
    TVectorND<T, N>& operator =(TVectorND<typename std::enable_if<std::is_convertible<U, T>::value, U>::type, N> const&other);


    template <typename U, std::size_t NU>
    TVectorND<T, N>& operator =(TVectorND<typename std::enable_if<std::is_convertible<U, T>::value, U>::type, NU> const&other);
   
    T const& operator[](std::size_t     const& index) const;
    T &      operator[](std::size_t     const& index);
    bool     operator==(TVectorND<T, N> const& other) const;

    std::array<T, N> const& values() const;
  };

  
  template <typename T>
  struct TVector1D
    : public TVectorND<T, 1> {

    explicit TVector1D()
      : TVectorND<T, 1>(T()) 
    {}

    explicit TVector1D(TVector1D<T> const& other)
      : TVectorND<T, 1>(other.x()) 
    {}

    explicit TVector1D(T const&x)
      : TVectorND<T, 1>(x)
    {}

    inline const T& x() const {
      return m_values[0];
    }

    inline T& x() {
      return m_values[0];
    }
  };

  template <typename T>
  struct TVector2D
    : TVectorND<T, 2> {

    TVector2D()
      : TVectorND<T, 2>({ T(), T() }) 
    {}

    TVector2D(const TVector2D<T>& other)
      : TVectorND<T, 2>({ other.x(), other.y() }) 
    {}

    TVector2D(
      const T &x,
      const T &y)
      : TVectorND<T, 2>({ x, y }) 
    {}

    TVector2D(const TVector1D<T>& other)
      : TVectorND<T, 2>({ other.x(), T() }) 
    {}

    inline const T& x() const {
      return m_values[0];
    }

    inline T& x() {
      return const_cast<T&>(static_cast<TVector2D<T> const*>(this)->x());
    }

    inline const T& y() const {
      return m_values[1];
    }

    inline T& y() {
      return const_cast<T&>(static_cast<TVector2D<T> const*>(this)->y());
    }

    DefinePermutationAccessor2D(x, y);
    DefinePermutationAccessor2D(y, x);
  };

  template <typename T>
  struct TVector3D
    : public TVectorND<T, 3> {

    TVector3D()
      : TVectorND<T, 3>(T(), T(), T()) 
    {}

    TVector3D(const T &x,
      const T &y,
      const T &z)
      : TVectorND<T, 3>({ x, y, z }) 
    {}

    TVector3D(const TVector1D<T>& other)
      : TVectorND<T, 3>({ other.x(), T(), T() }) 
    {}

    TVector3D(const TVector2D<T>& other)
      : TVectorND<T, 3>({ other.x(), other.y(), T() }) 
    {}

    TVector3D(const TVector3D<T>& other)
      : TVectorND<T, 3>({ other.xy(), other.y(), other.z() })
    {}

    inline const T& x() const {
      return m_values[0];
    }
    inline T& x() {
      return const_cast<T&>(static_cast<TVector3D<T> const*>(this)->x());
    }

    inline const T& y() const {
      return m_values[1];
    }
    inline T& y() {
      return const_cast<T&>(static_cast<TVector3D<T> const*>(this)->y());
    }

    inline const T& z() const {
      return m_values[2];
    }
    inline T& z() {
      return const_cast<T&>(static_cast<TVector3D<T> const*>(this)->z());
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

  template <typename T>
  struct TVector4D
    : public TVectorND<T, 4> {
    TVector4D()
      : VecND<T, 4>({ T(), T(), T(), T() })
    {}

    TVector4D(const T &x,
      const T &y,
      const T &z,
      const T &w)
      : TVectorND<T, 4>({ x, y, z, T() })
    {}

    TVector4D(const TVector1D<T>& other)
      : TVectorND<T, 4>({ other.x(), T(), T(), T() }) 
    {}

    TVector4D(const TVector2D<T>& other)
      : TVectorND<T, 4>({ other.x(), other.y(), T(), T() }) 
    {}

    TVector4D(const TVector3D<T>& other)
      : TVectorND<T, 4>({ other.x(), other.y(), other.z(), T() })
    {}

    inline const T& x() const {
      return m_values[0];
    }
    inline T& x() {
      return const_cast<T&>(static_cast<TVector4D<T> const*>(this)->x());
    }

    inline const T& y() const {
      return m_values[1];
    }
    inline T& y() {
      return const_cast<T&>(static_cast<TVector4D<T> const*>(this)->y());
    }

    inline const T& z() const {
      return m_values[2];
    }
    inline T& z() {
      return const_cast<T&>(static_cast<TVector4D<T> const*>(this)->z());
    }

    inline const T& w() const {
      return m_values[3];
    }
    inline T& w() {
      return const_cast<T&>(static_cast<TVector4D<T> const*>(this)->w());
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

  using Vector1D = TVector1D<float>;
  using Vector2D = TVector2D<float>;
  using Vector3D = TVector3D<float>;
  using Vector4D = TVector4D<float>;

  struct SHIRABE_LIBRARY_EXPORT Rect {
    TVector2D<uint16_t> position;
    TVector2D<uint16_t> size;

    Rect();
    Rect(
      long const&x,
      long const&y,
      long const&width,
      long const&height);

    explicit Rect(
      TVector2D<uint16_t> const& pos,
      TVector2D<uint16_t> const& sz);
  };

  #define FRAMEGRAPH_RESOURCERANGE_REMAINING -1

  struct SHIRABE_LIBRARY_EXPORT Range {
    Range();

    Range(
      uint32_t const&inOffset,
      int32_t  const&inLength);

    bool overlapsWith(Range const&other) const;

    uint32_t
      offset;
    int32_t
      length;
  };

  #include "Core/BasicTypes.tpp" // Must be included from within namespace.

}

#endif