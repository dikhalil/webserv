/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:01:37 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 20:36:46 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>
#include <ConfigParser.hpp>
#include "../config/ServerConfig.hpp"
#include "../config/LocationConfig.hpp"

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

void printServerConfig(const ServerConfig& server)
{
    std::cout << "  Server Names: ";
    printVector(server.serverNames);
    std::cout << std::endl;

    std::cout << "  Listen: ";
    for (size_t i = 0; i < server.listen.size(); ++i)
    {
        if (i > 0)
            std::cout << ", ";
        std::cout << server.listen[i].address << ":" << server.listen[i].port;
    }
    std::cout << std::endl;

    std::cout << "  Root: " << server.root << std::endl;
    std::cout << "  Index: ";
    printVector(server.index);
    std::cout << std::endl;

    if (server.clientMaxBodySize == "0")
        std::cout << "  Client Max Body Size: unlimited" << std::endl;
    else
        std::cout << "  Client Max Body Size: " << server.clientMaxBodySize << " bytes" << std::endl;
    std::cout << "  Autoindex: " << (server.autoIndex == 1 ? "on" : "off") << std::endl;
    std::cout << "  CGI Bin Path: " << (server.cgiBinPath.empty() ? "(none)" : server.cgiBinPath) << std::endl;

    if (!server.errorPages.empty())
    {
        std::cout << "  Error Pages:" << std::endl;
        for (std::map<int, std::string>::const_iterator it = server.errorPages.begin();
             it != server.errorPages.end(); ++it)
        {
            std::cout << "    " << it->first << " -> " << it->second << std::endl;
        }
    }
    else
        std::cout << "  Error Pages: (none)" << std::endl;

    if (!server.getLocations().empty())
    {
        std::cout << "  Locations (" << server.getLocations().size() << "):" << std::endl;
        for (size_t i = 0; i < server.getLocations().size(); ++i)
        {
            const LocationConfig& loc = server.getLocations()[i];
            std::cout << "    [" << loc.path << "]" << std::endl;
            std::cout << "      Root: " << loc.root << std::endl;
            std::cout << "      Index: ";
            printVector(loc.index);
            std::cout << std::endl;
            if (loc.clientMaxBodySize == "0")
                std::cout << "      Client Max Body Size: unlimited" << std::endl;
            else
                std::cout << "      Client Max Body Size: " << loc.clientMaxBodySize << " bytes" << std::endl;
            std::cout << "      Autoindex: " << (loc.autoIndex == 1 ? "on" : "off") << std::endl;
            std::cout << "      CGI Bin Path: " << (loc.cgiBinPath.empty() ? "(none)" : loc.cgiBinPath) << std::endl;
            std::cout << "      Methods: ";
            printVector(loc.methods);
            std::cout << std::endl;
            
            if (!loc.errorPages.empty())
            {
                std::cout << "      Error Pages:" << std::endl;
                for (std::map<int, std::string>::const_iterator it = loc.errorPages.begin();
                     it != loc.errorPages.end(); ++it)
                {
                    std::cout << "        " << it->first << " -> " << it->second << std::endl;
                }
            }
            else
                std::cout << "      Error Pages: (none)" << std::endl;
            
            if (loc.upload)
                std::cout << "      Upload: on (path: " << loc.uploadPath << ")" << std::endl;
            else
                std::cout << "      Upload: off" << std::endl;
            
            if (loc.cgi)
            {
                std::cout << "      CGI: on (ext: ";
                printVector(loc.cgiExtensions);
                std::cout << ")" << std::endl;
            }
            else
                std::cout << "      CGI: off" << std::endl;
            
            if (loc.redirectCode != 0)
                std::cout << "      Return: " << loc.redirectCode << " " << loc.redirectUrl << std::endl;
            else
                std::cout << "      Return: (none)" << std::endl;
        }
    }
}

void setDefualtSetting(ServerConfig &defaultServer)
{    
    if (defaultServer.listen.empty())
        defaultServer.listen.push_back(ListenConfig("0.0.0.0", 80));
    if (defaultServer.serverNames.empty())
        defaultServer.serverNames.push_back("localhost");
    if (defaultServer.index.empty())
        defaultServer.index.push_back("index.html");
    
    LocationConfig defaultLocation;
    defaultLocation.path = ("/");
    defaultLocation.root = defaultServer.root;
    defaultLocation.index = defaultServer.index;
    defaultLocation.clientMaxBodySize = defaultServer.clientMaxBodySize;
    defaultLocation.autoIndex = defaultServer.autoIndex;
    defaultLocation.errorPages = defaultServer.errorPages;
    defaultLocation.cgiBinPath = defaultServer.cgiBinPath;
    if (defaultLocation.methods.empty())
        defaultLocation.methods.push_back("GET");
    
    defaultServer.addLocation(defaultLocation);
    
    std::cout << " Default configuration loaded successfully!" << std::endl;
    printServerConfig(defaultServer);
}

int main (int argc, char **argv)
{
    ConfigParser configParser;
    std::string configFile;

    if (argc > 2)
    {
        std::cerr << "Error: Too many arguments provided!" << std::endl;
        std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
        return 1;
    }
    
    try
    {
        ServerConfig defaultServer;
        if (argc == 1)
            setDefualtSetting(defaultServer);
        else
        {
            configFile = argv[1];
            configParser.parse(configFile);
            std::cout << "Found " << configParser.getServers().size() << " server(s)" << std::endl;
            std::cout << std::endl;
            
            const std::vector<ServerConfig>& servers = configParser.getServers();
            for (size_t i = 0; i < servers.size(); ++i)
            {
                std::cout << "--- Server #" << (i + 1) << " ---"<< std::endl;
                printServerConfig(servers[i]);
                std::cout << std::endl;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}