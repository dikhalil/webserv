/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 15:34:14 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{
    listen.clear();
    serverNames.clear();
    root = "";
    index.clear();
    clientMaxBodySize = 0;
    errorPages.clear();
    cgiBinPath = "";
    locations.clear();
}

ServerConfig::~ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig& other)
{
    *this = other;
}

ServerConfig& ServerConfig::operator=(const ServerConfig& other)
{
    if (this != &other)
    {
        listen = other.listen;
        serverNames = other.serverNames;
        root = other.root;
        index = other.index;
        clientMaxBodySize = other.clientMaxBodySize;
        errorPages = other.errorPages;
        cgiBinPath = other.cgiBinPath;
        locations = other.locations;
    }
    return *this;
}

void ServerConfig::setDefaults()
{
    if (listen.empty())
        listen.push_back(ListenConfig("0.0.0.0", 80));
    
    // Note: root is REQUIRED, no default value
    // It must be set explicitly or inherited from HTTP
    
    if (index.empty())
        index.push_back("index.html");
    
    if (clientMaxBodySize == 0)
        clientMaxBodySize = 1048576;
    
    if (serverNames.empty())
        serverNames.push_back("localhost");
}
