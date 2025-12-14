/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:33:02 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 16:33:52 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <Tokenizer.hpp>
#include "../config/HttpConfig.hpp"
#include "ConfigValidator.hpp"

class ConfigParser
{
private:
    Tokenizer tokenizer;
    HttpConfig httpConfig;
    
    // Block parsers
    void parseHttpBlock(std::vector<std::string>& tokens, size_t& i);
    void parseServerBlock(std::vector<std::string>& tokens, size_t& i, ServerConfig& server);
    void parseLocationBlock(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);

    // Generic directive parsers (reusable)
    void parseIndex(std::vector<std::string>& tokens, size_t& i, std::vector<std::string>& target);
    void parseClientMaxBodySize(std::vector<std::string>& tokens, size_t& i, size_t& target);
    void parseErrorPage(std::vector<std::string>& tokens, size_t& i, std::map<int, std::string>& target);
    void parseAutoIndex(std::vector<std::string>& tokens, size_t& i, bool& target);
    void parseCgiBinPath(std::vector<std::string>& tokens, size_t& i, std::string& target);
    
    // Server-specific parsers
    void parseListen(std::vector<std::string>& tokens, size_t& i, ServerConfig& server);
    void parseServerName(std::vector<std::string>& tokens, size_t& i, ServerConfig& server);
    void parseRoot(std::vector<std::string>& tokens, size_t& i, std::string& target);
    
    // Location-specific parsers
    void parseMethods(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
    void parseUpload(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
    void parseUploadPath(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
    void parseCgi(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
    void parseCgiExt(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);
    void parseReturn(std::vector<std::string>& tokens, size_t& i, LocationConfig& location);

    // Helper functions
    bool isValidHttpStatusCode(int code);

    // Validation & inheritance
    void validateAndApplyDefaults();
    void inheritFromHttp(ServerConfig& server);
    void inheritFromServer(LocationConfig& location, const ServerConfig& server);

public:
    ConfigParser();
    ~ConfigParser();
    ConfigParser(const ConfigParser& other);
    ConfigParser& operator=(const ConfigParser& other);
    void parse(const std::string& filename);
    const HttpConfig& getHttpConfig() const;
    const std::vector<ServerConfig>& getServers() const;
};

#endif