/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 17:15:47 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 15:02:19 by dikhalil         ###   ########.fr       */
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
        void tokenizeFile(const std::string& filename);
        std::vector<std::string>& getTokens();
};

#endif