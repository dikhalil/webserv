/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/26 18:29:27 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/29 18:53:42 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest(const HttpConfig& config,
    const std::string& reqStr,
    const std::string& localIp,
    int localPort)
    : httpConfig(config), request(reqStr), _localIp(localIp), 
    _localPort(localPort), location(NULL), server(NULL), redirectCode(0)
{
    isValidRequest();
    setErrorPagePath();
}

std::string buildPath(const std::string& root, const std::string& locationPath, const std::string& uri)
{
    std::string uriNoPrefix = uri;
    if (!locationPath.empty() && uri.find(locationPath) == 0)
    {
        uriNoPrefix = uri.substr(locationPath.length());
        if (uriNoPrefix.empty())
            uriNoPrefix = "/";
    }
    if (!uriNoPrefix.empty() && uriNoPrefix[0] == '/' && uriNoPrefix != "/")
        uriNoPrefix = uriNoPrefix.substr(1);
    if (root[root.size() - 1] == '/')
        return root + uriNoPrefix;
    else
        return addSlash(root) + uriNoPrefix;
}

RequestStatus HttpRequest::handleCgiRequest()
{
    if (method == "POST")
    {
        status = isValidRequestBody();
        if (isFatalStatus())
            return status;
    }
    std::string cgiScriptName = uri.substr(uri.find_last_of('/') + 1);
    std::string cgiScriptPath = location->ctx.cgiBinPath;
    if (!cgiScriptPath.empty() && cgiScriptPath[cgiScriptPath.size() - 1] != '/')
        cgiScriptPath += "/";
    cgiScriptPath += cgiScriptName;
    struct stat cgiStat;
    if (stat(cgiScriptPath.c_str(), &cgiStat) != 0)
        return REQ_NOT_FOUND;
    if (S_ISREG(cgiStat.st_mode))
    {
        if (hasAccess(cgiScriptPath, X_OK))
        {
            finalPath = cgiScriptPath;
            //cgi logic here
            return REQ_CGI;
        }
        else 
            return REQ_FORBIDDEN;
    }
    return REQ_OK;
}

bool HttpRequest::hasAccess(const std::string& path, int mode)
{
    return access(path.c_str(), mode) == 0;
}

bool HttpRequest::fileExists(const std::string& path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
}

