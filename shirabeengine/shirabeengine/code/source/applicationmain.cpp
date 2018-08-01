#include "Platform/ApplicationEnvironment.h"

#include "Core/Engine.h"
#include "Log/Log.h"

namespace Main {
	SHIRABE_DECLARE_LOG_TAG(ApplicationMain)
}

int WINAPI WinMain(
		HINSTANCE hInstance, 
		HINSTANCE hPrevInstance,
		PSTR      szCmdLine, 
		int       iCmdShow)
{
	using namespace Engine;

  #ifdef _DEBUG
	InitializeConsole();
  #endif

	Log::setLineWidth(80);
	Log::Status(Main::logTag(), "SAE5300 - Application", Log::Style::CENTRE);

	CStdSharedPtr_t<Platform::ApplicationEnvironment> appEnvironment = makeCStdSharedPtr<Platform::ApplicationEnvironment>();
	appEnvironment->instanceHandle         = hInstance;
	appEnvironment->previousInstanceHandle = hPrevInstance;
	appEnvironment->osDisplays             = Platform::OSDisplay::GetDisplays();

	// TODO: Configuation
	// EngineConfiguration engineConfiguration = EngineConfiguration::loadConfiguration(/* target? */);

	EngineInstancePtr pEngine = makeCStdSharedPtr<EngineInstance>(appEnvironment/*, engineConfiguration*/);
	if (CheckEngineError(pEngine->initialize())) {
		Log::Error(Main::logTag(), "Failed to initialize engine instance.");
	}

	while (!CheckEngineError(pEngine->update())) {
		// Just run the shit out of the engine...
		// If it returns we had an error or something had it close.
		// Drop out automatically in this case and perform cleanup.
	}

	if (CheckEngineError(pEngine->deinitialize())) {
		Log::Error(Main::logTag(), "Failed to deinitialize engine instance.");
	}

	pEngine.reset();
	pEngine = nullptr;

  #ifdef _DEBUG
	DeinitializeConsole();
  #endif

	return 0;
}