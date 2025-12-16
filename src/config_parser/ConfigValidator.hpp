/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 20:33:22 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/17 00:38:39 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP

#include "ConfigStructures.hpp"
#include "Tokenizer.hpp"

class ConfigValidator
{
    public:
        static void validate(const HttpConfig& config);
        static bool isValidMethod(const std::string& method);
        static bool isValidStatus(int code);
        static bool isValidIP(const std::string& ip);
        static bool isValidPort(const std::string& port);
        static bool isReserved(const std::string& word);
        static bool isBlock(const std::string& token);
        static bool isDigits(const std::string& str);
        static void checkValue(const Tokenizer& tokenizer, const std::string& directive);
        static bool isDuplicate(const std::vector<std::string>& list, const std::string& value);
        static bool isDuplicateListen(const std::vector<ListenConfig>& list, const ListenConfig& conf);
        
    private:
        static void validateServer(const ServerConfig& srv);
        static void validateLocation(const LocationConfig& loc);
        static void checkDuplicates(const HttpConfig& config);
        static void checkDupLocations(const HttpConfig& config);
        static void checkDupServers(const HttpConfig& config);
        static bool hasSameName(const ServerConfig& s1, const ServerConfig& s2);
        static bool hasSameListen(const ServerConfig& s1, const ServerConfig& s2);
};

#endif
