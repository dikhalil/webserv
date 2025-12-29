/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 23:31:33 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/29 18:54:39 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "ConfigValidator.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <dirent.h>
#include <cstdio>

enum RequestStatus
{
    REQ_OK = 200,
    REQ_DELETE = 204,
    REQ_UPLOAD = 210,
    REQ_CGI = 220, 
    REQ_AUTOINDEX = 230,
    REQ_MULTIPLE_CHOICES = 300,
    REQ_MOVED_PERMANENTLY = 301,
    REQ_FOUND = 302,
    REQ_BAD_REQUEST = 400,
    REQ_FORBIDDEN = 403, 
    REQ_NOT_FOUND = 404,
    REQ_METHOD_NOT_ALLOWED = 405,
    REQ_CONFLICT = 409,
    REQ_PAYLOAD_TOO_LARGE = 413,
    REQ_URI_TOO_LONG = 414,
    REQ_INTERNAL_SERVER_ERROR = 500, 
    REQ_NOT_IMPLEMENTED = 501,
    REQ_VERSION_NOT_SUPPORTED = 505
};

class HttpRequest
{
    private:
        const HttpConfig httpConfig;
        const std::string request;
        std::string _localIp;
        int _localPort;
        
        const LocationConfig *location;
        ServerConfig *server;
        short redirectCode;
        std::string redirectUri;
        std::string finalPath;
        
        std::string method;
        std::string uri;
        std::string httpVersion;
        std::map<std::string, std::string> headers;
        std::string body;
        
        RequestStatus status;
            
        bool isFatalStatus();
        bool isRedirectStatus();
        bool isCgiRequest();
        bool unchunkBody();
        bool dirExists(const std::string& path);
        bool fileExists(const std::string& path);
        bool hasAccess(const std::string& path, int mode);
        void stripCRLFFromBody();

        RequestStatus parseRequestLine(std::istringstream& stream);
        
        RequestStatus parseHeaders(std::istringstream& stream);
        RequestStatus validateHost();
        RequestStatus checkRedirection();
        RequestStatus checkAllowedMethods();
        
        RequestStatus validatePath();
        RequestStatus handleGetRequest(const std::string &root, const std::string& path,
             bool isDir, bool isFile);
        RequestStatus deleteEntry(const std::string &path);
        RequestStatus deleteDir(const std::string &dirPath);
        RequestStatus handleDeleteRequest(const std::string& path, bool isDir, bool isFile);
        RequestStatus handlePostRequest();
        RequestStatus checkIndexFiles(const std::string& dirPath);
        RequestStatus validateContentLength();
        RequestStatus validateChunkedEncoding();
        std::string extractFileNameFromUri();
            RequestStatus handleCgiRequest();
        void setErrorPagePath();
        template<typename T>
        bool findErrorPage(const T* block, int status, const std::string& root, std::string& outPath)
        {
            if (!block)
                return false;
            typename std::map<int, std::string>::const_iterator it = block->ctx.errorPages.find(status);
            if (it != block->ctx.errorPages.end())
            {
                outPath = addSlash(root);
                std::string pagePath = it->second;
                if (!pagePath.empty() && pagePath[0] == '/')
                    pagePath = pagePath.substr(1);
                outPath += pagePath;
                if (fileExists(outPath) && hasAccess(outPath, R_OK))
                    return true;
            }
            return false;
        }
        
    public:
        HttpRequest(const HttpConfig& config, const std::string& reqStr,
                    const std::string& localIp, int localPort);
        RequestStatus parseRequest();
        void isValidRequest();
        RequestStatus isValidRequestLine();
        RequestStatus isValidHeader();
        RequestStatus isValidRequestBody();
        const std::string& getMethod() const;
        const std::string& getUri() const;
        const std::string& getHttpVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        const std::string& getBody() const;
        const std::string& getFinalPath() const;
        short getRedirectCode() const;
        const std::string& getRedirectUri() const;
        const RequestStatus &getStatus() const;
};

#endif