/*
===============================================================================

  RELogger - Royal Entertainment Logging System (Implementation)
  -------------------------------------------------------------

  Core implementation file for RELogger.
  Provides thread-safe, multi-platform logging with support for
  severity-based filtering, color-coded console output, and
  timestamped file logging.

  Supported Platforms:
    - Windows (Win32/Win64)
    - Linux / macOS (POSIX pthreads)
    - MinGW / Clang / GCC environments

  Author:  Jayansh Devgan
  Date:    09 October 2025
  License: Open Source (Royal Entertainment Project Core Utility)

===============================================================================
*/

#include "relogger.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>  /* For errno_t with fopen_s */

/*
===============================================================================
  PLATFORM-SPECIFIC MUTEX HANDLING
  --------------------------------
  Implements lightweight locking for multi-threaded environments.

  - On Windows: Uses CRITICAL_SECTION.
  - On POSIX (Linux/macOS): Uses pthread_mutex_t.
  - On NO_THREADS: Mutex calls are stripped out.
===============================================================================
*/

#ifdef _WIN32
    #include <windows.h>
    static CRITICAL_SECTION logMutex;
    #define INIT_MUTEX()    InitializeCriticalSection(&logMutex)
    #define LOCK_MUTEX()    EnterCriticalSection(&logMutex)
    #define UNLOCK_MUTEX()  LeaveCriticalSection(&logMutex)
    #define DESTROY_MUTEX() DeleteCriticalSection(&logMutex)
#else
    #include <pthread.h>
    static pthread_mutex_t logMutex = PTHREAD_MUTEX_INITIALIZER;
    #define INIT_MUTEX()    /* Statically initialized */
    #define LOCK_MUTEX()    pthread_mutex_lock(&logMutex)
    #define UNLOCK_MUTEX()  pthread_mutex_unlock(&logMutex)
    #define DESTROY_MUTEX() pthread_mutex_destroy(&logMutex)
#endif

/* Disable threading if explicitly defined */
#ifdef NO_THREADS
    #undef INIT_MUTEX
    #undef LOCK_MUTEX
    #undef UNLOCK_MUTEX
    #undef DESTROY_MUTEX
    #define INIT_MUTEX()
    #define LOCK_MUTEX()
    #define UNLOCK_MUTEX()
    #define DESTROY_MUTEX()
#endif

/*
===============================================================================
  INTERNAL CONSTANTS & GLOBAL STATE
===============================================================================
*/

#define RESET_COLOR "\033[0m"  /**< ANSI reset code for console colors */

static FILE* logFile = NULL;           /**< Output file handle for log writing */
static LogLevel currentLevel = LogLevel_Trace;  /**< Global minimum log level */
static int mutexInitialized = 0;      /**< Indicates if the mutex was initialized */

/*
===============================================================================
  FUNCTION: LevelToString
  -----------------------
  Converts a LogLevel enum value to a string label.

  @param level - The LogLevel to convert.
  @return Human-readable string name for the level.
===============================================================================
*/
const char* LevelToString(LogLevel level)
{
    switch (level)
    {
        case LogLevel_Trace: return "TRACE";
        case LogLevel_Debug: return "DEBUG";
        case LogLevel_Info:  return "INFO";
        case LogLevel_Warn:  return "WARN";
        case LogLevel_Error: return "ERROR";
        case LogLevel_Fatal: return "FATAL";
    }
    return "UNKNOWN";
}

/*
===============================================================================
  FUNCTION: LevelToColor
  ----------------------
  Maps a LogLevel to an ANSI terminal color sequence for colorized output.

  @param level - The LogLevel to convert.
  @return The corresponding ANSI color escape code.
===============================================================================
*/
const char* LevelToColor(LogLevel level)
{
    switch (level)
    {
        case LogLevel_Trace: return "\033[37m"; /* White */
        case LogLevel_Debug: return "\033[36m"; /* Cyan */
        case LogLevel_Info:  return "\033[32m"; /* Green */
        case LogLevel_Warn:  return "\033[33m"; /* Yellow */
        case LogLevel_Error: return "\033[31m"; /* Red */
        case LogLevel_Fatal: return "\033[41m"; /* Red background */
    }
    return "\033[0m"; /* Reset */
}

