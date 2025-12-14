/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 18:49:43 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>
#include <map>

class LocationConfig
{
    public:
        std::string path;
        std::string root;
        std::vector<std::string> index;
        std::vector<std::string> methods;
        std::map<int, std::string> errorPages;
        bool autoIndex;
        bool upload;
        std::string uploadPath;
        bool cgi;
        std::string cgiExtension;
        int redirectCode;
        std::string redirectUrl;
        
        LocationConfig();
        LocationConfig(const std::string& locationPath);
        ~LocationConfig();
        LocationConfig(const LocationConfig& other);
        LocationConfig& operator=(const LocationConfig& other);
        
        void setDefaults();
};

#endif
