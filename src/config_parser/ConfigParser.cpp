/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:37:03 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/11 21:39:52 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ConfigParser.hpp>
#include <Tokenizer.hpp>

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser(const ConfigParser& other)
{
    *this = other;
}

ConfigParser& ConfigParser::operator=(const ConfigParser& other)
{
    if (this != &other)
        servers = other.servers;
    return *this;
}

void ConfigParser::parse(const std::string& filename)
{
    tokenizer.tokenizeFile(filename);
    std::vector<std::string>& tokens = tokenizer.getTokens();
    size_t i = 0;

    if (i >= tokens.size() || tokens[i] != "http")
        throw std::runtime_error("Expected 'http' at beginning");

    i++;
    if (i >= tokens.size() || tokens[i] != "{")
        throw std::runtime_error("Expected '{' after 'http'");
    i++;

    parseHttpBlock(tokens, i);
    
    if (i < tokens.size())
        throw std::runtime_error("Unexpected tokens after http block");
}

void ConfigParser::parseHttpBlock(std::vector<std::string>& tokens, size_t& i)
{
    bool hasServers = false;
    
    while (i < tokens.size())
    {
        if (tokens[i] == "{")
        {
            throw std::runtime_error("Unexpected '{' in http block - did you mean 'server {'?");
        }
        else if (tokens[i] == "server")
        {
            ServerConfig server;
            parseServerBlock(tokens, i, server);
            applyDefaults(server);
            validateServerConfig(server);
            servers.push_back(server);
            hasServers = true;
        }
        else if (tokens[i] == "}")
        {
            if (!hasServers)
                throw std::runtime_error("HTTP block must contain at least one server block");
            i++;
            return;
        }
        else
            throw std::runtime_error("Unexpected token in http block: " + tokens[i]);
    }
    throw std::runtime_error("Unexpected end of tokens in http block");
}

void ConfigParser::parseServerBlock(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    i++;
    if (i >= tokens.size() || tokens[i] != "{")
        throw std::runtime_error("Expected '{' after 'server'");
    i++;

    while (i < tokens.size())
    {
        if (tokens[i] == "}")
        {
            i++;
            if (server.ports.empty())
                throw std::runtime_error("Server block must have at least one 'listen' directive");
            
            if (server.root.empty())
            {
                if (server.locations.empty())
                    throw std::runtime_error("Server block must have 'root' directive or at least one location with 'root'");
                
                for (size_t j = 0; j < server.locations.size(); ++j)
                {
                    if (server.locations[j].root.empty())
                        throw std::runtime_error("Location '" + server.locations[j].path + 
                                               "' must have 'root' directive (server has no default root)");
                }
            }
            
            return;
        }
        else if (tokens[i] == "location")
        {
            LocationConfig location;
            parseLocationBlock(tokens, i, location);
            server.locations.push_back(location);
        }
        else if (tokens[i] == "listen")
            parseListen(tokens, i, server);
        else if (tokens[i] == "server_name")
            parseServerName(tokens, i, server);
        else if (tokens[i] == "root")
            parseRoot(tokens, i, server);
        else if (tokens[i] == "index")
            parseIndex(tokens, i, server);
        else if (tokens[i] == "error_page")
            parseErrorPage(tokens, i, server);
        else if (tokens[i] == "client_max_body_size")
            parseClientMaxBodySize(tokens, i, server);
        else
            throw std::runtime_error("Unexpected token in server block: " + tokens[i]);
    }
    throw std::runtime_error("Unexpected end of tokens in server block");
}