bool HttpRequest::dirExists(const std::string& path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

void HttpRequest::setErrorPagePath()
{
    if (!isFatalStatus())
        return;
    std::string root = (location ? location->ctx.root : (server ? server->ctx.root : httpConfig.ctx.root));
    if (findErrorPage(location, status, root, finalPath)) return;
    if (findErrorPage(server, status, root, finalPath)) return;
    if (findErrorPage(&httpConfig, status, root, finalPath)) return;
    // if not found handle it in response in body 
}

RequestStatus HttpRequest::parseRequestLine(std::istringstream& requestStream)
{
    std::string line;
    
    if (!std::getline(requestStream, line))
        return REQ_BAD_REQUEST;
    if (!line.empty() && line[line.length() - 1] == '\r')
        line = line.substr(0, line.length() - 1);
    std::istringstream requestLineStream(line);
    if (!(requestLineStream >> method >> uri >> httpVersion))
        return REQ_BAD_REQUEST;
    return REQ_OK;
}

RequestStatus HttpRequest::parseHeaders(std::istringstream& requestStream)
{
    std::string line;
    
    while (std::getline(requestStream, line))
    {
        if (line == "\r" || line.empty())
            break;
        if (!line.empty() && line[line.length() - 1] == '\r')
            line = line.substr(0, line.length() - 1);
        size_t colonPos = line.find(": ");
        if (colonPos == std::string::npos)
            return REQ_BAD_REQUEST;
        std::string headerKey = line.substr(0, colonPos);
        std::string headerValue = line.substr(colonPos + 2);
        headers[headerKey] = headerValue;
    }
    return REQ_OK;
}

RequestStatus HttpRequest::parseRequest()
{
    std::istringstream requestStream(request);
    RequestStatus lineStatus = parseRequestLine(requestStream);
    if (lineStatus != REQ_OK)
        return lineStatus;
    RequestStatus headerStatus = parseHeaders(requestStream);
    if (headerStatus != REQ_OK)
        return headerStatus;
    std::ostringstream bodyStream;
    bodyStream << requestStream.rdbuf();
    body = bodyStream.str();
    stripCRLFFromBody();
    
    return REQ_OK;
}

RequestStatus HttpRequest::isValidRequestLine()
{
    if (method.empty() || uri.empty() || httpVersion.empty())
        return REQ_BAD_REQUEST;
    if (method != "GET" && method != "POST" && method != "DELETE")
        return REQ_METHOD_NOT_ALLOWED;
    if (uri.length() > 2048)
        return REQ_URI_TOO_LONG;
    if (httpVersion != "HTTP/1.1")
        return REQ_VERSION_NOT_SUPPORTED;
    return REQ_OK;
}

bool HttpRequest::isCgiRequest()
{
    if (!location || !location->cgiEnabled)
        return false;
    size_t dotPos = uri.rfind('.');
    if (dotPos == std::string::npos)
        return false;
    std::string ext = uri.substr(dotPos + 1);
    for (size_t i = 0; i < location->cgiExtensions.size(); i++)
    {
        if (ext == location->cgiExtensions[i])
            return true;
    }
    return false;
}

RequestStatus HttpRequest::validateHost()
{
    const std::string host = headers.count("Host") ? headers.at("Host") : "";
    
    if (host.empty())
        return REQ_BAD_REQUEST;
    server = httpConfig.findServerByHost(host, _localIp, _localPort);
    if (!server)
        return REQ_BAD_REQUEST;
    location = server->findLocationByUri(uri);
    if (!location)
        return REQ_BAD_REQUEST;    
    return REQ_OK;
}

RequestStatus HttpRequest::checkRedirection()
{
    if (location->redirectCode == 0)
        return REQ_OK;
    redirectCode = location->redirectCode;
    redirectUri = location->redirectUrl;
    if (redirectCode == 300)
        return REQ_MULTIPLE_CHOICES;
    if (redirectCode == 301)
        return REQ_MOVED_PERMANENTLY;
    if (redirectCode == 302)
        return REQ_FOUND;
    return REQ_OK;
}

RequestStatus HttpRequest::checkAllowedMethods()
{
    if (std::find(location->allowedMethods.begin(), 
                  location->allowedMethods.end(), 
                  method) == location->allowedMethods.end())
        return REQ_METHOD_NOT_ALLOWED;
    return REQ_OK;
}

RequestStatus HttpRequest::isValidHeader()
{
    RequestStatus hostStatus = validateHost();
    if (hostStatus != REQ_OK)
        return hostStatus;
    RequestStatus redirectStatus = checkRedirection();
    if (redirectStatus != REQ_OK)
        return redirectStatus;
    RequestStatus methodStatus = checkAllowedMethods();
    if (methodStatus != REQ_OK)
        return methodStatus;
    return validatePath();
}

RequestStatus HttpRequest::checkIndexFiles(const std::string& dirPath)
{
    for (size_t i = 0; i < location->ctx.index.size(); i++)
    {
        std::string indexPath = addSlash(dirPath) + location->ctx.index[i];
        if (fileExists(indexPath))
        {
            if (!hasAccess(indexPath, R_OK))
                return REQ_FORBIDDEN;
            finalPath = indexPath;
            return REQ_OK;
        }
    }
    if (!hasAccess(dirPath, R_OK | X_OK))
        return REQ_FORBIDDEN;
    if (location->ctx.autoIndex)
        return REQ_AUTOINDEX;
    return REQ_FORBIDDEN;
}

RequestStatus HttpRequest::handleGetRequest(const std::string &root, const std::string& path, 
    bool isDir, bool isFile)
{
    if (!isDir && !isFile)
        return REQ_NOT_FOUND;
    if (isDir)
    {
        if (!hasAccess(path, R_OK | X_OK))
            return REQ_FORBIDDEN;
        if (method == "GET")
            return checkIndexFiles(root);
        return REQ_FORBIDDEN;
    }
    if (isFile)
    {
        if (method == "GET")
            if (!hasAccess(path, R_OK))
                return REQ_FORBIDDEN;
        if (method == "DELETE")
            if (!hasAccess(path, W_OK))
                return REQ_FORBIDDEN;
        finalPath = path;
        return REQ_OK;
    }
    return REQ_NOT_FOUND;
}

RequestStatus HttpRequest::deleteEntry(const std::string &path)
{
    if (dirExists(path))
        return deleteDir(path);
    if (!hasAccess(path, W_OK))
        return REQ_FORBIDDEN;
    if (remove(path.c_str()) != 0)
        return REQ_INTERNAL_SERVER_ERROR;
    return REQ_DELETE;
}


RequestStatus HttpRequest::deleteDir(const std::string &dirPath)
{
    DIR *dir = opendir(dirPath.c_str());
    if (!dir)
        return REQ_INTERNAL_SERVER_ERROR;
    if (!hasAccess(dirPath, R_OK | X_OK))
    {
        closedir(dir);
        return REQ_FORBIDDEN;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;
        RequestStatus deleteStatus = deleteEntry(dirPath + "/" + name);
        if (deleteStatus != REQ_DELETE)
        {
            closedir(dir);
            return deleteStatus;
        }    
    }
    closedir(dir);
    if (rmdir(dirPath.c_str()) != 0)
        return errno == EACCES ? REQ_FORBIDDEN : REQ_INTERNAL_SERVER_ERROR;
    return REQ_DELETE;
}

RequestStatus HttpRequest::handleDeleteRequest(const std::string& path, bool isDir, bool isFile)
{
    if (!isDir && !isFile)
        return REQ_NOT_FOUND;
    return deleteEntry(path);
}

std::string HttpRequest::extractFileNameFromUri()
{
    size_t lastSlash = uri.rfind('/');
    std::string fileName;
    if (lastSlash != std::string::npos)
        fileName = uri.substr(lastSlash + 1);
    else
        fileName = uri;
    return fileName;
}
RequestStatus HttpRequest::handlePostRequest()
{
    status = isValidRequestBody();
    if (isFatalStatus())
        return status;
    if (!location->uploadEnabled)
        return REQ_METHOD_NOT_ALLOWED;
    std::string uploadPath = buildPath(location->ctx.root, location->path, location->uploadPath);
    std::string fileName = uri.substr(uri.find_last_of('/') + 1);
    if (fileName.empty() || fileName[fileName.size() - 1] == '/')
        return REQ_BAD_REQUEST;
    if (fileName.find('.') == std::string::npos)
        return REQ_BAD_REQUEST;
    finalPath = addSlash(uploadPath) + fileName;
    if (dirExists(finalPath))
        return REQ_BAD_REQUEST;
    if (!dirExists(uploadPath) || !hasAccess(uploadPath, W_OK | X_OK))
        return REQ_FORBIDDEN;
    if (fileExists(finalPath))
        return REQ_CONFLICT;
    int fd = open(finalPath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        if (errno == EACCES) return REQ_FORBIDDEN;
        return REQ_INTERNAL_SERVER_ERROR;    }
    write(fd, body.c_str(), body.size());
    close(fd);
    return REQ_UPLOAD;
}

RequestStatus HttpRequest::validatePath()
{
    std::string root = location->ctx.root;
    std::string path = buildPath(root, location->path, uri);
    if (!dirExists(root) || !hasAccess(root, R_OK | X_OK))
        return REQ_FORBIDDEN;
    bool isDir = dirExists(path);
    bool isFile = fileExists(path);
    if (isCgiRequest())
        return handleCgiRequest();
    if (method == "GET")
        return handleGetRequest(root, path, isDir, isFile);
    if (method == "DELETE")
        return handleDeleteRequest(path, isDir, isFile);
    if (method == "POST")
        return handlePostRequest();
    return REQ_OK;
}

void HttpRequest::stripCRLFFromBody()
{
    size_t pos = 0;
    while ((pos = body.find("\r\n", pos)) != std::string::npos)
    {
        body.erase(pos, 1);
        pos++;
    }
}

bool HttpRequest::unchunkBody()
{
    std::istringstream stream(body);
    std::ostringstream unchunked;
    std::string line;

    while (std::getline(stream, line))
    {
        if (line == "0")
            break;
        size_t semicolonPos = line.find(';');
        if (semicolonPos != std::string::npos)
            line = line.substr(0, semicolonPos);
        unsigned long chunkSize = 0;
        std::istringstream hexStream(line);
        hexStream >> std::hex >> chunkSize;
        if (hexStream.fail()) 
            return false;
        if (chunkSize == 0)
            break;
        std::vector<char> buffer(chunkSize);
        stream.read(buffer.data(), chunkSize);
        if (stream.gcount() != static_cast<std::streamsize>(chunkSize)) 
            return false;
        unchunked.write(buffer.data(), chunkSize);
        if (!std::getline(stream, line))
            break;
    }
    body = unchunked.str();
    return true;
}

RequestStatus HttpRequest::validateContentLength()
{
    if (!headers.count("Content-Length"))
        return REQ_NOT_IMPLEMENTED;
    size_t contentLength = strToUL(headers.at("Content-Length"));
    if (body.size() != contentLength)
        return REQ_BAD_REQUEST;
    return status;
}

RequestStatus HttpRequest::validateChunkedEncoding()
{
    if (!headers.count("Transfer-Encoding"))
        return validateContentLength();
    if (headers.at("Transfer-Encoding") != "chunked")
        return REQ_NOT_IMPLEMENTED;
    if (!unchunkBody())
        return REQ_BAD_REQUEST;
    return status;
}

RequestStatus HttpRequest::isValidRequestBody()
{
    if (body.empty())
        return REQ_BAD_REQUEST;    
    size_t clientMaxBodySize = strToUL(location->ctx.clientMaxBodySize);
    RequestStatus encodingStatus = validateChunkedEncoding();
    status = encodingStatus;
    if (isFatalStatus())
        return status;
    if (clientMaxBodySize && body.size() > clientMaxBodySize)
        return REQ_PAYLOAD_TOO_LARGE;
    return status;
}

bool HttpRequest::isFatalStatus()
{
    return (
        status == REQ_BAD_REQUEST ||
        status == REQ_FORBIDDEN ||
        status == REQ_NOT_FOUND ||
        status == REQ_METHOD_NOT_ALLOWED ||
        status == REQ_CONFLICT ||
        status == REQ_PAYLOAD_TOO_LARGE ||
        status == REQ_URI_TOO_LONG ||
        status == REQ_INTERNAL_SERVER_ERROR ||
        status == REQ_NOT_IMPLEMENTED ||
        status == REQ_VERSION_NOT_SUPPORTED
    );
}

bool HttpRequest::isRedirectStatus()
{
    return (
        status == REQ_MULTIPLE_CHOICES ||
        status == REQ_MOVED_PERMANENTLY ||
        status == REQ_FOUND
    );
}

void HttpRequest::isValidRequest()
{   
    status = parseRequest();
    if (isFatalStatus())
        return;
    status = isValidRequestLine();
    if (isFatalStatus())
        return;
    status = isValidHeader();
    if (isFatalStatus() || isRedirectStatus())
        return;
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

const std::string& HttpRequest::getFinalPath() const
{
    return finalPath;
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
