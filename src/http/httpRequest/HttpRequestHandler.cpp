/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 17:37:19 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/31 00:27:43 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestHandler.hpp"
#include "HttpRequest.hpp"

RequestStatus HttpRequestHandler::handleRequest(HttpRequest& req, std::string &finalPath)
{
    const std::string root = req.getLocation()->ctx.root;
    if (!dirExists(root) || !hasAccess(root, R_OK | X_OK))
        return REQ_INTERNAL_SERVER_ERROR; 

    if (isCgiRequest(req))
        return handleCgi(req, finalPath);
    if (req.getMethod() == "GET")
        return handleGet(req, finalPath);
    else if (req.getMethod() == "POST")
        return handlePost(req, finalPath);
    else if (req.getMethod() == "DELETE")
        return handleDelete(req);
    return REQ_METHOD_NOT_ALLOWED;
}

bool HttpRequestHandler::isCgiRequest(const HttpRequest& req)
{
    if (!req.getLocation()->cgiEnabled)
        return false;
    size_t dotPos = req.getUri().rfind('.');
    if (dotPos == std::string::npos)
        return false;
    std::string ext = req.getUri().substr(dotPos + 1);
    for (size_t i = 0; i < req.getLocation()->cgiExtensions.size(); i++)
    {
        if (ext == req.getLocation()->cgiExtensions[i])
            return true;
    }
    return false;
    
}

RequestStatus HttpRequestHandler::handleCgi(HttpRequest& req, std::string &finalPath)
{
    std::string root = req.getLocation()->ctx.root;
    std::string cgiBin = req.getLocation()->ctx.cgiBinPath;
    std::string scriptName = req.getUri().substr(req.getUri().find_last_of('/') + 1);
    std::string cgiScriptPath = joinPath(joinPath(root, cgiBin), scriptName);
    if (!fileExists(cgiScriptPath))
        return REQ_NOT_FOUND;
    if (!hasAccess(cgiScriptPath, X_OK))
        return REQ_FORBIDDEN;
    finalPath = cgiScriptPath;
    // CGI execution logic here
    return REQ_CGI;
}


RequestStatus HttpRequestHandler::checkIndexFiles(
    HttpRequest& req,
    const std::string& dirPath,
    std::string &finalPath)
{
    for (size_t i = 0; i < req.getLocation()->ctx.index.size(); i++)
    {
        std::string indexPath = joinPath(dirPath, req.getLocation()->ctx.index[i]);
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
    if (req.getLocation()->ctx.autoIndex)
        return REQ_AUTOINDEX;
    return REQ_FORBIDDEN;
}

RequestStatus HttpRequestHandler::handleGet(HttpRequest& req, std::string &finalPath)
{
    const std::string root = req.getLocation()->ctx.root;
    const std::string path = buildPath(root, req.getLocation()->path, req.getUri());
    bool isDir = dirExists(path);
    bool isFile = fileExists(path);
    
    if (!isDir && !isFile)
        return REQ_NOT_FOUND;
    if (isDir)
        return checkIndexFiles(req, path, finalPath);
    if (isFile)
    {
        if (!hasAccess(path, R_OK))
            return REQ_FORBIDDEN;
        finalPath = path;
        return REQ_OK;
    }
    return REQ_NOT_FOUND;
}

RequestStatus HttpRequestHandler::deleteEntry(const std::string& path)
{
    if (dirExists(path))
        return deleteDir(path);
    if (!hasAccess(path, W_OK))
        return REQ_FORBIDDEN;
    if (remove(path.c_str()) != 0)
        return REQ_INTERNAL_SERVER_ERROR;
    return REQ_DELETE;
}

RequestStatus HttpRequestHandler::deleteDir(const std::string& dirPath)
{
    DIR* dir = opendir(dirPath.c_str());
    if (!dir)
        return REQ_INTERNAL_SERVER_ERROR;
    if (!hasAccess(dirPath, R_OK | X_OK))
    {
        closedir(dir);
        return REQ_FORBIDDEN;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;
        RequestStatus status = deleteEntry(dirPath + "/" + name);
        if (status != REQ_DELETE)
        {
            closedir(dir);
            return status;
        }
    }
    closedir(dir);
    if (rmdir(dirPath.c_str()) != 0)
        return errno == EACCES ? REQ_FORBIDDEN : REQ_INTERNAL_SERVER_ERROR;
    return REQ_DELETE;
}

RequestStatus HttpRequestHandler::handleDelete(HttpRequest& req)
{
    const std::string root = req.getLocation()->ctx.root;
    const std::string path = buildPath(root, req.getLocation()->path, req.getUri());
    bool isDir = dirExists(path);
    bool isFile = fileExists(path);
    
    if (!isDir && !isFile)
        return REQ_NOT_FOUND;
    return deleteEntry(path);
}

RequestStatus HttpRequestHandler::handlePost(HttpRequest& req, std::string &finalPath)
{
    if (!req.getLocation()->uploadEnabled)
        return REQ_METHOD_NOT_ALLOWED;
    std::string uploadPath = joinPath(req.getLocation()->ctx.root, req.getLocation()->uploadPath);
    std::string fileName = req.getUri().substr(req.getUri().find_last_of('/') + 1);
    if (fileName.empty() || fileName[fileName.size() - 1] == '/')
        return REQ_BAD_REQUEST;
    if (fileName.find('.') == std::string::npos)
        return REQ_BAD_REQUEST;
    std::string completePath = joinPath(uploadPath, fileName);
    if (dirExists(completePath))
        return REQ_BAD_REQUEST;
    if (!dirExists(uploadPath) || !hasAccess(uploadPath, W_OK | X_OK))
        return REQ_FORBIDDEN; 
    if (fileExists(completePath) && !hasAccess(completePath, W_OK))
        return REQ_CONFLICT; 
    int fd = open(completePath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
    {
        if (errno == EACCES)
            return REQ_FORBIDDEN;
        return REQ_INTERNAL_SERVER_ERROR;
    }
    size_t written = write(fd, req.getBody().c_str(), req.getBody().size());
    close(fd);
    if (written != req.getBody().size())
        return REQ_INTERNAL_SERVER_ERROR;
    finalPath = completePath;
    return REQ_UPLOAD;
}

void HttpRequestHandler::setErrorPagePath(HttpRequest &req, std::string &finalPath)
{
    std::string root = (req.getLocation() ? req.getLocation()->ctx.root : 
    (req.getServer() ? req.getServer()->ctx.root : req.getHttpConfig().ctx.root));
    if (findErrorPage(req.getLocation(), req.getStatus(), root, finalPath)) return;
    if (findErrorPage(req.getServer(), req.getStatus(), root, finalPath)) return;
    if (findErrorPage(&req.getHttpConfig(), req.getStatus(), root, finalPath)) return;
    // if not found handle it in response in body 
}