#ifndef __SHIRABE_CORE_MEMORY_ALLOC_TRACK_H__
#define __SHIRABE_CORE_MEMORY_ALLOC_TRACK_H__

#include <typeinfo>
#include <xmemory>
#include <new>

using namespace std;

namespace Engine {
	namespace Memory {
		// Creates a wrapper class around __FILE__ & __LINE__, which can then 
		// be passed to the multiplication operator as the first argument.
		// Used to track a call to new/new[] or delete/delete[]
		class AllocLocationStamp {
		public:
			char const * const _pFile;
			const size_t _line;

		public:
			AllocLocationStamp(const char *pFile, size_t line)
				: _pFile(pFile), _line(line)
			{ }

			~AllocLocationStamp() { }
		};


		class AllocationTracker {
		private:
			AllocationTracker();

			static inline AllocationTracker *getInstance();

		public:
			~AllocationTracker();
			static AllocationTracker *_instance;

		public:
			template <typename T>
			static inline void TrackAllocation(T* pObj, AllocLocationStamp& stamp, const char *type_name)
			{
			};

			template <typename T>
			static inline void UntrackAllocation(T* pObj, AllocLocationStamp& stamp, const char *type_name)
			{

			};
		};

		// Templated multiplication-operator facilitating the standard use of new in 
		// conjunction with allocation tracking frameworks!
		template <typename T>
		inline T* operator * (AllocLocationStamp &stamp, T *pObj)
		{
			AllocationTracker::TrackAllocation(pObj, stamp, typeid(T).name());

			return pObj;
		}

		/*// Templated multiplication-operator facilitating the standard use of new in
		// conjunction with allocation tracking frameworks!
		template <typename T>
		inline T* operator / ( AllocLocationStamp &stamp,  T *pObj)
		{
			AllocationTracker::UntrackAllocation(pObj, stamp, typeid(T).name());

			return pObj;
		}*/
	}
}

#endif