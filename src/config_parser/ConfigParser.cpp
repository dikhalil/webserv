/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 20:36:34 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/16 22:18:01 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

ConfigParser::ConfigParser() {}

void ConfigParser::parse(const std::string& filename)
{
    tokenizer.tokenizeFile(filename);
    
    if (tokenizer.getTokens().empty())
        throw std::runtime_error("Config file is empty");
    
    std::string firstToken = tokenizer.peek();
    
    if (firstToken == "http")
    {
        parseHttp();
    }
    else if (firstToken == "server")
    {
        while (tokenizer.hasMore())
        {
            if (tokenizer.peek() == "server")
                parseServer();
            else
                throw std::runtime_error("Expected 'server' block");
        }
    }
    else
    {
        throw std::runtime_error("Config must start with 'http' or 'server' block");
    }
    
    if (config.servers.empty())
        throw std::runtime_error("Config must contain at least one server block");
    
    if (tokenizer.hasMore())
    {
        std::string extraToken = tokenizer.peek();
        if (extraToken == "}")
            throw std::runtime_error("Unexpected closing brace '}'");
        else if (extraToken == "{")
            throw std::runtime_error("Unexpected opening brace '{'");
        else
            throw std::runtime_error("Unexpected token after config: '" + extraToken + "'");
    }
    
    applyDefaults();
    validator.validate(config);
}

bool ConfigParser::parseContext(const std::string& directive, ConfigContext& ctx)
{
    if (directive == "root")
        parseRoot(ctx);
    else if (directive == "index")
        parseIndex(ctx);
    else if (directive == "client_max_body_size")
        parseBodySize(ctx);
    else if (directive == "autoindex")
        parseAutoIndex(ctx);
    else if (directive == "error_page")
        parseErrorPage(ctx);
    else if (directive == "cgi_bin_path")
        parseCgiBin(ctx);
    else
        return false;
    return true;
}
std::string ConfigParser::getValue(const std::string& directive)
{
    // استخدام getValidValue من الـ Validator
    std::string val = ConfigValidator::getValidValue(tokenizer, directive);
    tokenizer.consume();
    return val;
}

void ConfigParser::parseHttp()
{
    tokenizer.expect("http");
    tokenizer.expect("{");
    
    bool hasServer = false;
    
    while (tokenizer.hasMore() && tokenizer.peek() != "}")
    {
        std::string directive = tokenizer.peek();
        
        if (directive == "server")
        {
            hasServer = true;
            parseServer();
        }
        else if (!parseContext(directive, config.ctx))
        {
            throw std::runtime_error("Unknown directive in http block: " + directive);
        }
    }
    
    tokenizer.expect("}");
    
    if (!hasServer)
        throw std::runtime_error("HTTP block must contain at least one server block");
}

void ConfigParser::parseServer()
{
    ServerConfig server;
    
    tokenizer.expect("server");
    tokenizer.expect("{");
    
    while (tokenizer.hasMore() && tokenizer.peek() != "}")
    {
        std::string directive = tokenizer.peek();
        
        if (directive == "listen")
            parseListen(server);
        else if (directive == "server_name")
            parseServerName(server);
        else if (directive == "location")
            parseLocation(server);
        else if (!parseContext(directive, server.ctx))
            throw std::runtime_error("Unknown directive in server block: " + directive);
    }
    
    tokenizer.expect("}");
    
    config.servers.push_back(server);
}

bool ConfigParser::parseLocDirective(const std::string& directive, LocationConfig& location)
{
    if (directive == "methods")
        parseMethods(location);
    else if (directive == "return")
        parseReturn(location);
    else if (directive == "upload")
        parseUpload(location);
    else if (directive == "upload_path")
        parseUploadPath(location);
    else if (directive == "cgi")
        parseCgi(location);
    else if (directive == "cgi_ext")
        parseCgiExt(location);
    else
        return false;
    return true;
}

void ConfigParser::parseLocation(ServerConfig& server)
{
    LocationConfig location;
    
    tokenizer.expect("location");
    
    if (!tokenizer.hasMore() || tokenizer.peek() == "{")
        throw std::runtime_error("Location must have a path (e.g., location / { ... })");
    
    location.path = tokenizer.consume();
    
    if (location.path.empty())
        throw std::runtime_error("Location path cannot be empty");
    
    if (location.path[0] != '/')
        throw std::runtime_error("Location path must start with '/', got: " + location.path);
    
    tokenizer.expect("{");
    
    while (tokenizer.hasMore() && tokenizer.peek() != "}")
    {
        std::string directive = tokenizer.peek();
        
        if (!parseContext(directive, location.ctx) && 
            !parseLocDirective(directive, location))
            throw std::runtime_error("Unknown directive in location block: " + directive);
    }
    
    tokenizer.expect("}");
    
    server.locations.push_back(location);
    
}

