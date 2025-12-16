/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/16 20:47:08 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

static void printVector(const std::vector<std::string>& vec, const std::string& emptyMsg = "(none)")
{
    if (vec.empty())
    {
        std::cout << emptyMsg;
        return;
    }
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (i > 0)
            std::cout << ", ";
        std::cout << vec[i];
    }
}

static void printContext(const ConfigContext& ctx, const std::string& indent)
{
    std::cout << indent << "Root: " << ctx.root << std::endl;
    
    std::cout << indent << "Index: ";
    printVector(ctx.index);
    std::cout << std::endl;
    
    std::cout << indent << "Client Max Body Size: ";
    if (ctx.clientMaxBodySize == "0")
        std::cout << "unlimited";
    else
        std::cout << ctx.clientMaxBodySize << " bytes";
    std::cout << std::endl;
    
    std::cout << indent << "Autoindex: " << (ctx.autoIndex == 1 ? "on" : "off") << std::endl;
    
    if (!ctx.errorPages.empty())
    {
        std::cout << indent << "Error Pages:" << std::endl;
        for (std::map<int, std::string>::const_iterator it = ctx.errorPages.begin();
             it != ctx.errorPages.end(); ++it)
        {
            std::cout << indent << "  " << it->first << " -> " << it->second << std::endl;
        }
    }
    else
    {
        std::cout << indent << "Error Pages: (none)" << std::endl;
    }
}

static void printLocationConfig(const LocationConfig& loc, const std::string& indent)
{
    std::cout << indent << "[Location: " << loc.path << "]" << std::endl;
    
    printContext(loc.ctx, indent + "  ");
    
    std::cout << indent << "  Allowed Methods: ";
    printVector(loc.allowedMethods, "GET");
    std::cout << std::endl;
    
    if (loc.redirectCode != 0)
    {
        std::cout << indent << "  Return: " << loc.redirectCode << " " 
                  << loc.redirectUrl << std::endl;
    }
    else
    {
        std::cout << indent << "  Return: (none)" << std::endl;
    }
    
    if (loc.uploadEnabled)
    {
        std::cout << indent << "  Upload: enabled (path: " << loc.uploadPath << ")" << std::endl;
    }
    else
    {
        std::cout << indent << "  Upload: disabled" << std::endl;
    }
    
    if (loc.cgiEnabled)
    {
        std::cout << indent << "  CGI: enabled (extensions: ";
        printVector(loc.cgiExtensions);
        std::cout << ")" << std::endl;
    }
    else
    {
        std::cout << indent << "  CGI: disabled" << std::endl;
    }
}

static void printServerConfig(const ServerConfig& server, size_t serverNum)
{
    std::cout << "--- Server #" << serverNum << " ---" << std::endl;
    
    std::cout << "  Listen: ";
    for (size_t i = 0; i < server.listen.size(); ++i)
    {
        if (i > 0)
            std::cout << ", ";
        std::cout << server.listen[i].host << ":" << server.listen[i].port;
    }
    std::cout << std::endl;
    
    std::cout << "  Server Names: ";
    printVector(server.serverNames, "(none)");
    std::cout << std::endl;
    
    printContext(server.ctx, "  ");
    
    if (!server.locations.empty())
    {
        std::cout << "  Locations (" << server.locations.size() << "):" << std::endl;
        for (size_t i = 0; i < server.locations.size(); ++i)
        {
            printLocationConfig(server.locations[i], "    ");
        }
    }
    else
    {
        std::cout << "  Locations: (none)" << std::endl;
    }
}

static void printHttpConfig(const HttpConfig& config)
{
    std::cout << "\n=== HTTP Configuration ===" << std::endl;
    
    std::cout << "\nHTTP-Level Directives:" << std::endl;
    printContext(config.ctx, "  ");
    
    std::cout << "\nServers (" << config.servers.size() << "):" << std::endl;
    for (size_t i = 0; i < config.servers.size(); ++i)
    {
        std::cout << std::endl;
        printServerConfig(config.servers[i], i + 1);
    }
}

static void createDefaultConfig(HttpConfig& config)
{
    std::cout << "No config file provided. Using default configuration..." << std::endl;
    
    config.ctx.applyDefaults();
    
    ServerConfig defaultServer;
    
    defaultServer.ctx.inheritFrom(config.ctx);
    defaultServer.ctx.applyDefaults();
    defaultServer.serverNames.push_back("localhost");
    
    LocationConfig defaultLocation;
    defaultLocation.path = "/";
    defaultLocation.ctx.inheritFrom(defaultServer.ctx);
    defaultLocation.ctx.applyDefaults();
    
    defaultServer.locations.push_back(defaultLocation);
    config.servers.push_back(defaultServer);
    
    std::cout << "Default configuration created successfully!" << std::endl;
}


int main(int argc, char** argv)
{
    try
    {
        if (argc > 2)
        {
            std::cerr << "Error: Too many arguments!" << std::endl;
            std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
            return 1;
        }
        
        HttpConfig config;
        
        if (argc == 1)
        {
            createDefaultConfig(config);
        }
        else
        {
            std::string configFile = argv[1];
            
            std::cout << "Parsing config file: " << configFile << std::endl;
            
            ConfigParser parser;
            parser.parse(configFile);
            config = parser.getConfig();
            
            std::cout << "✓ Config file parsed successfully!" << std::endl;
        }
        
        printHttpConfig(config);
        
        std::cout << "\n=== Configuration loaded successfully! ===" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n✗ Error: " << e.what() << std::endl;
        return 1;
    }
}