void ConfigParser::parseLocationBlock(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected location path");

    location.path = tokens[i];
    i++;
    if (i >= tokens.size() || tokens[i] != "{")
        throw std::runtime_error("Expected '{' after location path");
    i++;

    while (i < tokens.size())
    {
        if (tokens[i] == "}")
        {
            i++;
            return;
        }
        else if (tokens[i] == "methods")
            parseMethods(tokens, i, location);
        else if (tokens[i] == "root")
            parseLocationRoot(tokens, i, location);
        else if (tokens[i] == "index")
            parseLocationIndex(tokens, i, location);
        else if (tokens[i] == "autoindex")
            parseAutoIndex(tokens, i, location);
        else if (tokens[i] == "upload")
            parseUpload(tokens, i, location);
        else if (tokens[i] == "upload_path")
            parseUploadPath(tokens, i, location);
        else if (tokens[i] == "cgi")
            parseCgi(tokens, i, location);
        else if (tokens[i] == "cgi_ext")
            parseCgiExt(tokens, i, location);
        else if (tokens[i] == "return")
            parseReturn(tokens, i, location);
        else
            throw std::runtime_error("Unknown directive in location block: " + tokens[i]);
    }
    throw std::runtime_error("Unexpected end of tokens in location block");
}
void ConfigParser::parseListen(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'listen'");
    
    std::string value = tokens[i];
    size_t colon = value.find(':');
    
    if (colon == std::string::npos)
    {
        throw std::runtime_error("Invalid listen format (expected IP:PORT): " + value);
    }
    
    std::string ip = value.substr(0, colon);
    std::string portStr = value.substr(colon + 1);
    
    size_t start = 0;
    int octetCount = 0;
    
    for (size_t j = 0; j <= ip.length(); ++j)
    {
        if (j == ip.length() || ip[j] == '.')
        {
            if (j == start)
                throw std::runtime_error("Empty octet in IP address: " + ip);
            
            std::string octet = ip.substr(start, j - start);
            
            for (size_t k = 0; k < octet.length(); ++k)
            {
                if (octet[k] < '0' || octet[k] > '9')
                    throw std::runtime_error("Invalid character in IP address: " + ip);
            }
            
            int num = atoi(octet.c_str());
            if (num < 0 || num > 255)
                throw std::runtime_error("IP octet out of range (0-255): " + octet + " in " + ip);
            
            octetCount++;
            start = j + 1;
        }
    }
    
    if (octetCount != 4)
        throw std::runtime_error("Invalid IP address format (must have 4 octets): " + ip);
    
    if (portStr.empty())
        throw std::runtime_error("Port number is empty in: " + value);
    
    for (size_t j = 0; j < portStr.length(); ++j)
    {
        if (portStr[j] < '0' || portStr[j] > '9')
            throw std::runtime_error("Invalid port number: " + portStr);
    }
    
    int port = atoi(portStr.c_str());
    if (port < 1 || port > 65535)
        throw std::runtime_error("Port number out of range (1-65535): " + portStr);
    
    server.ports.push_back(port);
    i++;
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after listen value");
    i++;
}


void ConfigParser::parseServerName(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected value after 'server_name'");
    server.serverName = tokens[i++];
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after server_name value");
    i++;
}

void ConfigParser::parseRoot(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected value after 'root'");
    server.root = tokens[i++];
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after root value");
    i++;
}

void ConfigParser::parseIndex(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected value after 'index'");
    server.index = tokens[i++];
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after index value");
    i++;
}

void ConfigParser::parseErrorPage(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected error code(s) after 'error_page'");

    std::vector<int> codes;
    while (i < tokens.size() && tokens[i] != ";" && tokens[i].find_first_not_of("0123456789") == std::string::npos)
    {
        codes.push_back(atoi(tokens[i].c_str()));
        i++;
    }

    if (codes.empty()) 
        throw std::runtime_error("Expected at least one error code after 'error_page'");
    
    if (i >= tokens.size()) 
        throw std::runtime_error("Expected file path after error code(s)");

    std::string path = tokens[i++];
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after error_page declaration");
    i++;
    
    for (size_t j = 0; j < codes.size(); ++j)
        server.errorPages[codes[j]] = path;
}

void ConfigParser::parseClientMaxBodySize(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected value after 'client_max_body_size'");
    std::string val = tokens[i++];
    
    size_t num = 0;
    for (size_t j = 0; j < val.length(); ++j)
    {
        if (val[j] >= '0' && val[j] <= '9')
        {
            num = num * 10 + (val[j] - '0');
        }
        else if (val[j] == 'M' || val[j] == 'm')
        {
            num *= 1024 * 1024;
        }
        else if (val[j] == 'K' || val[j] == 'k')
        {
            num *= 1024;
        }
        else if (val[j] == 'G' || val[j] == 'g')
        {
            num *= 1024 * 1024 * 1024;
        }
        else
        {
            throw std::runtime_error("Invalid client_max_body_size value: " + val);
        }
    }
    server.clientMaxBodySize = num;
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after client_max_body_size value");
    i++;
}


