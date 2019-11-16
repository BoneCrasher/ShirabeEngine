#include <sstream>

#include "log/log.h"

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    #ifdef SHIRABE_PLATFORM_WINDOWS
    void CConsole::InitializeConsole() {

        DWORD err = 0;

        int                         consoleHandle;
        HANDLE                      stdHandle;
        CONSOLE_SCREEN_BUFFER_INFO  consoleInformation;
        FILE                       *fp;

        // Allocate a console for this app
        if(!AllocConsole()) {
            // Console already allocated!
            return;
        }

        // if (!AttachConsole(GetCurrentProcessId())) {
        // 	// Failed to attach!
        // 	err = GetLastError();
        // 	return;
        // }

        // Set the screen buffer to be big enough to let us scroll text
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInformation);
        consoleInformation.dwSize.Y = MAX_CONSOLE_LINES;
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), consoleInformation.dwSize);
        SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));


        // Redirect unbuffered STDOUT to the console
        stdHandle     = GetStdHandle(STD_OUTPUT_HANDLE);
        consoleHandle = _open_osfhandle(intptr_t(stdHandle), _O_TEXT);
        // Open file to output console buffer
        fp = _fdopen(consoleHandle, "w");
        *stdout = *fp;
        setvbuf(stdout, NULL, _IONBF, 0);

        // Redirect unbuffered STDIN to the console
        stdHandle = GetStdHandle(STD_INPUT_HANDLE);
        consoleHandle = _open_osfhandle(intptr_t(stdHandle), _O_TEXT);
        // Open file to input console buffer
        fp = _fdopen(consoleHandle, "r");
        *stdin = *fp;
        setvbuf(stdin, NULL, _IONBF, 0);

        // Redirect unbuffered STDERR to the console
        stdHandle = GetStdHandle(STD_ERROR_HANDLE);
        consoleHandle = _open_osfhandle(intptr_t(stdHandle), _O_TEXT);
        // Open file to error output console buffer
        fp = _fdopen(consoleHandle, "w");
        *stderr = *fp;
        setvbuf(stderr, NULL, _IONBF, 0);

        // Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
        // point to console as well
        std::ios::sync_with_stdio(true);
    }

    void CConsole::DeinitializeConsole() {

        if(!FreeConsole()) {
            // There was no console...
        }
    }

    #else
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    #endif
}


