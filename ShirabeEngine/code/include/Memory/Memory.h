#ifndef __SHIRABE_CORE_MEMORY_H__
#define __SHIRABE_CORE_MEMORY_H__

#include <xmemory>
#include <map>

#include "Memory/New.h"
#include "Memory/Types.h"

namespace Engine {
	namespace Memory {

		template<typename T, std::size_t S>
		std::size_t SizeOf(T(&)[S]) {
			return S;
		}

		template <class T>
		void SAFE_RELEASE(T** p)
		{
			if (*p != NULL)
			{
				(*p)->Release();
				(*p) = NULL;
			}
		};

		template <class T>
		void SAFE_RELEASE_COMPTR(T p)
		{
			if (p != NULL)
			{
				(p).Release();
				(p) = NULL;
			}
		};

		template <class T>
		void SAFE_DESTROY(T** p)
		{
			if (*p != NULL)
			{
				(*p)->Destroy();
				(*p) = NULL;
			}
		};


		template <class T>
		void SAFE_DELETE(T** p)
		{
			if (*p != NULL)
			{
				delete (*p);
				(*p) = NULL;
			}
		};

		template <class T>
		void SAFE_DELETE_ARRAY(T** p)
		{
			if (*p != NULL)
			{
				delete[](*p);
				(*p) = NULL;
			}
		};

		template <typename T, typename U>
		void SAFE_RELEASE_MAP(std::map<T, U>* p)
		{
			if (p != NULL)
			{
				if (p->size() > 0)
					for (int i; i < p->size(); i++)
						SAFE_RELEASE(p->at(i));
				p->clear();
			}
		};
	}
}
#endif