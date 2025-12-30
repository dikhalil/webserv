/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestValidator.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 17:06:52 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/31 00:26:59 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestValidator.hpp"

RequestStatus HttpRequestValidator::validate(
    const HttpRequest& req,
    std::string &body,
    const ServerConfig*& server,
    const LocationConfig*& location,
    short &redirectCode,
    std::string &redirectUri)
{
    RequestStatus status;

    status = validateRequestLine(req);
    if (status != REQ_OK)
        return status;
    status = validateHeaders(req, server, location, redirectCode, redirectUri);
    if (status != REQ_OK)
        return status;
    return validateBody(req, body);
}
RequestStatus HttpRequestValidator::validateRequestLine(const HttpRequest& req)
{
    const std::string& method = req.getMethod();
    const std::string& uri = req.getUri();
    const std::string& version = req.getHttpVersion();

    if (method.empty() || uri.empty() || version.empty())
        return REQ_BAD_REQUEST;
    if (method != "GET" && method != "POST" && method != "DELETE")
        return REQ_METHOD_NOT_ALLOWED;
    if (uri.length() > 2048)
        return REQ_URI_TOO_LONG;
    if (version != "HTTP/1.1")
        return REQ_VERSION_NOT_SUPPORTED;
    return REQ_OK;
}

RequestStatus HttpRequestValidator::validateHost(
    const HttpRequest& req,
    const HttpConfig& httpConfig,
    const std::string& localIp,
    int localPort,
    const ServerConfig*& server,
    const LocationConfig*& location)
{
    const std::map<std::string, std::string>& headers = req.getHeaders();
    if (!headers.count("Host"))
        return REQ_BAD_REQUEST;
    server = httpConfig.findServerByHost(headers.at("Host"), localIp, localPort);
    if (!server)
        return REQ_BAD_REQUEST;
    location = server->findLocationByUri(req.getUri());
    if (!location)
        return REQ_BAD_REQUEST;
    return REQ_OK;
}


RequestStatus HttpRequestValidator::checkRedirection(
    const LocationConfig *location,
    short &redirectCode,
    std::string &redirectUri)
{
    if (!location || location->redirectCode == 0)
        return REQ_OK;
    redirectCode = location->redirectCode;
    redirectUri  = location->redirectUrl;
    if (redirectCode == 300)
        return REQ_MULTIPLE_CHOICES;
    if (redirectCode == 301)
        return REQ_MOVED_PERMANENTLY;
    if (redirectCode == 302)
        return REQ_FOUND;
    return REQ_OK;
}

RequestStatus HttpRequestValidator::checkAllowedMethods(
    const HttpRequest& req,
    const LocationConfig *location)
{
    if (!location)
        return REQ_METHOD_NOT_ALLOWED;
    const std::vector<std::string>& allowed = location->allowedMethods;
    if (std::find(allowed.begin(), allowed.end(), req.getMethod()) == allowed.end())
        return REQ_METHOD_NOT_ALLOWED;
    return REQ_OK;
}

RequestStatus HttpRequestValidator::validateHeaders(
    const HttpRequest& req,
    const ServerConfig*& server,
    const LocationConfig*& location,
    short &redirectCode,
    std::string &redirectUri)
{
    RequestStatus status;
    const HttpConfig& httpConfig = req.getHttpConfig();
    const std::string& localIp = req.getLocalIp();
    int localPort = req.getLocalPort();
    status = validateHost(req, httpConfig, localIp, localPort, server, location);
    if (status != REQ_OK)
        return status;
    status = checkRedirection(location, redirectCode, redirectUri);
    if (status != REQ_OK)
        return status;
    status = checkAllowedMethods(req, location);
    if (status != REQ_OK)
        return status;
    return REQ_OK;
}

bool HttpRequestValidator::unchunkBody(std::string &body)
{
    std::istringstream stream(body);
    std::ostringstream unchunked;
    std::string line;
    while (std::getline(stream, line))
    {
        if (line == "0")
            break;
        size_t pos = line.find(';');
        if (pos != std::string::npos)
            line = line.substr(0, pos);
        size_t chunkSize;
        std::istringstream hex(line);
        hex >> std::hex >> chunkSize;
        if (hex.fail())
            return false;
        std::vector<char> buffer(chunkSize);
        stream.read(&buffer[0], chunkSize);
        if (stream.gcount() != (std::streamsize)chunkSize)
            return false;
        unchunked.write(&buffer[0], chunkSize);
        std::getline(stream, line);
    }
    body = unchunked.str();
    return true;
}

RequestStatus HttpRequestValidator::validateContentLength(const HttpRequest& req, const std::string &body)
{
    const std::map<std::string, std::string>& headers = req.getHeaders();
    if (!headers.count("Content-Length"))
        return REQ_NOT_IMPLEMENTED;
    size_t contentLength = strToUL(headers.at("Content-Length"));
    if (body.size() != contentLength)
        return REQ_BAD_REQUEST;
    return REQ_OK;
}

RequestStatus HttpRequestValidator::validateChunkedEncoding(const HttpRequest& req, std::string &body)
{
    const std::map<std::string, std::string>& headers = req.getHeaders();
    if (!headers.count("Transfer-Encoding"))
        return validateContentLength(req, body);
    if (headers.at("Transfer-Encoding") != "chunked")
        return REQ_NOT_IMPLEMENTED;
    if (!unchunkBody(body))
        return REQ_BAD_REQUEST;
    return REQ_OK;
}

RequestStatus HttpRequestValidator::validateBody(const HttpRequest& req, std::string &body)
{
    if (req.getMethod() != "POST")
        return REQ_OK;
    if (body.empty())
        return REQ_BAD_REQUEST;
    RequestStatus status = validateChunkedEncoding(req, body);
    if (status != REQ_OK)
        return status;
    size_t maxSize = strToUL(req.getLocation()->ctx.clientMaxBodySize);
    if (maxSize && body.size() > maxSize)
        return REQ_PAYLOAD_TOO_LARGE;
    return REQ_OK;
}
