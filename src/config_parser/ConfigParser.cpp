/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:33:28 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 18:55:02 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ConfigParser.hpp>
#include <cstdlib>
#include <stdexcept>

// ============================================================================
// Constructor & Destructor
// ============================================================================

ConfigParser::ConfigParser() {}

ConfigParser::~ConfigParser() {}

ConfigParser::ConfigParser(const ConfigParser& other)
{
    *this = other;
}

ConfigParser& ConfigParser::operator=(const ConfigParser& other)
{
    if (this != &other)
    {
        tokenizer = other.tokenizer;
        httpConfig = other.httpConfig;
    }
    return *this;
}

// ============================================================================
// Helper Functions
// ============================================================================

bool ConfigParser::isValidHttpStatusCode(int code)
{
    // Valid HTTP status codes based on webserv project requirements
    switch (code)
    {
        // 2xx - Success
        case 200: // OK
        case 201: // Created
        case 204: // No Content
        
        // 3xx - Redirection
        case 301: // Moved Permanently
        case 302: // Found
        case 303: // See Other
        case 304: // Not Modified
        case 307: // Temporary Redirect
        case 308: // Permanent Redirect
        
        // 4xx - Client Error
        case 400: // Bad Request
        case 401: // Unauthorized
        case 403: // Forbidden
        case 404: // Not Found
        case 405: // Method Not Allowed
        case 406: // Not Acceptable
        case 408: // Request Timeout
        case 409: // Conflict
        case 410: // Gone
        case 411: // Length Required
        case 413: // Payload Too Large
        case 414: // URI Too Long
        case 415: // Unsupported Media Type
        case 429: // Too Many Requests
        
        // 5xx - Server Error
        case 500: // Internal Server Error
        case 501: // Not Implemented
        case 502: // Bad Gateway
        case 503: // Service Unavailable
        case 504: // Gateway Timeout
        case 505: // HTTP Version Not Supported
            return true;
        
        default:
            return false;
    }
}

// ============================================================================
// Main Parse Function
// ============================================================================

void ConfigParser::parse(const std::string& filename)
{
    tokenizer.tokenizeFile(filename);
    std::vector<std::string>& tokens = tokenizer.getTokens();
    
    if (tokens.empty())
        throw std::runtime_error("Config file is empty");
    
    size_t i = 0;
    
    // Check if we have http block or direct server blocks
    if (tokens[i] == "http")
    {
        // Parse HTTP block
        parseHttpBlock(tokens, i);
    }
    else if (tokens[i] == "server")
    {
        // Direct server blocks (no HTTP wrapper)
        while (i < tokens.size())
        {
            if (tokens[i] == "server")
            {
                ServerConfig server;
                parseServerBlock(tokens, i, server);
                httpConfig.servers.push_back(server);
            }
            else
            {
                throw std::runtime_error("Expected 'server' block, found: " + tokens[i]);
            }
        }
    }
    else
    {
        throw std::runtime_error("Config must start with 'http' or 'server' block");
    }
    
    if (httpConfig.servers.empty())
        throw std::runtime_error("Config must contain at least one server block");
    
    validateAndApplyDefaults();
}

const HttpConfig& ConfigParser::getHttpConfig() const
{
    return httpConfig;
}

const std::vector<ServerConfig>& ConfigParser::getServers() const
{
    return httpConfig.servers;
}

// ============================================================================
// HTTP Block Parser
// ============================================================================

void ConfigParser::parseHttpBlock(std::vector<std::string>& tokens, size_t& i)
{
    // Expect "http"
    if (tokens[i] != "http")
        throw std::runtime_error("Expected 'http'");
    i++;
    
    // Expect "{"
    if (i >= tokens.size() || tokens[i] != "{")
        throw std::runtime_error("Expected '{' after 'http'");
    i++;
    
    bool hasServer = false;
    
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
            httpConfig.servers.push_back(server);
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

// ============================================================================
// Server Block Parser
// ============================================================================

void ConfigParser::parseServerBlock(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    // Expect "server"
    if (tokens[i] != "server")
        throw std::runtime_error("Expected 'server'");
    i++;
    
    // Expect "{"
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
        else if (tokens[i] == "cgi_bin_path")
        {
            parseCgiBinPath(tokens, i, server.cgiBinPath);
        }
        else if (tokens[i] == "location")
        {
            i++;
            if (i >= tokens.size())
                throw std::runtime_error("Expected location path after 'location'");
            
            // Check if next token is a valid path (not '{')
            if (tokens[i] == "{")
                throw std::runtime_error("Location must have a path (e.g., location / { ... })");
            
            LocationConfig location;
            location.path = tokens[i++];
            
            if (i >= tokens.size() || tokens[i] != "{")
                throw std::runtime_error("Expected '{' after location path");
            i++;
            
            parseLocationBlock(tokens, i, location);
            server.locations.push_back(location);
        }
        else
        {
            throw std::runtime_error("Unknown directive in server block: " + tokens[i]);
        }
    }
    
    throw std::runtime_error("Unexpected end of tokens in server block");
}