void ConfigParser::parseRoot(ConfigContext& ctx)
{
    parseSimpleString("root", ctx.root);
}

void ConfigParser::parseIndex(ConfigContext& ctx)
{
    tokenizer.consume();
    ConfigValidator::checkValue(tokenizer, "index");
    
    ctx.index.clear();
    while (tokenizer.hasMore() && tokenizer.peek() != ";")
    {
        std::string val = getValue("index");
        ctx.index.push_back(val);
    }
    
    tokenizer.expect(";");
}

void ConfigParser::parseBodySize(ConfigContext& ctx)
{
    tokenizer.consume();
    std::string val = getValue("client_max_body_size");
    long long size = parseSize(val);
    
    std::ostringstream oss;
    oss << size;
    ctx.clientMaxBodySize = oss.str();
    
    tokenizer.expect(";");
}

void ConfigParser::parseAutoIndex(ConfigContext& ctx)
{
    tokenizer.consume();
    std::string val = getValue("autoindex");
    ctx.autoIndex = parseBool(val, "autoindex") ? 1 : 0;
    tokenizer.expect(";");
}

void ConfigParser::parseErrorPage(ConfigContext& ctx)
{
    tokenizer.consume();
    
    if (!tokenizer.hasMore())
        throw std::runtime_error("error_page directive requires status code(s) and path");
    
    std::vector<int> codes;
    
    while (tokenizer.hasMore() && tokenizer.peek() != ";")
    {
        std::string tok = tokenizer.peek();
        
        if (tok.find_first_not_of("0123456789") == std::string::npos)
        {
            int code = std::atoi(tok.c_str());
            if (!validator.isValidStatus(code))
                throw std::runtime_error("Invalid HTTP status code: " + tok);
            codes.push_back(code);
            tokenizer.consume();
        }
        else
            break;
    }
    
    if (codes.empty())
        throw std::runtime_error("error_page directive requires at least one status code");
    
    if (!tokenizer.hasMore() || tokenizer.peek() == ";")
        throw std::runtime_error("error_page directive requires a path");
    
    std::string path = tokenizer.consume();
    
    for (size_t i = 0; i < codes.size(); i++)
        ctx.errorPages[codes[i]] = path;
    
    tokenizer.expect(";");
}

void ConfigParser::parseCgiBin(ConfigContext& ctx)
{
    parseSimpleString("cgi_bin_path", ctx.cgiBinPath);
}

void ConfigParser::parseListen(ServerConfig& server)
{
    tokenizer.consume();
    
    if (!tokenizer.hasMore())
        throw std::runtime_error("listen directive requires address:port or port");
    
    ListenConfig conf = parseListen(tokenizer.consume());
    
    if (server.listen.size() == 1 && 
        server.listen[0].host == "0.0.0.0" && 
        server.listen[0].port == 8080)
    {
        server.listen.clear();
    }
    
    bool hasDup = false;
    for (size_t i = 0; i < server.listen.size(); i++)
    {
        if (server.listen[i].host == conf.host && server.listen[i].port == conf.port)
        {
            hasDup = true;
            break;
        }
    }
    
    if (!hasDup)
        server.listen.push_back(conf);
    
    tokenizer.expect(";");
}

void ConfigParser::parseServerName(ServerConfig& server)
{
    tokenizer.consume();
    ConfigValidator::checkValue(tokenizer, "server_name");
    
    server.serverNames.clear();
    
    while (tokenizer.hasMore() && tokenizer.peek() != ";")
    {
        std::string name = getValue("server_name");
        server.serverNames.push_back(name);
    }
    
    tokenizer.expect(";");
}

void ConfigParser::parseMethods(LocationConfig& location)
{
    tokenizer.consume();
    ConfigValidator::checkValue(tokenizer, "methods");
    
    location.allowedMethods.clear();
    
    while (tokenizer.hasMore() && tokenizer.peek() != ";")
    {
        std::string method = tokenizer.consume();
        if (ConfigValidator::isReserved(method))
            throw std::runtime_error("Cannot use directive '" + method + "' as HTTP method");
        if (!validator.isValidMethod(method))
            throw std::runtime_error("Invalid HTTP method: " + method);
        
        // استخدام isDuplicate من الـ Validator
        if (!ConfigValidator::isDuplicate(location.allowedMethods, method))
            location.allowedMethods.push_back(method);
    }
    
    tokenizer.expect(";");
}

void ConfigParser::parseReturn(LocationConfig& location)
{
    tokenizer.consume();
    
    if (!tokenizer.hasMore())
        throw std::runtime_error("return directive requires status code and URL");
    
    std::string codeStr = tokenizer.consume();
    location.redirectCode = std::atoi(codeStr.c_str());
    
    if (!validator.isValidStatus(location.redirectCode))
        throw std::runtime_error("Invalid HTTP status code: " + codeStr);
    
    if (!tokenizer.hasMore() || tokenizer.peek() == ";")
        throw std::runtime_error("return directive requires a URL");
    
    location.redirectUrl = tokenizer.consume();
    
    tokenizer.expect(";");
}

