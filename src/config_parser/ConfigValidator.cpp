/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 23:47:58 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/17 00:38:36 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigValidator.hpp"

void ConfigValidator::checkValue(const Tokenizer& tokenizer, const std::string& directive)
{
    if (!tokenizer.hasMore() || ConfigValidator::isBlock(tokenizer.peek()))
        throw std::runtime_error(directive + " directive requires a value (missing value before ';')");
    if (isReserved(tokenizer.peek()))
        throw std::runtime_error("Cannot use directive '" + tokenizer.peek() + "' as value for " + directive);
}

bool ConfigValidator::isDuplicate(const std::vector<std::string>& list, const std::string& value)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i] == value)
            return true;
    }
    return false;
}

bool ConfigValidator::isDuplicateListen(const std::vector<ListenConfig>& list, const ListenConfig& conf)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i].host == conf.host && list[i].port == conf.port)
            return true;
    }
    return false;
}

bool ConfigValidator::isValidMethod(const std::string& method)
{
    return (method == "GET" || method == "POST" || method == "DELETE");
}

bool ConfigValidator::isValidStatus(int code)
{
    return (code == 200 || code == 201 || code == 204 ||
            code == 301 || code == 302 || code == 303 || code == 304 || 
            code == 307 || code == 308 ||
            code == 400 || code == 401 || code == 403 || code == 404 || 
            code == 405 || code == 406 || code == 408 || code == 409 || 
            code == 410 || code == 411 || code == 413 || code == 414 || 
            code == 415 || code == 429 ||
            code == 500 || code == 501 || code == 502 || code == 503 || 
            code == 504 || code == 505);
}

bool ConfigValidator::isReserved(const std::string& word)
{
    static const std::string reserved[] = {
        "http", "server", "location", "listen", "server_name", "root",
        "index", "error_page", "client_max_body_size", "autoindex", 
        "methods", "return", "upload", "upload_path", "cgi", "cgi_ext", "cgi_bin_path"
    };
    static const size_t count = sizeof(reserved) / sizeof(reserved[0]);
    
    for (size_t i = 0; i < count; i++)
    {
        if (word == reserved[i])
            return true;
    }
    return false;
}

bool ConfigValidator::isBlock(const std::string& token)
{
    return (token == ";" || token == "{" || token == "}");
}

bool ConfigValidator::isDigits(const std::string& str)
{
    if (str.empty())
        return false;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!std::isdigit(str[i]))
            return false;
    }
    return true;
}

bool ConfigValidator::isValidIP(const std::string& ip)
{
    size_t dots = 0;
    size_t start = 0;

    if (ip.empty())
        return false;
    for (size_t i = 0; i <= ip.length(); i++)
    {
        if (i == ip.length() || ip[i] == '.')
        {
            if (i == start)
                return false;
            std::string octet = ip.substr(start, i - start);
            
            if (octet.length() > 3 || !isDigits(octet))
                return false;
            int val = std::atoi(octet.c_str());
            if (val < 0 || val > 255)
                return false;
            if (i < ip.length())
                dots++;
            start = i + 1;
        }
    }
    return dots == 3;
}

bool ConfigValidator::isValidPort(const std::string& port)
{
    if (!isDigits(port))
        return false;
    
    int p = std::atoi(port.c_str());
    return (p > 0 && p <= 65535);
}

void ConfigValidator::validateLocation(const LocationConfig& loc)
{
    if (loc.path.empty())
        throw std::runtime_error("Location must have a path");
    if (loc.uploadEnabled && loc.uploadPath.empty())
        throw std::runtime_error("Location with upload enabled must have upload_path");
    if (loc.cgiEnabled && loc.cgiExtensions.empty())
        throw std::runtime_error("Location with cgi enabled must have cgi_ext");
}

void ConfigValidator::validateServer(const ServerConfig& srv)
{
    if (srv.listen.empty())
        throw std::runtime_error("Server must have at least one listen directive");
    
    for (size_t j = 0; j < srv.locations.size(); j++)
        validateLocation(srv.locations[j]);
}

void ConfigValidator::checkDupLocations(const HttpConfig& config)
{
    for (size_t i = 0; i < config.servers.size(); i++)
    {
        const ServerConfig& srv = config.servers[i];
        std::map<std::string, bool> paths;
        
        for (size_t k = 0; k < srv.locations.size(); k++)
        {
            std::string path = srv.locations[k].path;
            if (paths.find(path) != paths.end())
                throw std::runtime_error("Duplicate location: " + path);
            paths[path] = true;
        }
    }
}

bool ConfigValidator::hasSameName(const ServerConfig& s1, const ServerConfig& s2)
{
    for (size_t i = 0; i < s1.serverNames.size(); i++)
    {
        for (size_t j = 0; j < s2.serverNames.size(); j++)
        {
            if (s1.serverNames[i] == s2.serverNames[j])
                return true;
        }
    }
    return false;
}

bool ConfigValidator::hasSameListen(const ServerConfig& s1, const ServerConfig& s2)
{
    for (size_t i = 0; i < s1.listen.size(); i++)
    {
        for (size_t j = 0; j < s2.listen.size(); j++)
        {
            if (s1.listen[i].host == s2.listen[j].host &&
                s1.listen[i].port == s2.listen[j].port)
                return true;
        }
    }
    return false;
}

void ConfigValidator::checkDupServers(const HttpConfig& config)
{
    for (size_t i = 0; i < config.servers.size(); i++)
    {
        for (size_t j = i + 1; j < config.servers.size(); j++)
        {
            const ServerConfig& s1 = config.servers[i];
            const ServerConfig& s2 = config.servers[j];
            
            if (hasSameListen(s1, s2) && hasSameName(s1, s2))
            {
                throw std::runtime_error("Duplicate server: same listen address and same server_name");
            }
        }
    }
}

void ConfigValidator::checkDuplicates(const HttpConfig& config)
{
    checkDupLocations(config);
    checkDupServers(config);
}

void ConfigValidator::validate(const HttpConfig& config)
{
    if (config.servers.empty())
        throw std::runtime_error("Configuration must have at least one server");
    for (size_t i = 0; i < config.servers.size(); i++)
        validateServer(config.servers[i]);
    checkDuplicates(config);
}