// ============================================================================
// Location Block Parser
// ============================================================================

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

// ============================================================================
// Generic Directive Parsers (Reusable)
// ============================================================================

void ConfigParser::parseIndex(std::vector<std::string>& tokens, size_t& i, std::vector<std::string>& target)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'index'");
    
    // Check if next token is semicolon (empty index)
    if (tokens[i] == ";")
        throw std::runtime_error("'index' directive cannot be empty");
    
    // Support multiple index pages
    while (i < tokens.size() && tokens[i] != ";")
    {
        target.push_back(tokens[i]);
        i++;
    }
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after index value(s)");
    i++;
}

void ConfigParser::parseClientMaxBodySize(std::vector<std::string>& tokens, size_t& i, size_t& target)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'client_max_body_size'");
    
    std::string value = tokens[i];
    size_t multiplier = 1;
    
    // Check for size suffix (K, M, G)
    if (!value.empty())
    {
        char last = value[value.length() - 1];
        if (last == 'M' || last == 'm')
        {
            multiplier = 1024 * 1024;
            value = value.substr(0, value.length() - 1);
        }
        else if (last == 'K' || last == 'k')
        {
            multiplier = 1024;
            value = value.substr(0, value.length() - 1);
        }
        else if (last == 'G' || last == 'g')
        {
            multiplier = 1024 * 1024 * 1024;
            value = value.substr(0, value.length() - 1);
        }
    }
    
    // Now validate the numeric part
    for (size_t j = 0; j < value.length(); ++j)
    {
        if (value[j] < '0' || value[j] > '9')
            throw std::runtime_error("Invalid client_max_body_size value: " + tokens[i]);
    }
    
    target = atoi(value.c_str()) * multiplier;
    i++;
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after client_max_body_size value");
    i++;
}

void ConfigParser::parseErrorPage(std::vector<std::string>& tokens, size_t& i, std::map<int, std::string>& target)
{
    i++;
    if (i + 1 >= tokens.size())
        throw std::runtime_error("Expected status code(s) and path after 'error_page'");
    
    // Collect all error codes until we find a path (starts with /)
    std::vector<int> codes;
    bool foundPath = false;
    
    while (i < tokens.size() && tokens[i] != ";")
    {
        std::string token = tokens[i];
        
        // Check if this is a path (starts with /)
        if (!token.empty() && token[0] == '/')
        {
            // This is the path, map all collected codes to it
            for (size_t j = 0; j < codes.size(); ++j)
            {
                target[codes[j]] = token;
            }
            foundPath = true;
            i++;
            break;
        }
        
        // Otherwise, it's an error code
        for (size_t j = 0; j < token.length(); ++j)
        {
            if (token[j] < '0' || token[j] > '9')
                throw std::runtime_error("Invalid error page code: " + token);
        }
        
        int code = atoi(token.c_str());
        
        // Validate HTTP status code
        if (!isValidHttpStatusCode(code))
            throw std::runtime_error("Invalid or unsupported HTTP status code for error_page: " + token);
        
        codes.push_back(code);
        i++;
    }
    
    if (codes.empty())
        throw std::runtime_error("error_page must have at least one status code");
    
    if (!foundPath)
        throw std::runtime_error("error_page must have a path (starts with /)");
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after error_page");
    i++;
}

void ConfigParser::parseAutoIndex(std::vector<std::string>& tokens, size_t& i, bool& target)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'autoindex'");
    
    std::string value = tokens[i];
    if (value == "on")
        target = true;
    else if (value == "off")
        target = false;
    else
        throw std::runtime_error("Invalid autoindex value (must be 'on' or 'off'): " + value);
    
    i++;
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after autoindex value");
    i++;
}

void ConfigParser::parseCgiBinPath(std::vector<std::string>& tokens, size_t& i, std::string& target)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'cgi_bin_path'");
    
    // Check if next token is semicolon (empty cgi_bin_path)
    if (tokens[i] == ";")
        throw std::runtime_error("'cgi_bin_path' directive cannot be empty");
    
    target = tokens[i++];
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after cgi_bin_path value");
    i++;
}

