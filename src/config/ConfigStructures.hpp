/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigStructures.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 20:38:58 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/17 00:38:24 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_STRUCTURES_HPP
#define CONFIG_STRUCTURES_HPP

#include "webserv.hpp"

struct ConfigContext
{
    std::string root;
    std::vector<std::string> index;
    std::string clientMaxBodySize;
    int autoIndex;
    std::map<int, std::string> errorPages;
    std::string cgiBinPath;

    ConfigContext();
    void inheritFrom(const ConfigContext& parent);
    void applyDefaults();
};

struct ListenConfig
{
    std::string host;
    int port;
    
    ListenConfig();
};

struct LocationConfig
{
    std::string path;
    ConfigContext ctx;
    std::vector<std::string> allowedMethods;
    int redirectCode;
    std::string redirectUrl;
    bool uploadEnabled;
    std::string uploadPath;
    bool cgiEnabled;
    std::vector<std::string> cgiExtensions;
    
    LocationConfig();
    void applyDefaults(std::vector<LocationConfig> &loc, ConfigContext &ctx);
};

struct ServerConfig
{
    std::vector<ListenConfig> listen;
    std::vector<std::string> serverNames;
    ConfigContext ctx;
    std::vector<LocationConfig> locations;
    
    ServerConfig();
    void applyDefaults(std::vector<ServerConfig> &srv, ConfigContext& ctx);
};

struct HttpConfig
{
    ConfigContext ctx;
    std::vector<ServerConfig> servers;
    
    HttpConfig();
};

#endif
