/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:33:28 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 21:15:09 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigValidator.hpp"
#include <stdexcept>
#include <sstream>

static bool listenMatches(const ListenConfig& a, const ListenConfig& b)
{
    return (a.address == b.address && a.port == b.port);
}

static bool hasCommonServerName(const std::vector<std::string>& a, const std::vector<std::string>& b)
{
    if (a.empty() || b.empty())
        return true;
    for (size_t i = 0; i < a.size(); ++i)
        for (size_t j = 0; j < b.size(); ++j)
            if (a[i] == b[j])
                return true;
    return false;
}

static void throwDuplicateServerError(const ListenConfig& listen, 
                                      const std::vector<std::string>& serverNames)
{
    std::stringstream ss;
    ss << "Duplicate server: " << listen.address << ":" << listen.port;
    if (serverNames.empty())
        ss << " with no server_name";
    else
        ss << " with server_name '" << serverNames[0] << "'";
    throw std::runtime_error(ss.str());
}

static void checkServerPair(const ServerConfig& s1, const ServerConfig& s2)
{
    for (size_t i = 0; i < s1.listen.size(); ++i)
    {
        for (size_t j = 0; j < s2.listen.size(); ++j)
        {
            if (!listenMatches(s1.listen[i], s2.listen[j]))
                continue;
            if (hasCommonServerName(s1.serverNames, s2.serverNames))
                throwDuplicateServerError(s1.listen[i], s1.serverNames);
        }
    }
}

void ConfigValidator::validateServerConfig(const ServerConfig& server)
{
    checkDuplicateListen(server);
    checkDuplicateLocations(server);
}

void ConfigValidator::checkDuplicateListen(const ServerConfig& server)
{
    for (size_t i = 0; i < server.listen.size(); ++i)
    {
        for (size_t j = i + 1; j < server.listen.size(); ++j)
        {
            if (listenMatches(server.listen[i], server.listen[j]))
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
    for (size_t i = 0; i < server.getLocations().size(); ++i)
    {
        for (size_t j = i + 1; j < server.getLocations().size(); ++j)
        {
            if (server.getLocations()[i].path == server.getLocations()[j].path)
                throw std::runtime_error("Duplicate location path: " + server.getLocations()[i].path);
        }
    }
}

void ConfigValidator::checkDuplicateServers(const HttpConfig& httpConfig)
{
    const std::vector<ServerConfig>& servers = httpConfig.getServers();
    
    for (size_t i = 0; i < servers.size(); ++i)
        for (size_t j = i + 1; j < servers.size(); ++j)
            checkServerPair(servers[i], servers[j]);
}

void ConfigValidator::validateLocationConfig(const LocationConfig& location)
{
    if (location.path.empty())
        throw std::runtime_error("Location block must have a path");
    
    if (location.path[0] != '/')
        throw std::runtime_error("Location path must start with '/': " + location.path);
    
    if (location.upload && location.uploadPath.empty())
        throw std::runtime_error("Location '" + location.path + "' has upload enabled but upload_path is missing");
    
    if (location.cgi && location.cgiExtensions.empty())
        throw std::runtime_error("Location '" + location.path + "' has cgi enabled but cgi_ext is missing");
    
    if (!location.redirectUrl.empty() && location.redirectCode == 0)
        throw std::runtime_error("Location '" + location.path + "' has redirect_url but redirect_code is missing or zero");
}
