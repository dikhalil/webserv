/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:33:02 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/11 21:03:01 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <Tokenizer.hpp>

struct LocationConfig
{
    std::string path;
    std::string root;
    std::string index;
    std::vector<std::string> methods;
    bool autoIndex;
    bool upload;
    std::string uploadPath;
    bool cgi;
    std::string cgiExtension;
    int redirectCode;
    std::string redirectUrl;
    
    // Constructor with default values
    LocationConfig() : autoIndex(false), upload(false), cgi(false), redirectCode(0) {}
};

struct ServerConfig
{
    std::vector<int> ports;
    std::string serverName;
    std::string root;
    std::string index;
    size_t clientMaxBodySize;
    std::map<int, std::string> errorPages;
    std::vector<LocationConfig> locations;
    
    // Constructor with default values
    ServerConfig() : index("index.html"), clientMaxBodySize(1048576) {} // 1MB default
};

class ConfigParser
{
    private:
        Tokenizer tokenizer;
        std::vector<ServerConfig> servers;
        
        void parseHttpBlock(std::vector<std::string>& tokens, size_t& i);
        void parseServerBlock(std::vector<std::string>& tokens, size_t& i, ServerConfig& server);
        void parseLocationBlock(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);

        void parseListen(std::vector<std::string>& tokens, size_t& i, ServerConfig& server);
        void parseServerName(std::vector<std::string>& tokens, size_t& i, ServerConfig& server);
        void parseRoot(std::vector<std::string>& tokens, size_t& i, ServerConfig& server);
        void parseIndex(std::vector<std::string>& tokens, size_t& i, ServerConfig& server);
        void parseErrorPage(std::vector<std::string>& tokens, size_t& i, ServerConfig& server);
        void parseClientMaxBodySize(std::vector<std::string>& tokens, size_t& i, ServerConfig& server);

        void parseMethods(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
        void parseAutoIndex(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
        void parseUpload(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
        void parseUploadPath(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
        void parseCgi(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
        void parseCgiExt(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
        void parseLocationRoot(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
        void parseLocationIndex(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
        void parseReturn(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);

        void validateServerConfig(ServerConfig& server);
        void validateLocationConfig(LocationConfig& location, bool serverHasRoot);
        void applyDefaults(ServerConfig& server);

    public:
        ConfigParser();
        ~ConfigParser();
        ConfigParser(const ConfigParser& other);
        ConfigParser& operator=(const ConfigParser& other);
        void parse(const std::string& filename);
        const std::vector<ServerConfig>& getServers() const;
};


#endif