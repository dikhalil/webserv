/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 20:31:56 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "HttpConfig.hpp"

class LocationConfig : public HttpConfig
{
    public:
        std::string path;
        std::vector<std::string> methods;
        bool upload;
        std::string uploadPath;
        bool cgi;
        std::vector<std::string> cgiExtensions;
        int redirectCode;
        std::string redirectUrl;
    
        LocationConfig();
};

#endif
