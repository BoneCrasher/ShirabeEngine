#include <Platform/Platform.h>
#include <OS/ApplicationEnvironment.h>

#include <Log/Log.h>

#include <Core/Engine.h>
#include <Core/EngineTypeHelper.h>
#include <OS/OSDisplay.h>

namespace Main {
  DeclareLogTag(ApplicationMain)
}

#ifdef PLATFORM_WINDOWS

#include <WSI/Windows/WindowsDisplay.h>
#include <WSI/Windows/WindowsWindowFactory.h>
#include <Windows.h>

using OSDisplayType = Engine::OS::OSDisplay<Engine::WSI::WinAPIDisplay>;

using namespace Engine;

int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow) 
{
  #endif

  #ifdef SHIRABE_DEBUG
  Console::InitializeConsole();
  #endif

  Log::setLineWidth(80);
  Log::Status(Main::logTag(), "ShirabeEngineDemo - Application");

  Ptr<OS::ApplicationEnvironment> appEnvironment = MakeSharedPointerType<OS::ApplicationEnvironment>();
  appEnvironment->instanceHandle         = (OSHandle)hInstance;
  appEnvironment->previousInstanceHandle = (OSHandle)hPrevInstance;
  appEnvironment->osDisplays             = OSDisplayType::GetDisplays(appEnvironment->primaryDisplayIndex);

  Ptr<Engine::EngineInstance> pEngine = MakeSharedPointerType<Engine::EngineInstance>(appEnvironment/*, engineConfiguration*/);
  if(CheckEngineError(pEngine->initialize())) {
    Log::Error(Main::logTag(), "Failed to initialize engine instance.");
  }

  while(!CheckEngineError(pEngine->update())) {
    // Just run the shit out of the engine...
    // If it returns we had an error or something had it close.
    // Drop out automatically in this case and perform cleanup.
  }

  if(CheckEngineError(pEngine->deinitialize())) {
    Log::Error(Main::logTag(), "Failed to deinitialize engine instance.");
  }

  pEngine.reset();
  pEngine = nullptr;

  #ifdef SHIRABE_DEBUG
  Console::DeinitializeConsole();
  #endif

  return 0;
}