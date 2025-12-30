/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 16:55:08 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/30 19:31:06 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestParser.hpp"

RequestStatus HttpRequestParser::parse(
    const std::string& raw,
    std::string& method,
    std::string& uri,
    std::string& version,
    std::map<std::string,std::string>& headers,
    std::string& body)
{
    std::istringstream stream(raw);

    RequestStatus status;

    status = parseRequestLine(stream, method, uri, version);
    if (status != REQ_OK)
        return status;

    status = parseHeaders(stream, headers);
    if (status != REQ_OK)
        return status;

    return parseBody(stream, body);
}

RequestStatus HttpRequestParser::parseRequestLine(
    std::istringstream& stream,
    std::string& method,
    std::string& uri,
    std::string& httpVersion)
{
    std::string line;

    if (!std::getline(stream, line))
        return REQ_BAD_REQUEST;
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    std::istringstream lineStream(line);
    if (!(lineStream >> method >> uri >> httpVersion))
        return REQ_BAD_REQUEST;
    return REQ_OK;
}


RequestStatus HttpRequestParser::parseHeaders(
    std::istringstream& stream,
    std::map<std::string, std::string>& headers)
{
    std::string line;

    while (std::getline(stream, line))
    {
        if (line == "\r" || line.empty())
            break;
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        size_t colonPos = line.find(": ");
        if (colonPos == std::string::npos)
            return REQ_BAD_REQUEST;
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 2);
        headers[key] = value;
    }
    return REQ_OK;
}

RequestStatus HttpRequestParser::parseBody(
    std::istringstream& stream,
    std::string& body)
{
    std::ostringstream bodyStream;
    bodyStream << stream.rdbuf();
    body = bodyStream.str();
    stripCRLF(body);
    return REQ_OK;
}


