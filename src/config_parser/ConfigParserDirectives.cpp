/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParserDirectives.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 20:10:47 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ConfigParser.hpp>
#include <cstdlib>
#include <stdexcept>

void ConfigParser::parseRoot(std::vector<std::string>& tokens, size_t& i, std::string& target)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'root'");
    
    if (tokens[i] == ";")
        throw std::runtime_error("'root' directive cannot be empty");
    
    target = tokens[i++];
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after root value");
    i++;
}

void ConfigParser::parseIndex(std::vector<std::string>& tokens, size_t& i, std::vector<std::string>& target)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'index'");    
    if (tokens[i] == ";")
        throw std::runtime_error("'index' directive cannot be empty");
    while (i < tokens.size() && tokens[i] != ";")
    {
        if (isDirective(tokens[i]))
            throw std::runtime_error("Missing ';' after 'index' directive");
        target.push_back(tokens[i]);
        i++;
    }
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after index value(s)");
    i++;
}

void ConfigParser::parseClientMaxBodySize(std::vector<std::string>& tokens, size_t& i, std::string& target)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'client_max_body_size'");
    
    std::string value = tokens[i];
    
    if (value.empty() || value == ";")
        throw std::runtime_error("'client_max_body_size' directive cannot be empty");
    
    long multiplier = 1;
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
    
    for (size_t j = 0; j < value.length(); ++j)
    {
        if (value[j] < '0' || value[j] > '9')
            throw std::runtime_error("Invalid client_max_body_size value: " + tokens[i]);
    }
    
    long result = atol(value.c_str()) * multiplier;
    if (result < 0)
        throw std::runtime_error("Invalid client_max_body_size value (negative): " + tokens[i]);
    
    std::stringstream ss;
    ss << result;
    target = ss.str();
    
    i++;
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after client_max_body_size value");
    i++;
}

void ConfigParser::parseAutoIndex(std::vector<std::string>& tokens, size_t& i, int& target)
{
    i++;
    if (i >= tokens.size())
        throw std::runtime_error("Expected value after 'autoindex'");
    
    std::string value = tokens[i];
    if (value == "on")
        target = 1;
    else if (value == "off")
        target = 0;
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
    
    if (tokens[i] == ";")
        throw std::runtime_error("'cgi_bin_path' directive cannot be empty");
    
    target = tokens[i++];
    
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Expected ';' after cgi_bin_path value");
    i++;
}

void ConfigParser::parseErrorPage(std::vector<std::string>& tokens, size_t& i, std::map<int, std::string>& target)
{
    i++;
    if (i + 1 >= tokens.size())
        throw std::runtime_error("Expected status code(s) and path after 'error_page'");    
    std::vector<int> codes;
    bool foundPath = false;
    while (i < tokens.size() && tokens[i] != ";")
    {
        std::string token = tokens[i];
        if (!token.empty() && token[0] == '/')
        {
            for (size_t j = 0; j < codes.size(); ++j)
            {
                target[codes[j]] = token;
            }
            foundPath = true;
            i++;
            break;
        }        
        for (size_t j = 0; j < token.length(); ++j)
        {
            if (token[j] < '0' || token[j] > '9')
                throw std::runtime_error("Invalid error page code: " + token);
        }
        int code = atoi(token.c_str());
        
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
