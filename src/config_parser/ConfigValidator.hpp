#ifndef CONFIGVALIDATOR_HPP
#define CONFIGVALIDATOR_HPP

#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "HttpConfig.hpp"
#include <vector>

class ConfigValidator
{
public:
    // Server validation
    static void validateServerConfig(ServerConfig& server);
    static void checkDuplicateListen(const ServerConfig& server);
    static void checkDuplicateLocations(const ServerConfig& server);
    
    // Cross-server validation
    static void checkDuplicateServers(const HttpConfig& httpConfig);
    
    // Location validation
    static void validateLocationConfig(const LocationConfig& location, const ServerConfig& server);
    
private:
    ConfigValidator(); // Prevent instantiation
    ~ConfigValidator();
    ConfigValidator(const ConfigValidator& other);
    ConfigValidator& operator=(const ConfigValidator& other);
};

#endif
