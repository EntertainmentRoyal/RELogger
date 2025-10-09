/**
 * @file Logger.cs
 * @author Jayansh Devgan
 * @date 10 October 2025
 * @brief RE Logger - A lightweight, thread-safe logging utility for .NET applications and game engines.
 *
 * Provides:
 *  - Console and file-based logging with color-coded output.
 *  - Severity-based filtering using LogLevel.
 *  - Contextual metadata (file, line, method) using Caller Info attributes.
 *  - Thread safety through global locking.
 *
 * This logger is minimal, robust, and designed to integrate easily
 * into custom engines, tools, or standalone .NET projects.
 */

using System;
using System.IO;
using System.Runtime.CompilerServices;

namespace RE
{
    /// <summary>
    /// Represents the severity or verbosity level of a log message.
    /// </summary>
    public enum LogLevel
    {
        /// <summary>Detailed execution trace. Used for extremely fine-grained debugging.</summary>
        Trace = 0,

        /// <summary>Standard debugging information.</summary>
        Debug,

        /// <summary>General information about the program’s execution.</summary>
        Info,

        /// <summary>Indicates potential issues or unexpected states.</summary>
        Warn,

        /// <summary>Indicates a recoverable error or failure in a subsystem.</summary>
        Error,

        /// <summary>Indicates a critical failure requiring immediate attention.</summary>
        Fatal
    }

    /// <summary>
    /// Provides global logging functionality for RE applications.
    /// 
    /// Thread-safe and configurable, it supports both console and file logging.
    /// </summary>
    public static class Logger
    {
        private static StreamWriter? logFile;                 ///< File stream for persistent logging.
        private static readonly object logLock = new object();///< Lock object to ensure thread safety.
        private static LogLevel currentLevel = LogLevel.Trace;///< Current minimum log level threshold.

        // --------------------------------------------------------------------
        //                          Helper Methods
        // --------------------------------------------------------------------

        /// <summary>
        /// Converts a <see cref="LogLevel"/> to a string label.
        /// </summary>
        private static string LevelToString(LogLevel level)
        {
            return level switch
            {
                LogLevel.Trace => "TRACE",
                LogLevel.Debug => "DEBUG",
                LogLevel.Info  => "INFO",
                LogLevel.Warn  => "WARN",
                LogLevel.Error => "ERROR",
                LogLevel.Fatal => "FATAL",
                _ => "UNKNOWN"
            };
        }

        /// <summary>
        /// Maps a <see cref="LogLevel"/> to its corresponding console color.
        /// </summary>
        private static ConsoleColor LevelToColor(LogLevel level)
        {
            return level switch
            {
                LogLevel.Trace => ConsoleColor.Gray,
                LogLevel.Debug => ConsoleColor.Cyan,
                LogLevel.Info  => ConsoleColor.Green,
                LogLevel.Warn  => ConsoleColor.Yellow,
                LogLevel.Error => ConsoleColor.Red,
                LogLevel.Fatal => ConsoleColor.White, // White on red background for high visibility
                _ => ConsoleColor.White
            };
        }

        // --------------------------------------------------------------------
        //                          Core Methods
        // --------------------------------------------------------------------

        /// <summary>
        /// Initializes the logger and optionally opens a file for logging output.
        /// </summary>
        /// <param name="logFilePath">
        /// Optional path to the log file. If empty, file logging is disabled and
        /// logs will only appear in the console.
        /// </param>
        public static void Init(string logFilePath = "")
        {
            lock (logLock)
            {
                if (!string.IsNullOrEmpty(logFilePath))
                {
                    try
                    {
                        logFile = new StreamWriter(logFilePath);
                    }
                    catch (Exception)
                    {
                        Console.Error.WriteLine($"[LOGGER ERROR] Failed to open log file: {logFilePath}");
                    }
                }
            }
        }

        /// <summary>
        /// Gracefully shuts down the logger, flushing and closing the log file.
        /// </summary>
        public static void Shutdown()
        {
            lock (logLock)
            {
                logFile?.Flush();
                logFile?.Dispose();
                logFile = null;
            }
        }

        /// <summary>
        /// Sets the minimum severity level for log messages.
        /// Messages below this level will be ignored.
        /// </summary>
        /// <param name="level">The desired log level threshold.</param>
        public static void SetLevel(LogLevel level)
        {
            lock (logLock)
                currentLevel = level;
        }

        /// <summary>
        /// Retrieves the current logging level threshold.
        /// </summary>
        /// <returns>The current <see cref="LogLevel"/> value.</returns>
        public static LogLevel GetLevel()
        {
            lock (logLock)
                return currentLevel;
        }

        /// <summary>
        /// Logs a message with full contextual information including source file,
        /// line number, and method name. Automatically color-codes console output
        /// based on the severity level and writes to both console and file (if active).
        /// </summary>
        /// <param name="level">Severity of the log message.</param>
        /// <param name="message">The message text to log.</param>
        /// <param name="file">Automatically provided file path of the caller.</param>
        /// <param name="line">Automatically provided line number of the caller.</param>
        /// <param name="func">Automatically provided method name of the caller.</param>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Log(
            LogLevel level,
            string message,
            [CallerFilePath] string? file = null,
            [CallerLineNumber] int line = 0,
            [CallerMemberName] string? func = null)
        {
#if DEBUG
            if (level < currentLevel)
                return;

            lock (logLock)
            {
                string timestamp = DateTime.Now.ToString("HH:mm:ss");
                TextWriter output = level >= LogLevel.Error ? Console.Error : Console.Out;

                ConsoleColor originalForeground = Console.ForegroundColor;
                ConsoleColor originalBackground = Console.BackgroundColor;

                Console.ForegroundColor = LevelToColor(level);
                if (level == LogLevel.Fatal)
                    Console.BackgroundColor = ConsoleColor.Red;

                output.WriteLine($"[{timestamp}] {LevelToString(level)} {file}:{line} ({func}) - {message}");

                // Reset console colors
                Console.ForegroundColor = originalForeground;
                Console.BackgroundColor = originalBackground;

                // Write to file if enabled
                if (logFile != null)
                {
                    logFile.WriteLine($"[{timestamp}] {LevelToString(level)} {file}:{line} ({func}) - {message}");
                    logFile.Flush();
                }
            }
#endif
        }
    }
}
