/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParserBlocks.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 18:45:07 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ConfigParser.hpp>
#include <stdexcept>

void ConfigParser::parseHttpBlock(std::vector<std::string>& tokens, size_t& i)
{
    bool hasServer = false;

    if (tokens[i] != "http")
        throw std::runtime_error("Expected 'http'");
    i++;
    if (i >= tokens.size() || tokens[i] != "{")
        throw std::runtime_error("Expected '{' after 'http'");
    i++;
    while (i < tokens.size())
    {
        if (tokens[i] == "}")
        {
            i++;
            if (!hasServer)
                throw std::runtime_error("HTTP block must contain at least one server block");
            return;
        }
        else if (tokens[i] == "server")
        {
            hasServer = true;
            ServerConfig server;
            parseServerBlock(tokens, i, server);
            httpConfig.addServer(server);
        }
        else if (tokens[i] == "root")
        {
            parseRoot(tokens, i, httpConfig.root);
        }
        else if (tokens[i] == "index")
        {
            parseIndex(tokens, i, httpConfig.index);
        }
        else if (tokens[i] == "client_max_body_size")
        {
            parseClientMaxBodySize(tokens, i, httpConfig.clientMaxBodySize);
        }
        else if (tokens[i] == "autoindex")
        {
            parseAutoIndex(tokens, i, httpConfig.autoIndex);
        }
        else if (tokens[i] == "error_page")
        {
            parseErrorPage(tokens, i, httpConfig.errorPages);
        }
        else if (tokens[i] == "cgi_bin_path")
        {
            parseCgiBinPath(tokens, i, httpConfig.cgiBinPath);
        }
        else
        {
            throw std::runtime_error("Unknown directive in http block: " + tokens[i]);
        }
    }
    
    throw std::runtime_error("Unexpected end of tokens in http block");
}

void ConfigParser::parseServerBlock(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    if (tokens[i] != "server")
        throw std::runtime_error("Expected 'server'");
    i++;    
    if (i >= tokens.size() || tokens[i] != "{")
        throw std::runtime_error("Expected '{' after 'server'");
    i++;
    while (i < tokens.size())
    {
        if (tokens[i] == "}")
        {
            i++;
            return;
        }
        else if (tokens[i] == "listen")
        {
            parseListen(tokens, i, server);
        }
        else if (tokens[i] == "server_name")
        {
            parseServerName(tokens, i, server);
        }
        else if (tokens[i] == "root")
        {
            parseRoot(tokens, i, server.root);
        }
        else if (tokens[i] == "index")
        {
            parseIndex(tokens, i, server.index);
        }
        else if (tokens[i] == "client_max_body_size")
        {
            parseClientMaxBodySize(tokens, i, server.clientMaxBodySize);
        }
        else if (tokens[i] == "error_page")
        {
            parseErrorPage(tokens, i, server.errorPages);
        }
        else if (tokens[i] == "autoindex")
        {
            parseAutoIndex(tokens, i, server.autoIndex);
        }
        else if (tokens[i] == "cgi_bin_path")
        {
            parseCgiBinPath(tokens, i, server.cgiBinPath);
        }
        else if (tokens[i] == "location")
        {
            LocationConfig location;
            i++;
            if (i >= tokens.size())
                throw std::runtime_error("Expected location path after 'location'");            
            if (tokens[i] == "{")
                throw std::runtime_error("Location must have a path (e.g., location / { ... })");
            location.path = tokens[i++];
            if (i >= tokens.size() || tokens[i] != "{")
                throw std::runtime_error("Expected '{' after location path");
            i++;
            parseLocationBlock(tokens, i, location);
            server.addLocation(location);
        }
        else
        {
            throw std::runtime_error("Unknown directive in server block: " + tokens[i]);
        }
    }
    throw std::runtime_error("Unexpected end of tokens in server block");
}

void ConfigParser::parseLocationBlock(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    while (i < tokens.size())
    {
        if (tokens[i] == "}")
        {
            i++;
            return;
        }
        else if (tokens[i] == "root")
        {
            parseRoot(tokens, i, location.root);
        }
        else if (tokens[i] == "index")
        {
            parseIndex(tokens, i, location.index);
        }
        else if (tokens[i] == "methods")
        {
            parseMethods(tokens, i, location);
        }
        else if (tokens[i] == "autoindex")
        {
            parseAutoIndex(tokens, i, location.autoIndex);
        }
        else if (tokens[i] == "upload")
        {
            parseUpload(tokens, i, location);
        }
        else if (tokens[i] == "upload_path")
        {
            parseUploadPath(tokens, i, location);
        }
        else if (tokens[i] == "cgi")
        {
            parseCgi(tokens, i, location);
        }
        else if (tokens[i] == "cgi_ext")
        {
            parseCgiExt(tokens, i, location);
        }
        else if (tokens[i] == "return")
        {
            parseReturn(tokens, i, location);
        }
        else if (tokens[i] == "error_page")
        {
            parseErrorPage(tokens, i, location.errorPages);
        }
        else
        {
            throw std::runtime_error("Unknown directive in location block: " + tokens[i]);
        }
    }
    
    throw std::runtime_error("Unexpected end of tokens in location block");
}
