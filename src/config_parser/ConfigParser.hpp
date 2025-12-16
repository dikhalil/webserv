/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/17 00:38:32 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_NEW_HPP
#define CONFIGPARSER_NEW_HPP

#include "ConfigValidator.hpp"

class ConfigParser
{
    private:
        HttpConfig config;
        Tokenizer tokenizer;
        ConfigValidator validator;
        
        void parseHttp();
        void parseServer();
        void parseLocation(ServerConfig& server);
        bool parseContext(const std::string& dir, ConfigContext& ctx);
        bool parseLocDirective(const std::string& dir, LocationConfig& loc);
        
        void parseIndex(ConfigContext& ctx);
        void parseBodySize(ConfigContext& ctx);
        void parseAutoIndex(ConfigContext& ctx);
        void parseErrorPage(ConfigContext& ctx);
        
        void parseListen(ServerConfig& srv);
        void parseServerName(ServerConfig& srv);
        
        void parseMethods(LocationConfig& loc);
        void parseReturn(LocationConfig& loc);
        void parseCgiExt(LocationConfig& loc);
        
        void applyDefaults();
        
        void parseSimpleString(const std::string& directive, std::string& target);
        void parseSimpleBool(const std::string& directive, bool& target);
        
        std::string getValue(const std::string& directive);
        int parsePort(const std::string& port) const;
        long long parseSize(const std::string& val) const;
        bool parseBool(const std::string& val, const std::string& directive) const;
        ListenConfig parseListen(const std::string& val) const;
        
    public:
        ConfigParser();
        void parse(const std::string& filename);
        const HttpConfig& getConfig() const;
};

#endif
