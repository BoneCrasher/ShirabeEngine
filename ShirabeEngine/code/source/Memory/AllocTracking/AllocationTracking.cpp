#include "Memory/AllocTracking/AllocationTracking.h"

namespace Engine
{
	namespace Memory
	{
		AllocationTracker::AllocationTracker() {};

		AllocationTracker* AllocationTracker::getInstance()
		{
			if (_instance == NULL)
				_instance = new AllocationTracker();

			return _instance;
		}

		AllocationTracker *AllocationTracker::_instance = NULL;

	}
}