/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 18:29:27 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/31 00:28:23 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "HttpRequestValidator.hpp" 
#include "HttpRequestHandler.hpp"   
#include "HttpRequestParser.hpp"

HttpRequest::HttpRequest(
    const HttpConfig& config,
    const std::string& reqStr,
    const std::string& localIp,
    int localPort)
    : httpConfig(config),
      server(NULL),
      location(NULL),
      _localIp(localIp),
      _localPort(localPort),
      request(reqStr),
      redirectCode(0)
{
        processRawRequest();
}

const std::string& HttpRequest::getMethod() const
{
    return method;
}

const std::string& HttpRequest::getUri() const
{
    return uri;
}

const std::string& HttpRequest::getHttpVersion() const
{
    return httpVersion;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const
{
    return headers;
}

const std::string& HttpRequest::getBody() const
{
    return body;
}



short HttpRequest::getRedirectCode() const
{
    return redirectCode;
}

const std::string& HttpRequest::getRedirectUri() const
{
    return redirectUri;
}

const RequestStatus &HttpRequest::getStatus() const
{
    return status;
}

const LocationConfig* HttpRequest::getLocation() const
{
    return location;
}

const ServerConfig* HttpRequest::getServer() const
{
    return server;
}

const std::string& HttpRequest::getFinalPath() const
{
    return finalPath;
}

const HttpConfig& HttpRequest::getHttpConfig() const
{
    return httpConfig;
}

const std::string HttpRequest::getLocalIp() const
{
    return _localIp;
}

int HttpRequest::getLocalPort() const
{
    return _localPort;
}

RequestStatus HttpRequest::processRawRequest()
{
    status = HttpRequestParser::parse(
        request,
        method,
        uri,
        httpVersion,
        headers,
        body
    );
    if (status != REQ_OK)
        return status;

    status = HttpRequestValidator::validate(*this,
        body, server, location, 
        redirectCode, redirectUri);
    if (status != REQ_OK)
        return status;

    status = HttpRequestHandler().handleRequest(*this, finalPath);
    if (status < 400)
        return status;

    HttpRequestHandler().setErrorPagePath(*this, finalPath);
    return status;
}
