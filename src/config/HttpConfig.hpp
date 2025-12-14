/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfig.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 15:10:15 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPCONFIG_HPP
#define HTTPCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "ServerConfig.hpp"

class HttpConfig
{
public:
    std::string root;
    std::vector<std::string> index;
    size_t clientMaxBodySize;
    bool autoIndex;
    std::map<int, std::string> errorPages;
    std::string cgiBinPath;
    std::vector<ServerConfig> servers;

    HttpConfig();
    ~HttpConfig();
    HttpConfig(const HttpConfig& other);
    HttpConfig& operator=(const HttpConfig& other);
    
    void setDefaults();
};

#endif
