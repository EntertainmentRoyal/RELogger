/*
===============================================================================

  RELogger - Royal Entertainment Logging System
  --------------------------------------------

  A lightweight logging interface for C-based projects. Provides simple,
  level-based logging to file and console. Inspired by simplicity and speed,
  built for clarity in debugging, diagnostics, and runtime monitoring.

  Author: Jayansh Devgan
  Date:   09 October 2025

===============================================================================
*/

#ifndef RELOGGER_H
#define RELOGGER_H

/*
===============================================================================
  ENUM: LogLevel
  --------------
  Defines various levels of log severity.
===============================================================================
*/

typedef enum
{
    LogLevel_Trace,   /**< Detailed tracing information, for deep debugging. */
    LogLevel_Debug,   /**< Developer-level debug messages. */
    LogLevel_Info,    /**< General informational messages. */
    LogLevel_Warn,    /**< Warning conditions; not fatal but noteworthy. */
    LogLevel_Error,   /**< Error conditions that require attention. */
    LogLevel_Fatal    /**< Critical conditions causing termination. */
} LogLevel;

/*
===============================================================================
  FUNCTION: RELogger_Init
  -----------------------
  Initializes the logging system and opens the specified log file for writing.
  Must be called before any logging occurs.

  @param logFilePath - Path to the file where logs will be written.
===============================================================================
*/
void RELogger_Init(const char* logFilePath);

/*
===============================================================================
  FUNCTION: RELogger_Shutdown
  ---------------------------
  Shuts down the logging system and closes the log file. Must be called
  before program termination to ensure all data is flushed properly.
===============================================================================
*/
void RELogger_Shutdown(void);

/*
===============================================================================
  FUNCTION: RELogger_Log
  ----------------------
  Core logging function. Writes a message with contextual information
  such as source file, line number, and function name.

  @param level - The severity level of the message.
  @param message - The log message to write.
  @param file - The source file name (__FILE__).
  @param line - The line number (__LINE__).
  @param func - The function name (__func__ or __FUNCTION__).
===============================================================================
*/
void RELogger_Log(LogLevel level, const char* message, const char* file, int line, const char* func);

/*
===============================================================================
  FUNCTION: RELogger_SetLevel
  ---------------------------
  Sets the minimum level of messages to be logged.
  Messages below this level are ignored.

  @param level - The minimum log level to record.
===============================================================================
*/
void RELogger_SetLevel(LogLevel level);

/*
===============================================================================
  FUNCTION: RELogger_GetLevel
  ---------------------------
  Retrieves the current minimum logging level.

  @return The currently set LogLevel.
===============================================================================
*/
LogLevel RELogger_GetLevel(void);

/*
===============================================================================
  MACROS: RELOG_<LEVEL>
  ---------------------
  Convenience macros for streamlined logging. Automatically capture
  the source file, line number, and function name.

  Example:
      RELOG_INFO("Game initialized successfully");
===============================================================================
*/

#ifdef _MSC_VER  /* Microsoft Visual C++ Compiler Specific Macros */

/* MSVC defines function name as __FUNCTION__ */
#define RELOG_TRACE(msg)  RELogger_Log(LogLevel_Trace, msg, __FILE__, __LINE__, __FUNCTION__)
#define RELOG_DEBUG(msg)  RELogger_Log(LogLevel_Debug, msg, __FILE__, __LINE__, __FUNCTION__)
#define RELOG_INFO(msg)   RELogger_Log(LogLevel_Info,  msg, __FILE__, __LINE__, __FUNCTION__)
#define RELOG_WARN(msg)   RELogger_Log(LogLevel_Warn,  msg, __FILE__, __LINE__, __FUNCTION__)
#define RELOG_ERROR(msg)  RELogger_Log(LogLevel_Error, msg, __FILE__, __LINE__, __FUNCTION__)
#define RELOG_FATAL(msg)  RELogger_Log(LogLevel_Fatal, msg, __FILE__, __LINE__, __FUNCTION__)

#else  /* GCC, Clang, MinGW */

/* GCC/Clang define function name as __func__ */
#define RELOG_TRACE(msg)  RELogger_Log(LogLevel_Trace, msg, __FILE__, __LINE__, __func__)
#define RELOG_DEBUG(msg)  RELogger_Log(LogLevel_Debug, msg, __FILE__, __LINE__, __func__)
#define RELOG_INFO(msg)   RELogger_Log(LogLevel_Info,  msg, __FILE__, __LINE__, __func__)
#define RELOG_WARN(msg)   RELogger_Log(LogLevel_Warn,  msg, __FILE__, __LINE__, __func__)
#define RELOG_ERROR(msg)  RELogger_Log(LogLevel_Error, msg, __FILE__, __LINE__, __func__)
#define RELOG_FATAL(msg)  RELogger_Log(LogLevel_Fatal, msg, __FILE__, __LINE__, __func__)

#endif /* _MSC_VER */

/*
===============================================================================
  END OF FILE
===============================================================================
*/

#endif /* RELOGGER_H */
