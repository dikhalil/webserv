/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParserServer.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 16:52:02 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ConfigParser.hpp>
#include <cstdlib>
#include <stdexcept>

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
            port = atoi(value.c_str());
            if (port < 1 || port > 65535)
                throw std::runtime_error("Port number out of range (1-65535): " + value);
            portSet = true;
        }
        else
        {
            address = value;
            portSet = true;
        }
    }
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
    if (tokens[i] == ";")
        throw std::runtime_error("'server_name' directive cannot be empty");
    while (i < tokens.size() && tokens[i] != ";")
    {
        if (isDirective(tokens[i]))
            throw std::runtime_error("Missing ';' after 'server_name' directive");
        server.serverNames.push_back(tokens[i]);
        i++;
    }
    if (i >= tokens.size() || tokens[i] != ";")
        throw std::runtime_error("Missing ';' after server_name (found end of file)");
    i++;
}
