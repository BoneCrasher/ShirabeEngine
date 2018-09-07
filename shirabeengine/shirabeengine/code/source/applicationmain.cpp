//#include <os/applicationenvironment.h>
//#include <core/engine.h>
//#include <log/log.h>

//namespace Main
//{
//	SHIRABE_DECLARE_LOG_TAG(ApplicationMain)
//}

//#if defined SHIRABE_PLATFORM_WINDOWS
//int WINAPI WinMain(
//		HINSTANCE hInstance,
//		HINSTANCE hPrevInstance,
//		PSTR      szCmdLine,
//		int       iCmdShow)
//#elif defined SHIRABE_PLATFORM_LINUX
//int main(int aArgc, char **aArgv)
//#endif
//{

//    using namespace engine;

//  #ifdef _DEBUG
//    CConsole::InitializeConsole();
//  #endif

//    CLog::Status(Main::logTag(), "SAE5300 - Application");

//    CStdSharedPtr_t<os::SApplicationEnvironment> appEnvironment = makeCStdSharedPtr<os::SApplicationEnvironment>();
//#if defined SHIRABE_PLATFORM_WINDOWS
//	appEnvironment->instanceHandle         = hInstance;
//	appEnvironment->previousInstanceHandle = hPrevInstance;
//#endif
//    // appEnvironment->osDisplays = os::SOSDisplay::GetDisplays();

//	// TODO: Configuation
//	// EngineConfiguration engineConfiguration = EngineConfiguration::loadConfiguration(/* target? */);

//    CStdSharedPtr_t<CEngineInstance> engine = makeCStdSharedPtr<CEngineInstance>(appEnvironment/*, engineConfiguration*/);
//    if (CheckEngineError(engine->initialize()))
//    {
//        CLog::Error(Main::logTag(), "Failed to initialize engine instance.");
//	}

//    while (!CheckEngineError(engine->update()))
//    {
//		// Just run the shit out of the engine...
//		// If it returns we had an error or something had it close.
//		// Drop out automatically in this case and perform cleanup.
//	}

//    if (CheckEngineError(engine->deinitialize()))
//    {
//        CLog::Error(Main::logTag(), "Failed to deinitialize engine instance.");
//	}

//    engine.reset();
//    engine = nullptr;

//  #ifdef _DEBUG
//    CConsole::DeinitializeConsole();
//  #endif

//	return 0;
//}
