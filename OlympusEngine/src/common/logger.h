#pragma once

#include <string>

class OLYMPUS_API Logger {
    // Getters and Setters for singleton static class

    private:
        static Logger* inst;
    public:
        static Logger* Instance() { return inst; }

        // Constructor
    public:
        ~Logger();
        Logger();

        /* Print to Log File */
        static void printLog( const WCHAR* fmt, ... );
        static std::wstring logDirectory();
        static std::wstring logFile();

        /* Print a line of '-' char's */
        static VOID printDebugSeparator();

        /* Check to see if MTail is already Running */
        static BOOL isMTailRunning();

        /* Start MTail Application */
        static VOID startMTail();
};