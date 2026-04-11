/**
 * @file IModeConfig.h
 * @brief Mode Configuration Interface for MS4000
 *
 * Abstract interface for mode-specific configuration access.
 * Decouples modes from direct protobuf structure access, allowing
 * configuration to come from different sources (protobuf, JSON, defaults).
 *
 * Part of Phase 2B refactoring - Dependency Injection layer.
 */

#ifndef IMODE_CONFIG_H
#define IMODE_CONFIG_H

#include <stdint.h>

/**
 * @brief Abstract interface for mode configuration
 *
 * Provides type-safe access to mode-specific configuration values.
 * Implementations can wrap protobuf structures, JSON configs, or defaults.
 */
class IModeConfig
{
public:
    virtual ~IModeConfig() = default;

    /**
     * @brief Get configuration value as integer
     *
     * @param key Configuration key name
     * @param defaultValue Default value if key not found
     * @return Configuration value
     */
    virtual int getInt(const char *key, int defaultValue = 0) = 0;

    /**
     * @brief Get configuration value as float
     *
     * @param key Configuration key name
     * @param defaultValue Default value if key not found
     * @return Configuration value
     */
    virtual float getFloat(const char *key, float defaultValue = 0.0f) = 0;

    /**
     * @brief Get configuration value as boolean
     *
     * @param key Configuration key name
     * @param defaultValue Default value if key not found
     * @return Configuration value
     */
    virtual bool getBool(const char *key, bool defaultValue = false) = 0;

    /**
     * @brief Set configuration value as integer
     *
     * @param key Configuration key name
     * @param value Value to set
     * @return true if successful
     */
    virtual bool setInt(const char *key, int value) = 0;

    /**
     * @brief Set configuration value as float
     *
     * @param key Configuration key name
     * @param value Value to set
     * @return true if successful
     */
    virtual bool setFloat(const char *key, float value) = 0;

    /**
     * @brief Set configuration value as boolean
     *
     * @param key Configuration key name
     * @param value Value to set
     * @return true if successful
     */
    virtual bool setBool(const char *key, bool value) = 0;

    /**
     * @brief Save configuration to persistent storage
     * @return true if successful
     */
    virtual bool save() = 0;

    /**
     * @brief Load configuration from persistent storage
     * @return true if successful
     */
    virtual bool load() = 0;
};

#endif // IMODE_CONFIG_H
