/// -----------------------------------------------------------------------------
/// File: RELogger.java
/// Project: RELogger
/// Author: Jayansh Devgan
/// Date: October 10, 2025
/// -----------------------------------------------------------------------------
/// Description:
/// A thread-safe, lightweight logging utility for Java applications.
/// Supports colored console output and optional file logging.
/// Features:
/// - Adjustable log level filtering (TRACE → FATAL)
/// - Thread-safe logging using synchronized blocks
/// - Colored console output for easy readability
/// - Optional log file writing with buffered I/O
/// - Includes timestamp, file name, line number, and method name
/// Usage Example:
///     RELogger.init("app.log");
///     RELogger.setLevel(LogLevel.INFO);
///     RELogger.log(LogLevel.INFO, "Application started successfully!");
///     RELogger.shutdown();
/// -----------------------------------------------------------------------------
package RELogger;

import java.io.*;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.Objects;

/**
 * Static logging utility for the RE framework.
 * Provides consistent, colored, and structured logging for applications.
 */
public class RELogger {

    /** Log file writer, initialized if a file path is provided. */
    private static PrintWriter logFile;

    /** Synchronization lock for thread-safe logging operations. */
    private static final Object logLock = new Object();

    /** Current global logging level threshold. */
    private static LogLevel currentLevel = LogLevel.TRACE;

    // ANSI escape codes for colored console output
    private static final String ANSI_RESET = "\u001B[0m";
    private static final String ANSI_WHITE = "\u001B[37m";
    private static final String ANSI_CYAN = "\u001B[36m";
    private static final String ANSI_GREEN = "\u001B[32m";
    private static final String ANSI_YELLOW = "\u001B[33m";
    private static final String ANSI_RED = "\u001B[31m";
    private static final String ANSI_WHITE_ON_RED = "\u001B[37;41m";
    private static final String ANSI_GRAY = "\u001B[90m";

    /**
     * Converts a LogLevel enum value to its string representation.
     *
     * @param level The log level to convert.
     * @return The corresponding string label.
     */
    private static String levelToString(LogLevel level) {
        return switch (level) {
            case TRACE -> "TRACE";
            case DEBUG -> "DEBUG";
            case INFO  -> "INFO";
            case WARN  -> "WARN";
            case ERROR -> "ERROR";
            case FATAL -> "FATAL";
        };
    }

    /**
     * Maps a LogLevel to a console color (ANSI escape code).
     *
     * @param level The log level to colorize.
     * @return The corresponding ANSI color code.
     */
    private static String levelToAnsiColor(LogLevel level) {
        return switch (level) {
            case TRACE -> ANSI_GRAY;
            case DEBUG -> ANSI_CYAN;
            case INFO  -> ANSI_GREEN;
            case WARN  -> ANSI_YELLOW;
            case ERROR -> ANSI_RED;
            case FATAL -> ANSI_WHITE_ON_RED;
        };
    }

    /**
     * Initializes the logger, optionally creating or overwriting a log file.
     *
     * @param logFilePath The path of the file to log into. Can be empty to disable file logging.
     */
    public static void init(String logFilePath) {
        synchronized (logLock) {
            if (logFilePath != null && !logFilePath.isEmpty()) {
                try {
                    logFile = new PrintWriter(new BufferedWriter(new FileWriter(logFilePath)));
                } catch (IOException e) {
                    System.err.println(ANSI_RED + "[LOGGER ERROR] Failed to open log file: "
                            + logFilePath + ANSI_RESET);
                }
            }
        }
    }

    /**
     * Closes and flushes the logger’s file output stream, if any.
     * Should be called before program termination to ensure no data loss.
     */
    public static void shutdown() {
        synchronized (logLock) {
            if (logFile != null) {
                logFile.flush();
                logFile.close();
                logFile = null;
            }
        }
    }

    /**
     * Sets the minimum log level to be recorded or displayed.
     * Messages below this level are ignored.
     *
     * @param level The minimum log level threshold.
     */
    public static void setLevel(LogLevel level) {
        synchronized (logLock) {
            currentLevel = Objects.requireNonNull(level, "LogLevel cannot be null");
        }
    }

    /**
     * Returns the currently configured log level threshold.
     *
     * @return The current log level.
     */
    public static LogLevel getLevel() {
        synchronized (logLock) {
            return currentLevel;
        }
    }

    /**
     * Logs a message to the console and optionally to a log file.
     * Includes contextual metadata such as file name, line number, and method name.
     *
     * @param level   The severity level of the log.
     * @param message The log message content.
     */
    public static void log(LogLevel level, String message, boolean DEBUG) {
        if (DEBUG && level.getOrdinal() >= currentLevel.getOrdinal()) {
            // Capture call site information for better debugging
            StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();
            StackTraceElement caller = stackTrace[2];
            String file = caller.getFileName() != null ? caller.getFileName() : "Unknown";
            int line = caller.getLineNumber();
            String func = caller.getMethodName();

            synchronized (logLock) {
                // Generate timestamp in HH:mm:ss format
                String timestamp = LocalTime.now().format(DateTimeFormatter.ofPattern("HH:mm:ss"));
                String color = levelToAnsiColor(level);

                // Select output stream: stdout for info, stderr for errors
                PrintStream out = (level.getOrdinal() >= LogLevel.ERROR.getOrdinal()) ? System.err : System.out;

                // Print to console with color
                out.println(color + "[" + timestamp + "] "
                        + levelToString(level) + " "
                        + file + ":" + line + " (" + func + ") - "
                        + message + ANSI_RESET);

                // Write to log file if enabled
                if (logFile != null) {
                    logFile.println("[" + timestamp + "] "
                            + levelToString(level) + " "
                            + file + ":" + line + " (" + func + ") - "
                            + message);
                    logFile.flush();
                }
            }
        }
    }
}
