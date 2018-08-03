#include "core/benchmarking/timer/timer.h"

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CTimer::CTimer()
    {
        mConversionConstant = m_initial = m_current = m_elapsed = 0;

		if (!CheckEngineError(initialize()))
			return;
	};

    CTimer::~CTimer() {
        cleanup();
	};

    void CTimer::setTimeInterface(const TimePtr& pInterface)
	{
		if (!pInterface)
			throw new std::invalid_argument("Cannot set time-interface to NULL.");

		m_timeInterface = pInterface;
	};

    EEngineStatus CTimer::initialize()
	{
		EEngineStatus result = EEngineStatus::Ok;

		setTimeInterface(makeCStdSharedPtr<internal_time_type>());

		if (!m_timeInterface) {
			printf("FATAL_ERROR: Timer::initialize: Assigning by YTime::getInterface() failed. Interface-pointer is NULL.");
			result = EEngineStatus::Timer_NoPlatformTimeInstance;
		} else {
			if (CheckEngineError(result = m_timeInterface->initialize())) {
				// ...
			}

			// Get conversion constant
            if (CheckEngineError(result = m_timeInterface->getConversionConstant(mConversionConstant))) {
				printf("FATAL_ERROR: Timer::initialize: An error occured on requesting the proprietary time interface conversion constant from the time interface.\nAt: %s:%s", __FILE__, __LINE__); // might cause exceptions
			}
			// get initial timestamp
			if (CheckEngineError(result = m_timeInterface->getTimestamp(m_initial))) {
				printf("ERROR: Timer::initialize: An error occured on requesting the current timestamp from the time interface.\nAt: %s:%s", __FILE__, __LINE__); // might cause exceptions
			} else
				m_current = m_initial; // init current for calculation of elapsed in the subsequent frame.
		}

		m_frames = 0;
		m_chunkPushCounter = 0;

		return result;
	};

    EEngineStatus CTimer::cleanup()
	{
		EEngineStatus result = EEngineStatus::Ok;

		m_dataStore.clear();

		m_timeInterface = NULL;

		return result;
	};

    EEngineStatus CTimer::update() {
		EEngineStatus result = EEngineStatus::Ok;

		++m_frames;

#ifdef _DEBUG
        if (mConversionConstant && m_timeInterface)
#elif
		if (m_pTimeInterface)
#endif
		{
			m_elapsed = m_current; // Store in elapsed variable to save memory!
			if (CheckEngineError(result = m_timeInterface->getTimestamp(m_current)))
				printf("ERROR: Timer::update(): Failed at querying the current timestamp.\n%s: %s", __FILE__, __LINE__);
			else {
				m_elapsed = m_current - m_elapsed;

                m_chunkPushCounter += elapsed(ETimeUnit::Seconds);
				if (m_chunkPushCounter >= 1.0) {
                    m_dataStore.push_chunk(total_elapsed(ETimeUnit::Seconds), m_frames);

					m_frames = 0;
					m_chunkPushCounter -= 1.0;
				}
			}
		}

		return result;
	};

    double CTimer::elapsed(ETimeUnit unit) {
		double factor = 1.0;

		if (m_timeInterface)
			factor = m_timeInterface->getConversionMask(unit);

        return (((double)m_elapsed / (double)mConversionConstant) * factor);
	};

    double CTimer::total_elapsed(ETimeUnit unit) {
		double factor = 1.0;

		if (m_timeInterface)
			factor = m_timeInterface->getConversionMask(unit);

        return (((m_current - m_initial) / (1.0*mConversionConstant))*factor);
	};

    float CTimer::FPS() {
		return (float)m_dataStore.average(m_dataStore.size() - 10, 10);
	};
}
