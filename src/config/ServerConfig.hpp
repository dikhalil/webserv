/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 14:31:51 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "LocationConfig.hpp"

struct ListenConfig
{
    std::string address;
    int port;
    
    ListenConfig() : address("0.0.0.0") , port(80) {}
    ListenConfig(const std::string& addr, int p) : address(addr), port(p) {}
};

class ServerConfig
{
    public:
        std::vector<ListenConfig> listen;
        std::vector<std::string> serverNames;
        std::string root;
        std::vector<std::string> index;
        size_t clientMaxBodySize;
        std::map<int, std::string> errorPages;
        std::string cgiBinPath;
        std::vector<LocationConfig> locations;

        ServerConfig();
        ~ServerConfig();
        ServerConfig(const ServerConfig& other);
        ServerConfig& operator=(const ServerConfig& other);
        
        void setDefaults();
};

#endif
