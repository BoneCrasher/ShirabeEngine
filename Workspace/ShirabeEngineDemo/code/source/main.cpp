#include <vulkan/vulkan.h>

#include <Platform/Platform.h>
#include <Platform/ApplicationEnvironment.h>

#include <Log/Log.h>

#include <Core/Engine.h>

namespace Main {
  DeclareLogTag(ApplicationMain)
}

#ifdef PLATFORM_WINDOWS

#include <Platform/Windows/WindowsWindowFactory.h>

int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow) 
{
  #endif

  #ifdef _DEBUG
  InitializeConsole();
  #endif

  Log::setLineWidth(80);
  Log::Status(Main::logTag(), "ShirabeEngineDemo - Application", Log::Style::CENTRE);

  Ptr<Platform::ApplicationEnvironment> appEnvironment = MakeSharedPointerType<Platform::ApplicationEnvironment>();
  appEnvironment->instanceHandle         = hInstance;
  appEnvironment->previousInstanceHandle = hPrevInstance;
  appEnvironment->osDisplays             = Platform::OSDisplay::GetDisplays(appEnvironment->primaryDisplayIndex);

  EngineInstancePtr pEngine = MakeSharedPointerType<EngineInstance>(appEnvironment/*, engineConfiguration*/);
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

  #ifdef _DEBUG
  DeinitializeConsole();
  #endif

  return 0;
}