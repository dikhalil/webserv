/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestValidator.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 17:04:05 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/31 00:30:15 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "RequestStatus.hpp"
#include "HttpRequest.hpp"
#include "utils.hpp" 

class HttpRequestValidator
{
    public:
        static RequestStatus validate(
            const HttpRequest& req,
            std::string &body, 
            const ServerConfig*& server, 
            const LocationConfig*& location, 
            short &redirectCode, 
            std::string &redirectUri
        );

    private:
        static RequestStatus validateRequestLine(const HttpRequest& req);
        static RequestStatus validateHeaders(const HttpRequest& req,
                        const ServerConfig*& server,
                        const LocationConfig*& location,
                        short &redirectCode,
                        std::string &redirectUri);
        static RequestStatus validateBody(const HttpRequest& req, std::string &body);
        static RequestStatus validateHost(
            const HttpRequest& req,
            const HttpConfig& httpConfig,
            const std::string& localIp,
            int localPort,
            const ServerConfig*& server,
            const LocationConfig*& location);
        static RequestStatus checkRedirection(
            const LocationConfig *location,
            short &redirectCode,
            std::string &redirectUri);
        static RequestStatus checkAllowedMethods(const HttpRequest& req, const LocationConfig *location);
        static RequestStatus validateContentLength(const HttpRequest& req, const std::string &body);
        static RequestStatus validateChunkedEncoding(const HttpRequest& req, std::string &body);
        static bool unchunkBody(std::string &body);
};
