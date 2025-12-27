/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 18:29:27 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/27 01:10:22 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest(const HttpConfig& config,
    const std::string& reqStr,
    const std::string& localIp,
    int localPort)
    : httpConfig(config), request(reqStr),server(NULL), loaction(NULL),
    _localIp(localIp), _localPort(localPort), redirectCode(0)
{
    parseRequest();
}


RequestStatus HttpRequest::parseRequest()
{
    std::istringstream requestStream(request);
    std::string line;
    
    if (!std::getline(requestStream, line))
        return REQ_BAD_REQUEST;
    if (!line.empty() && line[line.length() - 1] == '\r')
        line = line.substr(0, line.length() - 1);
    std::istringstream requestLineStream(line);
    requestLineStream >> method >> uri >> httpVersion;
    while (std::getline(requestStream, line) && line != "\r")
    {
        if (!line.empty() && line[line.length() - 1] == '\r')
            line = line.substr(0, line.length() - 1);
        size_t colonPos = line.find(": ");
        if (colonPos == std::string::npos)
            return REQ_BAD_REQUEST;
        std::string headerKey = line.substr(0, colonPos);
        std::string headerValue = line.substr(colonPos + 2);
        headers[headerKey] = headerValue;
    }
    std::ostringstream bodyStream;
    bodyStream << requestStream.rdbuf();
    body = bodyStream.str();
    return REQ_OK;
}

RequestStatus HttpRequest::isValidRequestLine()
{
    if (method.empty() || uri.empty() || httpVersion.empty())
        return REQ_BAD_REQUEST;
    if (method != "GET" && method != "POST" && method != "DELETE")
        return REQ_METHOD_NOT_ALLOWED;
    if (httpVersion != "HTTP/1.1")
        return REQ_VERSION_NOT_SUPPORTED;
    return REQ_OK;
}

bool HttpRequest::isCgiRequest(LocationConfig &loc, std::string const &uri)
{
    size_t dotPos;
    std::string ext;

    if (!loc.cgiEnabled)
        return false;
    dotPos = uri.rfind('.');
    if (dotPos == std::string::npos)
        return false;
    ext = uri.substr(dotPos + 1);
    for (size_t i = 0; i < loc.cgiExtensions.size(); i++)
    {
        if (ext == loc.cgiExtensions[i])
            return true;
    }
    return false;
}