/*
===============================================================================
  FUNCTION: RELogger_Init
  -----------------------
  Initializes the logging system, creates or opens the specified log file,
  and sets up thread-safety mechanisms if supported.

  @param logFilePath - The file path for the log file. If NULL or empty,
                       logging will occur only on console.
===============================================================================
*/
void RELogger_Init(const char* logFilePath)
{
    INIT_MUTEX();
    mutexInitialized = 1;

    if (logFilePath && strlen(logFilePath) > 0)
    {
    #ifdef _MSC_VER
        /* Use secure fopen_s on MSVC */
        errno_t err = fopen_s(&logFile, logFilePath, "w");
        if (err != 0 || logFile == NULL)
        {
            fprintf(stderr, "Warning: Could not open log file '%s' (error: %d)\n",
                logFilePath, (int)err);
        }
    #else
        /* Use standard fopen for POSIX and others */
        logFile = fopen(logFilePath, "w");
        if (logFile == NULL)
        {
            fprintf(stderr, "Warning: Could not open log file '%s' (error: %s)\n",
                logFilePath, strerror(errno));
        }
    #endif
    }
}

/*
===============================================================================
  FUNCTION: RELogger_Shutdown
  ---------------------------
  Cleans up the logging system by closing any open log file and
  destroying the synchronization primitives.
===============================================================================
*/
void RELogger_Shutdown(void)
{
    if (logFile != NULL)
    {
        fflush(logFile);
        fclose(logFile);
        logFile = NULL;
    }

    if (mutexInitialized)
    {
        DESTROY_MUTEX();
        mutexInitialized = 0;
    }
}

/*
===============================================================================
  FUNCTION: RELogger_SetLevel
  ---------------------------
  Sets the current minimum log level. Messages below this level
  will not be printed or written.

  @param level - The LogLevel threshold to set.
===============================================================================
*/
void RELogger_SetLevel(LogLevel level)
{
    currentLevel = level;
}

/*
===============================================================================
  FUNCTION: RELogger_GetLevel
  ---------------------------
  Retrieves the currently active minimum logging level.

  @return The currently set LogLevel.
===============================================================================
*/
LogLevel RELogger_GetLevel(void)
{
    return currentLevel;
}

/*
===============================================================================
  FUNCTION: RELogger_Log
  ----------------------
  The core logging function. Handles formatting, timestamping,
  thread-safety, console output, and optional file writing.

  @param level   - The log severity level.
  @param message - The log message text.
  @param file    - Source file where the log originated.
  @param line    - Line number of the log call.
  @param func    - Function name where the log was triggered.
===============================================================================
*/
void RELogger_Log(LogLevel level, const char* message, const char* file, int line, const char* func)
{
#ifdef DEBUG
    /* Skip logs below current verbosity */
    if (level < currentLevel)
        return;

    LOCK_MUTEX();

    /* --- Timestamp Generation --- */
    time_t now_t = time(NULL);
    struct tm tm;

    #ifdef _MSC_VER
        localtime_s(&tm, &now_t);
    #elif defined(__MINGW32__) || defined(__MINGW64__)
        tm = *localtime(&now_t); /* Safe due to outer mutex */
    #else
        localtime_r(&now_t, &tm);
    #endif

    char timeStr[10];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &tm);

    /* --- Console Output --- */
    FILE* out = (level >= LogLevel_Error) ? stderr : stdout;
    fprintf(out, "%s[%s] %s %s:%d (%s) - %s%s\n",
        LevelToColor(level), timeStr, LevelToString(level),
        file, line, func, message, RESET_COLOR);

    /* --- File Logging --- */
    if (logFile != NULL)
    {
        fprintf(logFile, "[%s] %s %s:%d (%s) - %s\n",
            timeStr, LevelToString(level), file, line, func, message);
        fflush(logFile);
    }

    UNLOCK_MUTEX();
#endif /* DEBUG */
}

/*
===============================================================================
  END OF FILE
===============================================================================
*/