// Helper: Parse simple string directive (directive value;)
void ConfigParser::parseSimpleString(const std::string& directive, std::string& target)
{
    tokenizer.consume();
    target = getValue(directive);
    tokenizer.expect(";");
}

// Helper: Parse simple boolean directive (directive on/off;)
void ConfigParser::parseSimpleBool(const std::string& directive, bool& target)
{
    tokenizer.consume();
    std::string val = getValue(directive);
    target = parseBool(val, directive);
    tokenizer.expect(";");
}

void ConfigParser::parseUpload(LocationConfig& location)
{
    parseSimpleBool("upload", location.uploadEnabled);
}

void ConfigParser::parseUploadPath(LocationConfig& location)
{
    parseSimpleString("upload_path", location.uploadPath);
}

void ConfigParser::parseCgi(LocationConfig& location)
{
    parseSimpleBool("cgi", location.cgiEnabled);
}

void ConfigParser::parseCgiExt(LocationConfig& location)
{
    tokenizer.consume();
    ConfigValidator::checkValue(tokenizer, "cgi_ext");
    
    location.cgiExtensions.clear();
    
    while (tokenizer.hasMore() && tokenizer.peek() != ";")
    {
        std::string ext = getValue("cgi_ext");
        location.cgiExtensions.push_back(ext);
    }
    
    tokenizer.expect(";");
}

void ConfigParser::applyLocDefaults(LocationConfig& loc)
{
    if (loc.allowedMethods.empty())
    {
        loc.allowedMethods.push_back("GET");
        loc.allowedMethods.push_back("POST");
        loc.allowedMethods.push_back("DELETE");
    }
}

void ConfigParser::applyServerDefaults(ServerConfig& srv)
{
    if (srv.serverNames.empty())
        srv.serverNames.push_back("localhost");
    
    if (srv.locations.empty())
    {
        LocationConfig defaultLoc;
        defaultLoc.path = "/";
        srv.locations.push_back(defaultLoc);
    }
}

void ConfigParser::applyDefaults()
{
    config.ctx.applyDefaults();
    
    for (size_t i = 0; i < config.servers.size(); i++)
    {
        ServerConfig& srv = config.servers[i];
        
        srv.ctx.inheritFrom(config.ctx);
        srv.ctx.applyDefaults();
        applyServerDefaults(srv);
        
        for (size_t j = 0; j < srv.locations.size(); j++)
        {
            LocationConfig& loc = srv.locations[j];
            
            loc.ctx.inheritFrom(srv.ctx);
            loc.ctx.applyDefaults();
            applyLocDefaults(loc);
        }
    }
}

const HttpConfig& ConfigParser::getConfig() const
{
    return config;
}


int ConfigParser::parsePort(const std::string& port) const
{
    if (!ConfigValidator::isValidPort(port))
        throw std::runtime_error("Invalid port: " + port);
    return std::atoi(port.c_str());
}

long long ConfigParser::parseSize(const std::string& val) const
{
    if (val.empty())
        throw std::runtime_error("Size value cannot be empty");
    
    char last = val[val.length() - 1];
    long long mult = 1;
        
    if (last == 'k' || last == 'K')
        mult = 1024;
    else if (last == 'm' || last == 'M')
        mult = 1024 * 1024;
    else if (last == 'g' || last == 'G')
        mult = 1024 * 1024 * 1024;
    
    std::string num = val.substr(0, val.length() - 1);

    if (!ConfigValidator::isDigits(num))
        throw std::runtime_error("Invalid size value: " + val);
    
    long long n = std::atoll(num.c_str());
    if (n < 0)
        throw std::runtime_error("Invalid size value: " + val);
    
    return n * mult;
}

bool ConfigParser::parseBool(const std::string& val, const std::string& directive) const
{
    if (val == "on")
        return true;
    else if (val == "off")
        return false;
    else
        throw std::runtime_error(directive + " must be 'on' or 'off', got: " + val);
}

ListenConfig ConfigParser::parseListen(const std::string& val) const
{
    ListenConfig conf;
    size_t colon = val.find(':');
    
    if (colon != std::string::npos)
    {
        conf.host = val.substr(0, colon);
        std::string port = val.substr(colon + 1);
        
        if (!ConfigValidator::isValidIP(conf.host))
            throw std::runtime_error("Invalid IP address: " + conf.host);
        
        conf.port = parsePort(port);
    }
    else if (ConfigValidator::isValidPort(val))
    {
        conf.host = "0.0.0.0";
        conf.port = parsePort(val);
    }
    else if (ConfigValidator::isValidIP(val))
    {
        conf.host = val;
        conf.port = 80;
    }
    else
    {
        throw std::runtime_error("Invalid listen directive: " + val);
    }
    
    return conf;
}
