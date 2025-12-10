/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:37:03 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/11 01:02:24 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

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
        servers = other.servers;
    }
    return *this;
}

void ConfigParser::parse(const std::string& filename)
{
    std::ifstream configFile(filename.c_str());
    std::string line;

    if (!configFile.is_open())
        throw std::runtime_error("Could not open config file: " + filename);
    while (std::getline(configFile, line))
    {
        std::cout << line << std::endl;
    }
    configFile.close();
}