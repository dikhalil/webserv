/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 20:10:47 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpConfig.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

static void applyInheritance(HttpConfig& child, const HttpConfig& parent)
{
    if (child.index.empty() && !parent.index.empty())
        child.index = parent.index;
    if (child.root.empty())
        child.root = parent.root;
    if (child.clientMaxBodySize.empty() && !parent.clientMaxBodySize.empty())
        child.clientMaxBodySize = parent.clientMaxBodySize;
    if (child.autoIndex == -1 && parent.autoIndex != -1)
        child.autoIndex = parent.autoIndex;
    if (child.errorPages.empty() && !parent.errorPages.empty())
        child.errorPages = parent.errorPages;
    if (child.cgiBinPath.empty() && !parent.cgiBinPath.empty())
        child.cgiBinPath = parent.cgiBinPath;
}

HttpConfig::HttpConfig() 
    : root("")
    , clientMaxBodySize("")
    , autoIndex(-1)
    , cgiBinPath("")
{}

const std::vector<ServerConfig>& HttpConfig::getServers() const 
{ 
    return servers; 
}

void HttpConfig::addServer(const ServerConfig& server) 
{ 
    servers.push_back(server); 
}

void HttpConfig::applyDefaults()
{
    if (root.empty())
        root = "./www";
    if (clientMaxBodySize.empty())
        clientMaxBodySize = "1048576";
    if (autoIndex == -1)
        autoIndex = 0;
    if (index.empty())
        index.push_back("index.html");
    
    for (size_t i = 0; i < servers.size(); ++i)
    {
        applyInheritance(servers[i], *this);
        
        if (servers[i].listen.empty())
            servers[i].listen.push_back(ListenConfig("0.0.0.0", 80));
        if (servers[i].serverNames.empty())
            servers[i].serverNames.push_back("localhost");
        
        const std::vector<LocationConfig>& locations = servers[i].getLocations();
        for (size_t j = 0; j < locations.size(); ++j)
        {
            LocationConfig& location = const_cast<LocationConfig&>(locations[j]);
            
            applyInheritance(location, servers[i]);
            
            if (location.methods.empty())
                location.methods.push_back("GET");
        }
    }
}
