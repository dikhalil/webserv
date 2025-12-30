/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 16:38:50 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/30 19:43:21 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include "utils.hpp"
#include "RequestStatus.hpp"

class HttpRequestParser
{
    public:
        static RequestStatus parse(
            const std::string& rawRequest,
            std::string& method,
            std::string& uri,
            std::string& httpVersion,
            std::map<std::string, std::string>& headers,
            std::string& body
        );

    private:
        static RequestStatus parseRequestLine(
            std::istringstream& stream,
            std::string& method,
            std::string& uri,
            std::string& httpVersion
        );

        static RequestStatus parseHeaders(
            std::istringstream& stream,
            std::map<std::string, std::string>& headers
        );
        
        static RequestStatus parseBody(
            std::istringstream& stream,
            std::string &body
        );
};