void ConfigParser::parseRoot(std::vector<std::string>& tokens, size_t& i, std::string& target)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'root'");
    
    // Check if next token is semicolon (empty root)
    if (tokens[i] == ";")
        throw std::runtime_error("'root' directive cannot be empty");
    
    target = tokens[i++];
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after root value");
    i++;
}

// ============================================================================
// Server-Specific Parsers
// ============================================================================

void ConfigParser::parseListen(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'listen'");
    
    std::string value = tokens[i];
    std::string address = "0.0.0.0";
    int port = 80;
    bool portSet = false;
    
    size_t colon = value.find(':');
    
    if (colon != std::string::npos)
    {
        // Format: address:port
        address = value.substr(0, colon);
        std::string portStr = value.substr(colon + 1);
        
        if (portStr.empty())
            throw std::runtime_error("Port number is empty in: " + value);
        
        for (size_t j = 0; j < portStr.length(); ++j)
        {
            if (portStr[j] < '0' || portStr[j] > '9')
                throw std::runtime_error("Invalid port number: " + portStr);
        }
        
        port = atoi(portStr.c_str());
        if (port < 1 || port > 65535)
            throw std::runtime_error("Port number out of range (1-65535): " + portStr);
        portSet = true;
    }
    else
    {
        // Check if it's a port number or IP address
        bool isPort = true;
        for (size_t j = 0; j < value.length(); ++j)
        {
            if (value[j] < '0' || value[j] > '9')
            {
                isPort = false;
                break;
            }
        }
        
        if (isPort)
        {
            // Format: port only
            port = atoi(value.c_str());
            if (port < 1 || port > 65535)
                throw std::runtime_error("Port number out of range (1-65535): " + value);
            portSet = true;
        }
        else
        {
            // Format: address only, use default port 80
            address = value;
            portSet = true;
        }
    }
    
    // Validate IP address if not 0.0.0.0
    if (address != "0.0.0.0")
    {
        size_t start = 0;
        int octetCount = 0;
        
        for (size_t j = 0; j <= address.length(); ++j)
        {
            if (j == address.length() || address[j] == '.')
            {
                if (j == start)
                    throw std::runtime_error("Empty octet in IP address: " + address);
                
                std::string octet = address.substr(start, j - start);
                
                for (size_t k = 0; k < octet.length(); ++k)
                {
                    if (octet[k] < '0' || octet[k] > '9')
                        throw std::runtime_error("Invalid character in IP address: " + address);
                }
                
                int num = atoi(octet.c_str());
                if (num < 0 || num > 255)
                    throw std::runtime_error("IP octet out of range (0-255): " + octet);
                
                octetCount++;
                start = j + 1;
            }
        }
        
        if (octetCount != 4)
            throw std::runtime_error("Invalid IP address format (must have 4 octets): " + address);
    }
    
    if (!portSet)
        throw std::runtime_error("listen directive must have a port");
    
    server.listen.push_back(ListenConfig(address, port));
    i++;
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after listen value");
    i++;
}

void ConfigParser::parseServerName(std::vector<std::string>& tokens, size_t& i, ServerConfig& server)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'server_name'");
    
    // Check if next token is semicolon (empty server_name)
    if (tokens[i] == ";")
        throw std::runtime_error("'server_name' directive cannot be empty");
    
    // Support multiple server names
    while (i < tokens.size() && tokens[i] != ";")
    {
        server.serverNames.push_back(tokens[i]);
        i++;
    }
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after server_name value(s)");
    i++;
}

// ============================================================================
// Location-Specific Parsers
// ============================================================================

void ConfigParser::parseMethods(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'methods'");
    
    // Check if next token is semicolon (empty methods)
    if (tokens[i] == ";")
        throw std::runtime_error("'methods' directive cannot be empty");
    
    while (i < tokens.size() && tokens[i] != ";")
    {
        std::string method = tokens[i];
        // Validate HTTP method
        if (method != "GET" && method != "POST" && method != "DELETE")
        {
            throw std::runtime_error("Invalid HTTP method: " + method);
        }
        location.methods.push_back(method);
        i++;
    }
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after methods values");
    i++;
}

void ConfigParser::parseUpload(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'upload'");
    
    std::string value = tokens[i];
    if (value == "on")
        location.upload = true;
    else if (value == "off")
        location.upload = false;
    else
        throw std::runtime_error("Invalid upload value (must be 'on' or 'off'): " + value);
    
    i++;
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after upload value");
    i++;
}

