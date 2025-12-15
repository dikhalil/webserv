/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParserLocation.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 20:51:04 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ConfigParser.hpp>
#include <cstdlib>
#include <stdexcept>

void ConfigParser::parseMethods(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'methods'");
    
    if (tokens[i] == ";")
        throw std::runtime_error("'methods' directive cannot be empty");
    
    while (i < tokens.size() && tokens[i] != ";")
    {
        std::string method = tokens[i];
        if (isDirective(method))
            throw std::runtime_error("Missing ';' after 'methods' directive");
        if (method != "GET" && method != "POST" && method != "DELETE")
            throw std::runtime_error("Invalid HTTP method: " + method);
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
    
    if (tokens[i] == ";")
        throw std::runtime_error("'cgi_ext' directive cannot be empty");
    
    while (i < tokens.size() && tokens[i] != ";")
    {
        if (tokens[i].empty())
        {
            i++;
            continue;
        }
        
        if (isDirective(tokens[i]))
            throw std::runtime_error("Expected ';' after cgi_ext value, found directive: " + tokens[i]);
        
        if (tokens[i][0] != '.')
            throw std::runtime_error("CGI extension must start with '.': " + tokens[i]);
        
        location.cgiExtensions.push_back(tokens[i]);
        i++;
    }
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after cgi_ext value");
    i++;
}

void ConfigParser::parseReturn(std::vector<std::string>& tokens, size_t& i, LocationConfig& location)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected status code after 'return'");
    
    std::string codeStr = tokens[i];
    
    if (codeStr == ";")
        throw std::runtime_error("'return' directive requires a status code");
    
    for (size_t j = 0; j < codeStr.length(); ++j)
    {
        if (codeStr[j] < '0' || codeStr[j] > '9')
            throw std::runtime_error("Invalid return code: " + codeStr);
    }
    
    location.redirectCode = atoi(codeStr.c_str());
    
    if (!isValidHttpStatusCode(location.redirectCode))
        throw std::runtime_error("Invalid or unsupported HTTP status code for return: " + codeStr);
    
    i++;
    
    if (i >= tokens.size())
        throw std::runtime_error("Expected ';' after return code");
    
    if (tokens[i] == ";")
    {
        location.redirectUrl = "";
        i++;
        return;
    }
    
    location.redirectUrl = tokens[i];
    i++;
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after return values");
    i++;
}
