/*
===============================================================================

  RELogger - Royal Entertainment Logging System (C++ Header)
  ---------------------------------------------------------

  A lightweight, modern C++20 logging interface designed for game engines
  and tools within the Royal Entertainment framework. Supports console
  and file output with color-coded log levels and compile-time location
  tracing.

  Features:
    - Modern C++ interface (std::string_view, std::format)
    - Level-based filtering (Trace → Fatal)
    - Optional file logging
    - Thread-safe (implemented in .cpp)
    - Platform-independent macros

  Author:  Jayansh Devgan
  Date:    09 October 2025
  License: Open Source (Royal Entertainment Project Core Utility)

===============================================================================
*/

#pragma once
#ifndef RELOGGER_H
#define RELOGGER_H

#include <string>
#include <string_view>
#include <format>

/*
===============================================================================
  ENUM CLASS: LogLevel
  --------------------
  Defines various levels of logging severity.
===============================================================================
*/
enum class LogLevel
{
    Trace,  /**< Extremely detailed information used for tracing execution flow. */
    Debug,  /**< Developer-level diagnostic information. */
    Info,   /**< General information for normal runtime events. */
    Warn,   /**< Warnings about potential issues. */
    Error,  /**< Errors that may prevent correct operation. */
    Fatal,  /**< Critical errors causing shutdown or crash. */
};

/*
===============================================================================
  NAMESPACE: RELogger
  -------------------
  Core logging namespace containing all logger controls.
  Provides simple global access while maintaining modularity.
===============================================================================
*/
namespace RELogger
{
    /*
    =======================================================================
      FUNCTION: Init
      ---------------------------------------------------------------------
      Initializes the logging system. If a file path is provided, logs
      will also be written to that file in addition to the console.

      @param logFilePath - Optional path for log file output.
    =======================================================================
    */
    void Init(const std::string& logFilePath = "");

    /*
    =======================================================================
      FUNCTION: Shutdown
      ---------------------------------------------------------------------
      Properly closes and cleans up the logging system.
      Should be called before program termination.
    =======================================================================
    */
    void Shutdown();

    /*
    =======================================================================
      FUNCTION: Log
      ---------------------------------------------------------------------
      Core logging function that handles message formatting and
      level-based output.

      @param level   - The severity level of the log.
      @param message - The log message text.
      @param file    - The source file where the message originated.
      @param line    - The line number of the log call.
      @param func    - The function name where the log occurred.
    =======================================================================
    */
    void Log(LogLevel level, std::string_view message,
             const char* file, int line, const char* func);

    /*
    =======================================================================
      FUNCTION: SetLevel
      ---------------------------------------------------------------------
      Sets the minimum logging level to display or write.
      Messages below this level will be ignored.

      @param level - The minimum LogLevel to record.
    =======================================================================
    */
    void SetLevel(LogLevel level);

    /*
    =======================================================================
      FUNCTION: GetLevel
      ---------------------------------------------------------------------
      Retrieves the current minimum logging level.

      @return The currently set LogLevel.
    =======================================================================
    */
    LogLevel GetLevel();
}

/*
===============================================================================
  MACRO DEFINITIONS
  -----------------
  Convenience macros for easy in-code logging with automatic
  file name, line number, and function capture.
===============================================================================
*/

#define RELOG_TRACE(msg) RELogger::Log(LogLevel::Trace, msg, __FILE__, __LINE__, __func__)
#define RELOG_DEBUG(msg) RELogger::Log(LogLevel::Debug, msg, __FILE__, __LINE__, __func__)
#define RELOG_INFO(msg)  RELogger::Log(LogLevel::Info,  msg, __FILE__, __LINE__, __func__)
#define RELOG_WARN(msg)  RELogger::Log(LogLevel::Warn,  msg, __FILE__, __LINE__, __func__)
#define RELOG_ERROR(msg) RELogger::Log(LogLevel::Error, msg, __FILE__, __LINE__, __func__)
#define RELOG_FATAL(msg) RELogger::Log(LogLevel::Fatal, msg, __FILE__, __LINE__, __func__)

/*
===============================================================================
  END OF FILE
===============================================================================
*/

#endif /* RELOGGER_H */
