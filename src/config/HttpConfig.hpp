/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfig.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 20:14:32 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPCONFIG_HPP
#define HTTPCONFIG_HPP

#include <string>
#include <vector>
#include <map>

class ServerConfig;

class HttpConfig
{
    private:
        std::vector<ServerConfig> servers;   
    public:
        std::string root;
        std::vector<std::string> index;
        std::string clientMaxBodySize;
        int autoIndex;
        std::map<int, std::string> errorPages;
        std::string cgiBinPath;
        
        HttpConfig();
        
        const std::vector<ServerConfig>& getServers() const;
        void addServer(const ServerConfig& server);
        void applyDefaults();
};

#endif
