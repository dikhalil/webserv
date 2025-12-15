/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:33:28 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 18:55:21 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ConfigParser.hpp>
#include <cstdlib>
#include <stdexcept>

ConfigParser::ConfigParser() {}

bool ConfigParser::isDirective(const std::string& token)
{
    return (token == "http" || token == "server" || token == "location" ||
            token == "listen" || token == "server_name" || token == "root" ||
            token == "index" || token == "client_max_body_size" || token == "error_page" ||
            token == "autoindex" || token == "cgi_bin_path" || token == "methods" ||
            token == "upload" || token == "upload_path" || token == "cgi" ||
            token == "cgi_ext" || token == "return" || token == "{" || token == "}");
}

bool ConfigParser::isValidHttpStatusCode(int code)
{
    switch (code)
    {
        case 200: case 201: case 204: 
        case 301: case 302: case 303: case 304: case 307: case 308:
        case 400: case 401: case 403: case 404: case 405:
        case 406: case 408: case 409: case 410: case 411: case 413:
        case 414: case 415: case 429:
        case 500: case 501: case 502: case 503: case 504: case 505: 
            return true;
        default:
            return false;
    }
}

void ConfigParser::parse(const std::string& filename)
{
    tokenizer.tokenizeFile(filename);
    std::vector<std::string>& tokens = tokenizer.getTokens();
    size_t i = 0;
    
    if (tokens.empty())
        throw std::runtime_error("Config file is empty");
    if (tokens[i] == "http")
    {
        parseHttpBlock(tokens, i);
    }
    else if (tokens[i] == "server")
    {
        while (i < tokens.size() && tokens[i] == "server")
        {
            ServerConfig server;
            parseServerBlock(tokens, i, server);
            httpConfig.addServer(server);
        }
        if (i < tokens.size())
            throw std::runtime_error("Unexpected token after server block: " + tokens[i]);
    }
    else
    {
        throw std::runtime_error("Config must start with 'http' or 'server' block");
    }
    if (httpConfig.getServers().empty())
        throw std::runtime_error("Config must contain at least one server block");
    
    validateAndApplyDefaults();
}

const HttpConfig& ConfigParser::getHttpConfig() const
{
    return httpConfig;
}

const std::vector<ServerConfig>& ConfigParser::getServers() const
{
    return httpConfig.getServers();
}

void ConfigParser::validateAndApplyDefaults()
{
    httpConfig.applyDefaults();
    
    const std::vector<ServerConfig>& servers = httpConfig.getServers();
    for (size_t i = 0; i < servers.size(); ++i)
    {
        const ServerConfig& server = servers[i];
        
        ConfigValidator::validateServerConfig(server);
        
        const std::vector<LocationConfig>& locations = server.getLocations();
        for (size_t j = 0; j < locations.size(); ++j)
        {
            const LocationConfig& location = locations[j];
            
            ConfigValidator::validateLocationConfig(location);
        }
    }
    
    ConfigValidator::checkDuplicateServers(httpConfig);
}
