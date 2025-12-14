#include "ConfigValidator.hpp"
#include <stdexcept>
#include <sstream>

// ============================================================================
// Server Validation
// ============================================================================

void ConfigValidator::validateServerConfig(ServerConfig& server)
{
    // Check for duplicate listen directives (same IP:port)
    checkDuplicateListen(server);
    
    // Check for duplicate location paths
    checkDuplicateLocations(server);
    
    // Validate listen (must have at least one)
    if (server.listen.empty())
        server.listen.push_back(ListenConfig("0.0.0.0", 80));
    
    // Validate root (required)
    if (server.root.empty())
        throw std::runtime_error("Server block must have 'root' directive");
}

void ConfigValidator::checkDuplicateListen(const ServerConfig& server)
{
    // Check for duplicate listen (same IP:Port in same server)
    for (size_t i = 0; i < server.listen.size(); ++i)
    {
        for (size_t j = i + 1; j < server.listen.size(); ++j)
        {
            if (server.listen[i].address == server.listen[j].address &&
                server.listen[i].port == server.listen[j].port)
            {
                std::stringstream ss;
                ss << "Duplicate listen directive: " 
                   << server.listen[i].address << ":" << server.listen[i].port;
                throw std::runtime_error(ss.str());
            }
        }
    }
}

void ConfigValidator::checkDuplicateLocations(const ServerConfig& server)
{
    // Check for duplicate location paths (same path in same server)
    for (size_t i = 0; i < server.locations.size(); ++i)
    {
        for (size_t j = i + 1; j < server.locations.size(); ++j)
        {
            if (server.locations[i].path == server.locations[j].path)
            {
                throw std::runtime_error("Duplicate location path: " + server.locations[i].path);
            }
        }
    }
}

// ============================================================================
// Cross-Server Validation
// ============================================================================

void ConfigValidator::checkDuplicateServers(const HttpConfig& httpConfig)
{
    // Check for duplicate server configurations (same IP:port:server_name)
    const std::vector<ServerConfig>& servers = httpConfig.servers;
    
    for (size_t i = 0; i < servers.size(); ++i)
    {
        for (size_t j = i + 1; j < servers.size(); ++j)
        {
            // Compare each listen directive from server i with server j
            for (size_t li = 0; li < servers[i].listen.size(); ++li)
            {
                for (size_t lj = 0; lj < servers[j].listen.size(); ++lj)
                {
                    // Check if they have the same IP:port
                    if (servers[i].listen[li].address == servers[j].listen[lj].address &&
                        servers[i].listen[li].port == servers[j].listen[lj].port)
                    {
                        // Same IP:port, now check server_names
                        // If both have the same server_name (or both empty), it's a duplicate
                        for (size_t sni = 0; sni < servers[i].serverNames.size(); ++sni)
                        {
                            for (size_t snj = 0; snj < servers[j].serverNames.size(); ++snj)
                            {
                                if (servers[i].serverNames[sni] == servers[j].serverNames[snj])
                                {
                                    std::stringstream ss;
                                    ss << "Duplicate server: " 
                                       << servers[i].listen[li].address << ":" 
                                       << servers[i].listen[li].port
                                       << " with server_name '" << servers[i].serverNames[sni] << "'";
                                    throw std::runtime_error(ss.str());
                                }
                            }
                        }
                        
                        // Special case: if both servers have empty server_names on same IP:port
                        if (servers[i].serverNames.empty() && servers[j].serverNames.empty())
                        {
                            std::stringstream ss;
                            ss << "Duplicate server: " 
                               << servers[i].listen[li].address << ":" 
                               << servers[i].listen[li].port
                               << " with no server_name";
                            throw std::runtime_error(ss.str());
                        }
                    }
                }
            }
        }
    }
}

// ============================================================================
// Location Validation
// ============================================================================

void ConfigValidator::validateLocationConfig(const LocationConfig& location, const ServerConfig& server)
{
    (void)server; // May be used for future validation
    
    // Validate path (required)
    if (location.path.empty())
        throw std::runtime_error("Location block must have a path");
    
    // Validate root (required, can be inherited)
    if (location.root.empty())
        throw std::runtime_error("Location '" + location.path + "' must have 'root' directive (either in location or server)");
    
    // Validate upload_path if upload is enabled
    if (location.upload && location.uploadPath.empty())
        throw std::runtime_error("Location '" + location.path + "' has upload enabled but upload_path is missing");
    
    // Validate cgi_extension if cgi is enabled
    if (location.cgi && location.cgiExtension.empty())
        throw std::runtime_error("Location '" + location.path + "' has cgi enabled but cgi_ext is missing");
    
    // Validate return directive
    // Note: redirectUrl can be empty (return code only, direct response)
    // But if URL is present, code must be present too
    if (!location.redirectUrl.empty() && location.redirectCode == 0)
        throw std::runtime_error("Location '" + location.path + "' has redirect_url but redirect_code is missing or zero");
}
