/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 17:15:47 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 13:24:57 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <webserv.hpp>

class Tokenizer
{
    private:
        std::vector<std::string> tokens;
        std::string removeComments(const std::string& line);
        std::string trim(const std::string& line);
        std::string addSpacesBetweenSymbol(std::string& line);
    public:
        Tokenizer();
        ~Tokenizer();
        Tokenizer(const Tokenizer& other);  
        Tokenizer& operator=(const Tokenizer& other);
        void tokenizeFile(const std::string& filename);
        std::vector<std::string>& getTokens();
};

#endif