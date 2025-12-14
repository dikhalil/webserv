/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 17:15:41 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/14 13:24:36 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Tokenizer.hpp>

Tokenizer::Tokenizer() {}

Tokenizer::~Tokenizer() {}

Tokenizer::Tokenizer(const Tokenizer& other)
{
    *this = other;
}

Tokenizer& Tokenizer::operator=(const Tokenizer& other)
{
    if (this != &other)
    {
        tokens = other.tokens;
    }
    return *this;
}

std::string Tokenizer::removeComments(const std::string& line)
{
    size_t commentPos;
    std::string cleanLine;

    cleanLine = line;
    commentPos = line.find('#');
    if (commentPos != std::string::npos)
        cleanLine = line.substr(0, commentPos);
    return cleanLine;
}

std::string Tokenizer::trim(const std::string& line)
{
    size_t first;
    size_t last;
    std::string trimmedLine;
    
    first = line.find_first_not_of(" \t");
    if (first == std::string::npos)
        return "";
    last = line.find_last_not_of(" \t");
    trimmedLine = line.substr(first, last - first + 1);
    return trimmedLine;
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
    std::ifstream configFile;
    std::string line;
    std::string cleanLine;
    std::string token;

    configFile.open(filename.c_str());
    if (!configFile.is_open())
        throw std::runtime_error("Error: Could not open config file " + filename);
    while (std::getline(configFile, line))
    {
        cleanLine = removeComments(line);
        cleanLine = trim(cleanLine);
        if (cleanLine.empty())
            continue;
        cleanLine = addSpacesBetweenSymbol(cleanLine);
        std::stringstream ss(cleanLine);
        while (ss >> token)
        {
            tokens.push_back(token);
        }
    }
    configFile.close();
}

std::vector<std::string>& Tokenizer::getTokens()
{
    return tokens;
}