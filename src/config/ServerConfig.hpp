/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 18:42:36 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "HttpConfig.hpp"

class LocationConfig;

struct ListenConfig
{
    std::string address;
    int port;
    
    ListenConfig() : address("0.0.0.0") , port(80) {}
    ListenConfig(const std::string& addr, int p) : address(addr), port(p) {}
};

class ServerConfig : public HttpConfig
{
    private:
        std::vector<LocationConfig> locations;   
    public:
        std::vector<ListenConfig> listen;
        std::vector<std::string> serverNames;
        ServerConfig();
        
        const std::vector<LocationConfig>& getLocations() const;
        void addLocation(const LocationConfig& loc);
};

#endif
