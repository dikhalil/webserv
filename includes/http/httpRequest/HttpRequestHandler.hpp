/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/30 17:37:27 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/31 00:29:27 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include "utils.hpp"  
#include "HttpRequest.hpp"
#include "RequestStatus.hpp"

class HttpRequestHandler
{
    public:
        RequestStatus handleRequest(HttpRequest& req, std::string &finalPath);
    void setErrorPagePath(HttpRequest &req,  std::string &finalPath);

    private:
        RequestStatus handleCgi(HttpRequest& req, std::string &finalPath);
        RequestStatus handleGet(HttpRequest& req, std::string &finalPath);
        RequestStatus handlePost(HttpRequest& req, std::string &finalPath);
        RequestStatus handleDelete(HttpRequest& req);
        bool isCgiRequest(const HttpRequest& req);
        RequestStatus checkIndexFiles(HttpRequest& req, const std::string& dirPath, std::string &finalPath);
        RequestStatus deleteEntry(const std::string& path);
        RequestStatus deleteDir(const std::string& dirPath);

        template<typename T>
        bool findErrorPage(const T* block, int status, const std::string& root, std::string& outPath)
        {
            if (!block)
                return false;
            typename std::map<int, std::string>::const_iterator it = block->ctx.errorPages.find(status);
            if (it != block->ctx.errorPages.end())
            {
                std::string pagePath = it->second;
                outPath = joinPath(root, pagePath);
                if (fileExists(outPath) && hasAccess(outPath, R_OK))
                    return true;
            }
            return false;
        }
};
