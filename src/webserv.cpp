/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:01:37 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/11 01:06:30 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

int main (int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Error: Try " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }
    std::string configFile;
    ConfigParser configParser;

    configFile = argv[1];
    try
    {
        configParser.parse(configFile);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}