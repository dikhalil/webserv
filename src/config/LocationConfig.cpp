/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 18:59:35 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

LocationConfig::LocationConfig()
{
    path = "";
    root = "";
    index.clear();
    methods.clear();
    errorPages.clear();
    autoIndex = false;
    upload = false;
    uploadPath = "";
    cgi = false;
    cgiExtension = "";
    redirectCode = 0;
    redirectUrl = "";
}

LocationConfig::LocationConfig(const std::string& locationPath) : path(locationPath)
{
    root = "";
    index.clear();
    methods.clear();
    errorPages.clear();
    autoIndex = false;
    upload = false;
    uploadPath = "";
    cgi = false;
    cgiExtension = "";
    redirectCode = 0;
    redirectUrl = "";
}

LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig(const LocationConfig& other)
{
    *this = other;
}

LocationConfig& LocationConfig::operator=(const LocationConfig& other)
{
    if (this != &other)
    {
        path = other.path;
        root = other.root;
        index = other.index;
        methods = other.methods;
        errorPages = other.errorPages;
        autoIndex = other.autoIndex;
        upload = other.upload;
        uploadPath = other.uploadPath;
        cgi = other.cgi;
        cgiExtension = other.cgiExtension;
        redirectCode = other.redirectCode;
        redirectUrl = other.redirectUrl;
    }
    return *this;
}

void LocationConfig::setDefaults()
{
    if (methods.empty())
        methods.push_back("GET");
    if (index.empty())
        index.push_back("index.html");
}
