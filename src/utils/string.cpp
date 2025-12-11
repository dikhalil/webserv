/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 16:09:53 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/11 21:35:20 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

std::string trim(const std::string& line)
{
    size_t first;
    size_t last;
    
    first = line.find_first_not_of(" \t");
    if (first == std::string::npos)
        return "";
    last = line.find_last_not_of(" \t");
    return line.substr(first, last - first + 1);
}