/**
 * @file ILogger.h
 * @brief Logging Interface for MS4000
 *
 * Abstract interface for logging operations. Decouples modes from
 * specific logging backends (Serial, Syslog, file, etc.)
 *
 * Part of Phase 2B refactoring - Dependency Injection layer.
 */

#ifndef ILOGGER_H
#define ILOGGER_H

/**
 * @brief Abstract interface for logging
 *
 * Provides methods for logging messages at different severity levels.
 * Implementations can route to Serial, Syslog, file, or multiple destinations.
 */
class ILogger
{
public:
    virtual ~ILogger() = default;

    /**
     * @brief Log a message without newline
     *
     * @param message Message to log
     */
    virtual void log(const char *message) = 0;

    /**
     * @brief Log a message with newline
     *
     * @param message Message to log
     */
    virtual void logln(const char *message) = 0;

    /**
     * @brief Log an error message
     *
     * @param message Error message to log
     */
    virtual void error(const char *message) = 0;

    /**
     * @brief Log a warning message
     *
     * @param message Warning message to log
     */
    virtual void warning(const char *message) = 0;

    /**
     * @brief Log a debug message
     *
     * @param message Debug message to log
     */
    virtual void debug(const char *message) = 0;
};

#endif // ILOGGER_H
