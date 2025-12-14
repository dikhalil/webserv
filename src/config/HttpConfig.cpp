/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConfig.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/14 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 15:10:15 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpConfig.hpp"

HttpConfig::HttpConfig() : clientMaxBodySize(0), autoIndex(false)
{
    root = "";
}

HttpConfig::~HttpConfig() {}

HttpConfig::HttpConfig(const HttpConfig& other)
{
    *this = other;
}

HttpConfig& HttpConfig::operator=(const HttpConfig& other)
{
    if (this != &other)
    {
        root = other.root;
        index = other.index;
        clientMaxBodySize = other.clientMaxBodySize;
        autoIndex = other.autoIndex;
        errorPages = other.errorPages;
        cgiBinPath = other.cgiBinPath;
        servers = other.servers;
    }
    return *this;
}

void HttpConfig::setDefaults()
{
    if (clientMaxBodySize == 0)
        clientMaxBodySize = 1048576;
    
    if (index.empty())
        index.push_back("index.html");
}
