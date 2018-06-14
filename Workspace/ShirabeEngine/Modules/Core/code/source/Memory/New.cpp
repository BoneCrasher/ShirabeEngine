#include "New.h"

#undef new

// DO NOT FORGET TO USE THE ALLOC_TYPE-Flags
// ALLOC_TYPE_ARRAY && ALLOC_TYPE_SINGLE_INSTANCE (DEFAULT)

/*
void* operator new( std::size_t size) throw(std::bad_alloc)
{
#if defined(__SHIRABE_MEMORY__USE_MEMORY_POOL__)
#else
#endif

	return ::operator new(size);
};

void* operator new[]( std::size_t size) throw(std::bad_alloc)
{
#if defined(__SHIRABE_MEMORY__USE_MEMORY_POOL__)
#else
#endif

	return ::operator new[](size);
};

void operator delete( void *p) throw()
{
#if defined(__SHIRABE_MEMORY__USE_MEMORY_POOL__)
#else
#endif
	::operator delete(p);
};

void operator delete[]( void *p) throw()
{
#if defined(__SHIRABE_MEMORY__USE_MEMORY_POOL__)
#else
#endif
	::operator delete(p);
};*/