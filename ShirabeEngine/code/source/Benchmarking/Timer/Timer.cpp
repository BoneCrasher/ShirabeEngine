#include "Benchmarking/Timer/Timer.h"

namespace Engine {

	Timer::Timer() {
		_conversionConstant = _initial = _current = _elapsed = 0;

		if (!CheckEngineError(initialize()))
			return;
	};

	Timer::~Timer() {
		this->cleanup();
	};

	void Timer::setTimeInterface(const TimePtr& pInterface)
	{
		if (!pInterface)
			throw new std::invalid_argument("Cannot set time-interface to NULL.");

		_timeInterface = pInterface;
	};

	EEngineStatus Timer::initialize()
	{
		EEngineStatus result = EEngineStatus::Ok;

		setTimeInterface(MakeSharedPointerType<internal_time_type>());

		if (!_timeInterface) {
			printf("FATAL_ERROR: Timer::initialize: Assigning by YTime::getInterface() failed. Interface-pointer is NULL.");
			result = EEngineStatus::Timer_NoPlatformTimeInstance;
		} else {
			if (CheckEngineError(result = _timeInterface->initialize())) {
				// ...
			}

			// Get conversion constant
			if (CheckEngineError(result = _timeInterface->getConversionConstant(_conversionConstant))) {
				printf("FATAL_ERROR: Timer::initialize: An error occured on requesting the proprietary time interface conversion constant from the time interface.\nAt: %s:%s", __FILE__, __LINE__); // might cause exceptions
			}
			// get initial timestamp
			if (CheckEngineError(result = _timeInterface->getTimestamp(_initial))) {
				printf("ERROR: Timer::initialize: An error occured on requesting the current timestamp from the time interface.\nAt: %s:%s", __FILE__, __LINE__); // might cause exceptions
			} else
				_current = _initial; // init current for calculation of elapsed in the subsequent frame.
		}

		_frames = 0;
		_chunkPushCounter = 0;

		return result;
	};

	EEngineStatus Timer::cleanup()
	{
		EEngineStatus result = EEngineStatus::Ok;

		_dataStore.clear();

		_timeInterface = NULL;

		return result;
	};

	EEngineStatus Timer::update() {
		EEngineStatus result = EEngineStatus::Ok;

		++_frames;

#ifdef _DEBUG
		if (_conversionConstant && _timeInterface)
#elif
		if (m_pTimeInterface)
#endif
		{
			_elapsed = _current; // Store in elapsed variable to save memory!
			if (CheckEngineError(result = _timeInterface->getTimestamp(_current)))
				printf("ERROR: Timer::update(): Failed at querying the current timestamp.\n%s: %s", __FILE__, __LINE__);
			else {
				_elapsed = _current - _elapsed;

				_chunkPushCounter += this->elapsed(ETimeUnit::Seconds);
				if (_chunkPushCounter >= 1.0) {
					_dataStore.push_chunk(this->total_elapsed(ETimeUnit::Seconds), _frames);

					_frames = 0;
					_chunkPushCounter -= 1.0;
				}
			}
		}

		return result;
	};

	double Timer::elapsed(ETimeUnit unit) {
		double factor = 1.0;

		if (_timeInterface)
			factor = _timeInterface->getConversionMask(unit);

		return (((double)_elapsed / (double)_conversionConstant) * factor);
	};

	double Timer::total_elapsed(ETimeUnit unit) {
		double factor = 1.0;

		if (_timeInterface)
			factor = _timeInterface->getConversionMask(unit);

		return (((_current - _initial) / (1.0*_conversionConstant))*factor);
	};

	float Timer::FPS() {
		return (float)_dataStore.average(_dataStore.size() - 10, 10);
	};
}