RequestStatus HttpRequest::isValidHeader()
{
    const std::string host = headers.count("Host") ? headers.at("Host") : "";
  
    if (host.empty())
        return REQ_BAD_REQUEST;
    server = httpConfig.findServerByHost(host, _localIp, _localPort);
    if (!server)
        return REQ_BAD_REQUEST;
    location = &(server->findLocationByUri(uri));
    if (!location)
        return REQ_BAD_REQUEST;
    if (location->redirectCode != 0)
    {
        redirectCode = location->redirectCode;
        redirectUri = location->redirectUrl;
        if (redirectCode == 300)
            return REQ_MULTIPLE_CHOICES;
        if (redirectCode == 301)
            return REQ_MOVED_PERMANENTLY;
        if (redirectCode == 302)
            return REQ_FOUND;
    }
    if (std::find(location->allowedMethods.begin(), location->allowedMethods.end(), method) == location->allowedMethods.end())
        return REQ_METHOD_NOT_ALLOWED;

    struct stat rs;
    struct stat ps;
    std::string root;
    std::string path;
    bool pathExists;
    bool isDir;
    bool isFile;

    root = location->ctx.root;
    if (stat(root.c_str(), &rs) == -1 || !S_ISDIR(rs.st_mode))
        return REQ_INTERNAL_SERVER_ERROR;
    if (root[root.size() - 1] == '/')
        path = root.substr(0, root.size() - 1) + uri;
    else
        path = location->ctx.root + uri;
    pathExists = (stat(path.c_str(), &ps) == 0);
    isDir = pathExists && S_ISDIR(ps.st_mode());
    isFile = pathExists && S_ISREG(ps.st_mode());

    if (isCgiRequest(*location, uri))
    {
        if (pathExists && isFile)
        {
            finalPath = path;
            return REQ_CGI;
        }
    }
    if (method == "GET")
    {
        if (!pathExists)
            return REQ_NOT_FOUND;
        if (isDir)
        {
            
            for (size_t i = 0; i < location->ctx.index.size(); i++)
            {
                std::string indexPath = path;
                if (indexPath[indexPath.size() - 1] != '/')
                    indexPath += "/";
                indexPath += location->ctx.index[i];
                struct stat indexStat;
                if (stat(indexPath.c_str(), &indexStat) == 0 && S_ISREG(indexStat.st_mode))
                {
                    finalPath = indexPath;
                    return REQ_OK;
                }
            }
            if (location->ctx.autoIndex)
                return REQ_AUTOINDEX;
            return REQ_FORBIDDEN;
        }
        if (isFile)
        {
            finalPath = path;
            return REQ_OK;
        }
    }
    if (method == "DELETE")
    {
        if (!pathExists)
            return REQ_NOT_FOUND;
        if (isDir)
            return REQ_FORBIDDEN;
        if (isFile)
        {
            finalPath = path;
            return REQ_DELETE; 
        }
    }
    if (method == "POST")
    {
        if (!pathExists)
        {
            if (isCgiRequest(*location, uri))
                return REQ_NOT_FOUND;
            if (location->ctx.uploadEnabled)
            {
                struct stat uploadDir;
                if (stat(location->ctx.uploadDir.c_str(), &uploadDir) == 0
                    && S_ISDIR(uploadDir.st_mode))
                {
                    finalPath = location->ctx.uploadDir;
                    return REQ_UPLOAD;
                }
                else 
                    return REQ_FORBIDDEN;
            }
            else 
                return REQ_METHOD_NOT_ALLOWED;
        }
        else 
        {
            if (isCgiRequest(*location, uri) && isDir)
                return REQ_FORBIDDEN;
            return REQ_CONFLICT;
        }
    }
    return REQ_OK;
}

RequestStatus HttpRequest::isValidRequestBody()
{
    if (method == "GET" || method == "DELETE")
        return REQ_OK;
    if (body.empty() && isCgiRequest(*location, uri))
        return REQ_CGI;
    if (body.empty())
        return REQ_BAD_REQUEST;
    if (headers.count("Transfer-Encoding"))
    {
        if (headers.at("Transfer-Encoding") != "chunked")
            return REQ_NOT_IMPLEMENTED;
        return REQ_OK;
    }
    else if (headers.count("Content-Length"))
    {
        size_t cl = strToUL(msgBodyFraming);
        if (cl > location->ctx.clientMaxBodySize && clientMaxBodySize != 0)
            return 
    }
    else 
        return REQ_NOT_IMPLEMENTED;
    return REQ_OK;
}

bool isFatalStatus(RequestStatus status)
{
    return (
        status == REQ_BAD_REQUEST ||
        status == REQ_FORBIDDEN ||
        status == REQ_NOT_FOUND ||
        status == REQ_METHOD_NOT_ALLOWED ||
        status == REQ_CONFLICT ||
        status == REQ_LENGTH_REQUIRED ||
        status == REQ_PAYLOAD_TOO_LARGE ||
        status == REQ_URI_TOO_LONG ||
        status == REQ_INTERNAL_SERVER_ERROR ||
        status == REQ_NOT_IMPLEMENTED ||
        status == REQ_VERSION_NOT_SUPPORTED
    );
}


RequestStatus HttpRequest::isValidRequest()
{
    RequestStatus status;
    
    status = isValidRequestLine();
    if (isFatalStatus(status))
        return status;
    status = isValidHeader();
    if (isFatalStatus(status))
        return status;
    status = isValidRequestBody();
    return status;
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


