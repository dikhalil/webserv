/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 00:01:37 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/11 19:55:33 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>
#include <ConfigParser.hpp>

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
        std::cout << "✓ Configuration file parsed successfully!" << std::endl;
        std::cout << "Found " << configParser.getServers().size() << " server(s)" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}