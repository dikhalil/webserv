#ifndef CONFIGVALIDATOR_HPP
#define CONFIGVALIDATOR_HPP

#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "HttpConfig.hpp"

class ConfigValidator
{
    private:
        ConfigValidator();
    public:
        static void validateServerConfig(const ServerConfig& server);
        static void checkDuplicateListen(const ServerConfig& server);
        static void checkDuplicateLocations(const ServerConfig& server);        
        static void checkDuplicateServers(const HttpConfig& httpConfig);        
        static void validateLocationConfig(const LocationConfig& location);
};

#endif