void ConfigParser::parseMethods(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    while (i < tokens.size() && tokens[i] != ";")
    {
        location.methods.push_back(tokens[i++]);
    }
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after methods list");
    i++;
}

void ConfigParser::parseAutoIndex(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected 'on' or 'off' after 'autoindex'");
    std::string val = tokens[i++];
    if (val == "on") location.autoIndex = true;
    else if (val == "off") location.autoIndex = false;
    else throw std::runtime_error("Invalid value for autoindex: " + val);

    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after autoindex value");
    i++;
}

void ConfigParser::parseUpload(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected 'on' or 'off' after 'upload'");
    std::string val = tokens[i++];
    if (val == "on") location.upload = true;
    else if (val == "off") location.upload = false;
    else throw std::runtime_error("Invalid value for upload: " + val);

    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after upload value");
    i++;
}

void ConfigParser::parseUploadPath(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected value after 'upload_path'");
    location.uploadPath = tokens[i++];

    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after upload_path value");
    i++;
}

void ConfigParser::parseCgi(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected 'on' or 'off' after 'cgi'");
    std::string val = tokens[i++];
    if (val == "on") location.cgi = true;
    else if (val == "off") location.cgi = false;
    else throw std::runtime_error("Invalid value for cgi: " + val);

    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after cgi value");
    i++;
}

void ConfigParser::parseCgiExt(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected value after 'cgi_ext'");
    location.cgiExtension = tokens[i++];

    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after cgi_ext value");
    i++;
}

void ConfigParser::parseLocationRoot(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected value after 'root'");
    location.root = tokens[i++];
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after root value");
    i++;
}

void ConfigParser::parseLocationIndex(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected value after 'index'");
    location.index = tokens[i++];
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after index value");
    i++;
}

void ConfigParser::parseReturn(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size()) throw std::runtime_error("Expected status code after 'return'");
    
    std::string codeStr = tokens[i++];
    for (size_t j = 0; j < codeStr.length(); ++j)
    {
        if (codeStr[j] < '0' || codeStr[j] > '9')
            throw std::runtime_error("Invalid redirect status code: " + codeStr);
    }
    
    int code = atoi(codeStr.c_str());
    if (code < 300 || code > 399)
        throw std::runtime_error("Redirect status code must be 3xx: " + codeStr);
    
    if (i >= tokens.size()) throw std::runtime_error("Expected URL after redirect status code");
    
    location.redirectCode = code;
    location.redirectUrl = tokens[i++];
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after return directive");
    i++;
}


void ConfigParser::validateLocationConfig(LocationConfig& location, bool serverHasRoot)
{
    if (location.upload && location.uploadPath.empty())
        throw std::runtime_error("Location '" + location.path + "': upload is ON but upload_path is not set");
    
    if (location.cgi && location.cgiExtension.empty())
        throw std::runtime_error("Location '" + location.path + "': cgi is ON but cgi_ext is not set");
    
    if (!serverHasRoot && location.root.empty())
        throw std::runtime_error("Location '" + location.path + "': root is required when server has no root directive");
    
    if (location.methods.empty())
    {
        location.methods.push_back("GET");
        location.methods.push_back("POST");
        location.methods.push_back("DELETE");
    }
    
}

void ConfigParser::validateServerConfig(ServerConfig& server)
{
    if (server.ports.empty())
        throw std::runtime_error("Server block must contain at least one 'listen' directive");
    
    bool serverHasRoot = !server.root.empty();
    bool hasRootSomewhere = serverHasRoot;
    
    for (size_t i = 0; i < server.locations.size(); ++i)
    {
        if (!server.locations[i].root.empty())
            hasRootSomewhere = true;
    }
    
    if (!hasRootSomewhere)
        throw std::runtime_error("Server must have 'root' directive or at least one location with 'root'");
    
    for (size_t i = 0; i < server.locations.size(); ++i)
    {
        validateLocationConfig(server.locations[i], serverHasRoot);
    }
}

void ConfigParser::applyDefaults(ServerConfig& server)
{
    
    if (server.serverName.empty())
        server.serverName = "localhost";
}

const std::vector<ServerConfig>& ConfigParser::getServers() const
{
    return servers;
}
