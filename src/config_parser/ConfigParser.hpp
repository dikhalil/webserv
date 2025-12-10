/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:33:02 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/11 00:46:01 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "webserv.hpp"

struct LocationConfig
{
    std::string path;
    std::vector<std::string> methods;
    bool autoIndex;
    bool upload;
    std::string uploadPath;
    bool cgi;
    std::string cgiExtension;
    std::string defaultFile;
};

struct ServerConfig
{
    std::vector<int> ports;
    std::string serverName;
    std::string root;
    std::string index;
    size_t clientMaxBodySize;
    std::map<int, std::string> errorPages;
    std::vector<LocationConfig> locations;
};


class ConfigParser
{
    private:
        std::vector<ServerConfig> servers;
    public:
        ConfigParser();
        ~ConfigParser();
        ConfigParser(const ConfigParser& other);
        ConfigParser& operator=(const ConfigParser& other);
        void parse(const std::string& filename);
};

#endif