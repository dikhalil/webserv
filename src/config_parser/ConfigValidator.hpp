/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigValidator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 20:33:22 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/17 18:58:57 by dikhalil         ###   ########.fr       */
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
        static void validateMethod(const std::string& method);
        
        template<typename T>
        static bool isDuplicate(const std::vector<T>& list, const T& value)
        {
            for (size_t i = 0; i < list.size(); i++)
            {
                if (list[i] == value)
                    return true;
            }
            return false;
        }
        
        template<typename T>
        static bool hasCommonElement(const std::vector<T>& v1, const std::vector<T>& v2)
        {
            for (size_t i = 0; i < v1.size(); i++)
            {
                for (size_t j = 0; j < v2.size(); j++)
                {
                    if (v1[i] == v2[j])
                        return true;
                }
            }
            return false;
        }
        
        template<typename T, typename CompareFunc>
        static void compareAllPairs(const std::vector<T>& vec, CompareFunc compare)
        {
            for (size_t i = 0; i < vec.size(); i++)
            {
                for (size_t j = i + 1; j < vec.size(); j++)
                    compare(vec[i], vec[j]);
            }
        }
        
    private:
        static void validateServer(const ServerConfig& srv);
        static void validateLocation(const LocationConfig& loc);
        static void checkDuplicates(const HttpConfig& config);
        static void checkDupLocations(const HttpConfig& config);
        static void checkDupServers(const HttpConfig& config);
};

#endif
