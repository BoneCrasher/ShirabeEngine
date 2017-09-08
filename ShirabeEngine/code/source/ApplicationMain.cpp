#include "Platform/ApplicationEnvironment.h"

#include "Core/Engine.h"
#include "Log/Log.h"

namespace Main {
	DeclareLogTag(ApplicationMain)
}

int WINAPI WinMain(
		HINSTANCE hInstance, 
		HINSTANCE hPrevInstance,
		PSTR      szCmdLine, 
		int       iCmdShow)
{
	using namespace Engine;

	InitializeConsole();

	Log::setLineWidth(80);
	Log::Status(Main::logTag(), "SAE5300 - Application", Log::Style::CENTRE);

	Platform::ApplicationEnvironment appEnvironment;
	appEnvironment._instanceHandle         = hInstance;
	appEnvironment._previousInstanceHandle = hPrevInstance;
	appEnvironment._osDisplays             = Platform::OSDisplay::GetDisplays();

	// TODO: Configuation
	// EngineConfiguration engineConfiguration = EngineConfiguration::loadConfiguration(/* target? */);

	EngineInstancePtr pEngine = MakeSharedPointerType<EngineInstance>(appEnvironment/*, engineConfiguration*/);
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

	DeinitializeConsole();

	return 0;
}