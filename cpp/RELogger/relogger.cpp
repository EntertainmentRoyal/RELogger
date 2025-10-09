/**
 * @file relogger.cpp
 * @author Jayansh Devgan
 * @date 09 October 2025
 * @brief Implementation of the RELogger system – a lightweight, thread-safe
 *        logging utility for engine and application diagnostics.
 *
 * The RELogger provides:
 *  - Console and file logging support.
 *  - Color-coded severity levels for terminal readability.
 *  - Timestamps and contextual metadata (file, line, function).
 *  - Thread-safe operations using std::mutex.
 *
 * This module is designed to be minimal, fast, and easily integrable into
 * game engines or standalone applications. Inspired by robust logging systems
 * found in open-source engines like DOOM and idTech.
 */

#include "relogger.h"

#include <iostream>
#include <fstream>
#include <mutex>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace
{
	// -------------------------------------------------------------------------
	// Internal State
	// -------------------------------------------------------------------------

	std::ofstream logFile;                   ///< File stream for persistent log output.
	std::mutex logMutex;                     ///< Mutex to ensure thread-safe logging.
	LogLevel currentLevel = LogLevel::Trace; ///< Minimum level to log (default: Trace).

	// -------------------------------------------------------------------------
	// Helper Functions
	// -------------------------------------------------------------------------

	/**
	 * @brief Converts a LogLevel enum to its string representation.
	 * @param level The log level to convert.
	 * @return A constant C-string corresponding to the log level.
	 */
	const char* LevelToString(LogLevel level)
	{
		switch (level)
		{
			case LogLevel::Trace: return "TRACE";
			case LogLevel::Debug: return "DEBUG";
			case LogLevel::Info:  return "INFO";
			case LogLevel::Warn:  return "WARN";
			case LogLevel::Error: return "ERROR";
			case LogLevel::Fatal: return "FATAL";
			default: return "UNKNOWN";
		}
	}

	/**
	 * @brief Maps each LogLevel to a specific ANSI color code for terminal output.
	 * @param level The log level.
	 * @return The corresponding ANSI escape sequence for color.
	 */
	const char* LevelToColor(LogLevel level)
	{
		switch (level)
		{
			case LogLevel::Trace: return "\033[37m"; // White
			case LogLevel::Debug: return "\033[36m"; // Cyan
			case LogLevel::Info:  return "\033[32m"; // Green
			case LogLevel::Warn:  return "\033[33m"; // Yellow
			case LogLevel::Error: return "\033[31m"; // Red
			case LogLevel::Fatal: return "\033[41m"; // Red background
			default: return "\033[0m";               // Reset
		}
	}
}

// ============================================================================
//                          RELogger IMPLEMENTATION
// ============================================================================

/**
 * @brief Initializes the logger and optionally opens a file for log output.
 *
 * If a file path is provided, it will open or overwrite the file for new logs.
 * If not provided, only console output will be active.
 *
 * @param logFilePath Path to the log file. Leave empty to disable file logging.
 */
void RELogger::Init(const std::string& logFilePath)
{
	std::lock_guard<std::mutex> lock(logMutex);
	if (!logFilePath.empty())
	{
		logFile.open(logFilePath, std::ios::out | std::ios::trunc);
		if (!logFile)
		{
			std::cerr << "\033[31m[LOGGER ERROR] Failed to open log file: "
					  << logFilePath << "\033[0m" << std::endl;
		}
	}
}

/**
 * @brief Gracefully shuts down the logger, flushing and closing the log file.
 */
void RELogger::Shutdown()
{
	std::lock_guard<std::mutex> lock(logMutex);
	if (logFile.is_open())
	{
		logFile.flush();
		logFile.close();
	}
}

/**
 * @brief Sets the minimum severity level for log messages.
 *
 * Any message below the specified level will be ignored.
 *
 * @param level The desired minimum log level (e.g., LogLevel::Warn).
 */
void RELogger::SetLevel(LogLevel level)
{
	std::lock_guard<std::mutex> lock(logMutex);
	currentLevel = level;
}

/**
 * @brief Retrieves the currently active log level.
 * @return The current LogLevel setting.
 */
LogLevel RELogger::GetLevel()
{
	std::lock_guard<std::mutex> lock(logMutex);
	return currentLevel;
}

/**
 * @brief Logs a message to both console and file with full contextual details.
 *
 * Produces colorized output in the console and a clean plain-text log in the file.
 * Each message includes:
 *  - Timestamp (HH:MM:SS)
 *  - Log level
 *  - File name, line number, and function name
 *  - Log message
 *
 * Thread-safe. Automatically skips logs below the active log level.
 *
 * @param level Severity of the message.
 * @param message The message to log.
 * @param file The source file where this log was invoked.
 * @param line The line number within the file.
 * @param func The name of the function that triggered the log.
 */
void RELogger::Log(LogLevel level, std::string_view message, const char* file, int line, const char* func)
{
#ifndef NDEBUG // Logging disabled in release builds unless explicitly enabled
	if (level < currentLevel)
		return;

	std::lock_guard<std::mutex> lock(logMutex);

	// -------------------------------------------------------------------------
	// Timestamp Generation
	// -------------------------------------------------------------------------
	auto now = std::chrono::system_clock::now();
	std::time_t t_c = std::chrono::system_clock::to_time_t(now);
	std::tm tm {};

#ifdef _WIN32
	localtime_s(&tm, &t_c);
#else
	localtime_r(&t_c, &tm);
#endif

	std::ostringstream oss;
	oss << std::put_time(&tm, "%H:%M:%S");

	// -------------------------------------------------------------------------
	// Console Output
	// -------------------------------------------------------------------------
	std::ostream& out = (level >= LogLevel::Error) ? std::cerr : std::cout;
	out << LevelToColor(level)
		<< "[" << oss.str() << "] "
		<< LevelToString(level) << " "
		<< file << ":" << line << " (" << func << ") - "
		<< message
		<< "\033[0m" << std::endl;

	// -------------------------------------------------------------------------
	// File Output (if enabled)
	// -------------------------------------------------------------------------
	if (logFile.is_open())
	{
		logFile << "[" << oss.str() << "] "
				<< LevelToString(level) << " "
				<< file << ":" << line << " (" << func << ") - "
				<< message << std::endl;
	}
#endif
}
