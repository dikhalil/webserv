/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStructures.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 20:44:23 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/16 20:44:25 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigStructures.hpp"

ConfigContext::ConfigContext() : autoIndex(-1) {}

void ConfigContext::inheritFrom(const ConfigContext& parent)
{
    if (root.empty())
        root = parent.root;
    if (index.empty())
        index = parent.index;
    if (clientMaxBodySize.empty())
        clientMaxBodySize = parent.clientMaxBodySize;
    if (autoIndex == -1)
        autoIndex = parent.autoIndex;
    if (cgiBinPath.empty())
        cgiBinPath = parent.cgiBinPath;
    
    for (std::map<int, std::string>::const_iterator it = parent.errorPages.begin();
         it != parent.errorPages.end(); ++it)
    {
        if (errorPages.find(it->first) == errorPages.end())
            errorPages[it->first] = it->second;
    }
}

void ConfigContext::applyDefaults()
{
    if (root.empty())
        root = "/var/www/html";
    if (index.empty())
        index.push_back("index.html");
    if (clientMaxBodySize.empty())
    {
        std::ostringstream oss;
        oss << (1024 * 1024);
        clientMaxBodySize = oss.str();
    }
    if (autoIndex == -1)
        autoIndex = 0;
}

ListenConfig::ListenConfig() : port(8080)
{
    host = "0.0.0.0";
}

LocationConfig::LocationConfig() : redirectCode(0), uploadEnabled(false), cgiEnabled(false) {}

ServerConfig::ServerConfig()
{
    ListenConfig defaultListen;
    listen.push_back(defaultListen);
}

HttpConfig::HttpConfig() {}
