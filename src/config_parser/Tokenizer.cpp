/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 17:15:41 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/16 21:30:18 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Tokenizer.hpp"

Tokenizer::Tokenizer() : pos(0) {}

std::string Tokenizer::removeComments(const std::string& line)
{
    size_t commentPos = line.find('#');
    if (commentPos != std::string::npos)
        return line.substr(0, commentPos);
    return line;
}

std::string Tokenizer::trim(const std::string& line)
{
    size_t first = line.find_first_not_of(" \t");
    if (first == std::string::npos)
        return "";
    size_t last = line.find_last_not_of(" \t");
    return line.substr(first, last - first + 1);
}

std::string Tokenizer::addSpacesBetweenSymbol(std::string& line)
{
    std::string modifiedLine;
    
    for (size_t i = 0; i < line.length(); ++i)
    {
        if (line[i] == '{' || line[i] == '}' || line[i] == ';')
        {
            modifiedLine += ' ';
            modifiedLine += line[i];
            modifiedLine += ' ';
        }
        else
        {
            modifiedLine += line[i];
        }
    }
    return modifiedLine;
}

void Tokenizer::tokenizeFile(const std::string& filename)
{
    std::ifstream configFile(filename.c_str());
    if (!configFile.is_open())
        throw std::runtime_error("Error: Could not open config file " + filename);
    
    std::string line;
    while (std::getline(configFile, line))
    {
        std::string cleanLine = removeComments(line);
        cleanLine = trim(cleanLine);
        if (cleanLine.empty())
            continue;
        cleanLine = addSpacesBetweenSymbol(cleanLine);
        
        std::stringstream ss(cleanLine);
        std::string token;
        while (ss >> token)
            tokens.push_back(token);
    }
    configFile.close();
    pos = 0;
}

std::vector<std::string>& Tokenizer::getTokens()
{
    return tokens;
}

bool Tokenizer::hasMore() const
{
    return pos < tokens.size();
}

std::string Tokenizer::peek() const
{
    if (!hasMore())
        throw std::runtime_error("Unexpected end of file");
    return tokens[pos];
}

std::string Tokenizer::consume()
{
    if (!hasMore())
        throw std::runtime_error("Unexpected end of file");
    return tokens[pos++];
}

void Tokenizer::expect(const std::string& expected)
{
    std::string token = consume();
    if (token != expected)
    {
        if (expected == ";" && (token == "location" || token == "server" || 
            token == "root" || token == "index" || token == "listen" || 
            token == "server_name" || token == "methods" || token == "autoindex" ||
            token == "error_page" || token == "client_max_body_size" || 
            token == "{" || token == "}"))
        {
            throw std::runtime_error("Missing semicolon ';' before '" + token + "'");
        }
        throw std::runtime_error("Expected '" + expected + "' but got '" + token + "'");
    }
}