void ConfigParser::parseUploadPath(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'upload_path'");
    
    // Check if next token is semicolon (empty upload_path)
    if (tokens[i] == ";")
        throw std::runtime_error("'upload_path' directive cannot be empty");
    
    location.uploadPath = tokens[i++];
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after upload_path value");
    i++;
}

void ConfigParser::parseCgi(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'cgi'");
    
    std::string value = tokens[i];
    if (value == "on")
        location.cgi = true;
    else if (value == "off")
        location.cgi = false;
    else
        throw std::runtime_error("Invalid cgi value (must be 'on' or 'off'): " + value);
    
    i++;
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after cgi value");
    i++;
}

void ConfigParser::parseCgiExt(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'cgi_ext'");
    
    // Check if next token is semicolon (empty cgi_ext)
    if (tokens[i] == ";")
        throw std::runtime_error("'cgi_ext' directive cannot be empty");
    
    location.cgiExtension = tokens[i++];
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after cgi_ext value");
    i++;
}

void ConfigParser::parseReturn(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected status code after 'return'");
    
    // Parse the status code
    std::string codeStr = tokens[i];
    
    // Check if it's a semicolon (missing code)
    if (codeStr == ";")
        throw std::runtime_error("'return' directive requires a status code");
    
    for (size_t j = 0; j < codeStr.length(); ++j)
    {
        if (codeStr[j] < '0' || codeStr[j] > '9')
            throw std::runtime_error("Invalid return code: " + codeStr);
    }
    
    location.redirectCode = atoi(codeStr.c_str());
    
    // Validate HTTP status code
    if (!isValidHttpStatusCode(location.redirectCode))
        throw std::runtime_error("Invalid or unsupported HTTP status code for return: " + codeStr);
    
    i++;
    
    // Check for URL (optional)
    if (i >= tokens.size())
        throw std::runtime_error("Expected ';' after return code");
    
    // If next token is semicolon, return without URL (direct response)
    if (tokens[i] == ";")
    {
        location.redirectUrl = ""; // Empty URL means direct response with status code
        i++;
        return;
    }
    
    // Otherwise, parse the URL
    location.redirectUrl = tokens[i];
    i++;
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after return values");
    i++;
}

// ============================================================================
// Validation & Inheritance
// ============================================================================

void ConfigParser::validateAndApplyDefaults()
{
    // Apply HTTP defaults
    httpConfig.setDefaults();
    
    // Process each server
    for (size_t i = 0; i < httpConfig.servers.size(); ++i)
    {
        ServerConfig& server = httpConfig.servers[i];
        
        // Inherit from HTTP
        inheritFromHttp(server);
        
        // Apply server defaults
        server.setDefaults();
        
        // Validate server (includes duplicate checking)
        ConfigValidator::validateServerConfig(server);
        
        // Process each location in server
        for (size_t j = 0; j < server.locations.size(); ++j)
        {
            LocationConfig& location = server.locations[j];
            
            // Inherit from server
            inheritFromServer(location, server);
            
            // Apply location defaults
            location.setDefaults();
            
            // Validate location
            ConfigValidator::validateLocationConfig(location, server);
        }
    }
    
    // Check for duplicate servers (same IP:port:server_name across servers)
    ConfigValidator::checkDuplicateServers(httpConfig);
}

void ConfigParser::inheritFromHttp(ServerConfig& server)
{
    // Inherit root if server doesn't have it
    if (server.root.empty() && !httpConfig.root.empty())
        server.root = httpConfig.root;
    
    // Inherit index if server doesn't have it
    if (server.index.empty() && !httpConfig.index.empty())
        server.index = httpConfig.index;
    
    // Inherit clientMaxBodySize if not set
    if (server.clientMaxBodySize == 0)
        server.clientMaxBodySize = httpConfig.clientMaxBodySize;
    
    // Inherit error pages
    if (server.errorPages.empty() && !httpConfig.errorPages.empty())
        server.errorPages = httpConfig.errorPages;
    
    // Inherit cgi_bin_path
    if (server.cgiBinPath.empty() && !httpConfig.cgiBinPath.empty())
        server.cgiBinPath = httpConfig.cgiBinPath;
}

void ConfigParser::inheritFromServer(LocationConfig& location, const ServerConfig& server)
{
    // Inherit root if location doesn't have it
    if (location.root.empty())
        location.root = server.root;
    
    // Inherit index if location doesn't have it
    if (location.index.empty() && !server.index.empty())
        location.index = server.index;
}
