/**
 * -----------------------------------------------------------------------------
 * File: LogLevel.java
 * Project: RELogger
 * Author: Jayansh Devgan
 * Date: October 10, 2025
 * -----------------------------------------------------------------------------
 * Description:
 * Defines various logging levels used throughout the RELogger system.
 * Each level represents a severity, controlling which messages are output
 * based on the current configured log level.
 * -----------------------------------------------------------------------------
 */

package RELogger;

/**
 * Enumeration representing the severity of log messages.
 * The logging system uses this to filter messages based on importance.
 */
public enum LogLevel {
    TRACE(0),  // Fine-grained information for debugging and tracing flow
    DEBUG(1),  // Diagnostic information useful during development
    INFO(2),   // General information about normal application operation
    WARN(3),   // Warnings about potentially harmful situations
    ERROR(4),  // Error events that might still allow the application to continue
    FATAL(5);  // Severe errors that cause premature termination

    /** Numeric severity level, used for comparison between levels. */
    private final int ordinal;

    /**
     * Constructs a LogLevel with the given numeric severity value.
     *
     * @param ordinal Numeric value representing severity.
     */
    LogLevel(int ordinal) {
        this.ordinal = ordinal;
    }

    /**
     * Returns the numeric severity value of the logging level.
     *
     * @return The numeric ordinal value.
     */
    public int getOrdinal() {
        return ordinal;
    }
}
