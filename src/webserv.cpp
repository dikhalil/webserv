/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:01:37 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 15:11:48 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>
#include <ConfigParser.hpp>
#include "../config/ServerConfig.hpp"
#include "../config/LocationConfig.hpp"

int main (int argc, char **argv)
{
    ConfigParser configParser;

    if (argc < 2)
    {
        std::cout << "No config file provided, using default configuration..." << std::endl;
        
        ServerConfig defaultServer;
        defaultServer.setDefaults();
        
        LocationConfig defaultLocation;
        defaultLocation.path = "/";
        defaultLocation.setDefaults();
        defaultServer.locations.push_back(defaultLocation);
        
        std::cout << "✓ Default configuration loaded successfully!" << std::endl;
        std::cout << "  Server: " << defaultServer.serverNames[0] << std::endl;
        std::cout << "  Listen: " << defaultServer.listen[0].address << ":" << defaultServer.listen[0].port << std::endl;
        std::cout << "  Root: " << defaultServer.root << std::endl;
        std::cout << "  Index: " << defaultServer.index[0] << std::endl;
        return 0;
    }
    
    std::string configFile;
    configFile = argv[1];
    try
    {
        configParser.parse(configFile);
        std::cout << "✓ Configuration file parsed successfully!" << std::endl;
        std::cout << "Found " << configParser.getServers().size() << " server(s)" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}