#include "Core/Memory/AllocTracking/AllocationTracking.h"

namespace Engine
{
	namespace Memory
	{
		AllocationTracker::AllocationTracker() {};

		AllocationTracker* AllocationTracker::getInstance()
		{
			if (m_instance == NULL)
				m_instance = new AllocationTracker();

			return m_instance;
		}

		AllocationTracker *AllocationTracker::m_instance = NULL;

	}
}