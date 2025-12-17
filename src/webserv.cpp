/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/17 19:08:26 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

// template<typename Container, typename TransformFunc>
// static void printTransformed(const Container& container, TransformFunc transform, 
//                             const std::string& separator = ", ", const std::string& emptyMsg = "(none)")
// {
//     if (container.empty())
//     {
//         std::cout << emptyMsg;
//         return;
//     }
//     typename Container::const_iterator it = container.begin();
//     std::cout << transform(*it);
//     ++it;
//     for (; it != container.end(); ++it)
//         std::cout << separator << transform(*it);
// }

// template<typename Container>
// static void printWithSeparator(const Container& container, const std::string& separator = ", ", const std::string& emptyMsg = "(none)")
// {
//     if (container.empty())
//     {
//         std::cout << emptyMsg;
//         return;
//     }
//     typename Container::const_iterator it = container.begin();
//     std::cout << *it;
//     ++it;
//     for (; it != container.end(); ++it)
//         std::cout << separator << *it;
// }

// static void printVector(const std::vector<std::string>& vec, const std::string& emptyMsg = "(none)")
// {
//     printWithSeparator(vec, ", ", emptyMsg);
// }

// static std::string formatListen(const ListenConfig& listen)
// {
//     std::ostringstream oss;
//     oss << listen.host << ":" << listen.port;
//     return oss.str();
// }

// template<typename K, typename V, typename PrintFunc>
// static void printMap(const std::map<K, V>& map, PrintFunc printEntry, const std::string& indent = "")
// {
//     for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); ++it)
//         printEntry(it->first, it->second, indent);
// }

// static void printErrorPageEntry(int code, const std::string& path, const std::string& indent)
// {
//     std::cout << indent << "  " << code << " -> " << path << std::endl;
// }

// static void printContext(const ConfigContext& ctx, const std::string& indent)
// {
//     std::cout << indent << "Root: " << ctx.root << std::endl;
    
//     std::cout << indent << "Index: ";
//     printVector(ctx.index);
//     std::cout << std::endl;
    
//     std::cout << indent << "Client Max Body Size: ";
//     if (ctx.clientMaxBodySize == "0")
//         std::cout << "unlimited";
//     else
//         std::cout << ctx.clientMaxBodySize << " bytes";
//     std::cout << std::endl;
    
//     std::cout << indent << "Autoindex: " << (ctx.autoIndex == 1 ? "on" : "off") << std::endl;
    
//     if (!ctx.errorPages.empty())
//     {
//         std::cout << indent << "Error Pages:" << std::endl;
//         printMap(ctx.errorPages, printErrorPageEntry, indent);
//     }
//     else
//     {
//         std::cout << indent << "Error Pages: (none)" << std::endl;
//     }
// }

// static void printLocationConfig(const LocationConfig& loc, const std::string& indent)
// {
//     std::cout << indent << "[Location: " << loc.path << "]" << std::endl;
    
//     printContext(loc.ctx, indent + "  ");
    
//     std::cout << indent << "  Allowed Methods: ";
//     printVector(loc.allowedMethods, "GET");
//     std::cout << std::endl;
    
//     if (loc.redirectCode != 0)
//     {
//         std::cout << indent << "  Return: " << loc.redirectCode << " " 
//                   << loc.redirectUrl << std::endl;
//     }
//     else
//     {
//         std::cout << indent << "  Return: (none)" << std::endl;
//     }
    
//     if (loc.uploadEnabled)
//     {
//         std::cout << indent << "  Upload: enabled (path: " << loc.uploadPath << ")" << std::endl;
//     }
//     else
//     {
//         std::cout << indent << "  Upload: disabled" << std::endl;
//     }
    
//     if (loc.cgiEnabled)
//     {
//         std::cout << indent << "  CGI: enabled (extensions: ";
//         printVector(loc.cgiExtensions);
//         std::cout << ")" << std::endl;
//     }
//     else
//     {
//         std::cout << indent << "  CGI: disabled" << std::endl;
//     }
// }

// static void printServerConfig(const ServerConfig& server, size_t serverNum)
// {
//     std::cout << "--- Server #" << serverNum << " ---" << std::endl;
    
//     std::cout << "  Listen: ";
//     printTransformed(server.listen, formatListen);
//     std::cout << std::endl;
    
//     std::cout << "  Server Names: ";
//     printVector(server.serverNames, "(none)");
//     std::cout << std::endl;
    
//     printContext(server.ctx, "  ");
    
//     if (!server.locations.empty())
//     {
//         std::cout << "  Locations (" << server.locations.size() << "):" << std::endl;
//         for (size_t i = 0; i < server.locations.size(); ++i)
//         {
//             printLocationConfig(server.locations[i], "    ");
//         }
//     }
//     else
//     {
//         std::cout << "  Locations: (none)" << std::endl;
//     }
// }

// static void printHttpConfig(const HttpConfig& config)
// {
//     std::cout << "\n=== HTTP Configuration ===" << std::endl;
    
//     std::cout << "\nHTTP-Level Directives:" << std::endl;
//     printContext(config.ctx, "  ");
    
//     std::cout << "\nServers (" << config.servers.size() << "):" << std::endl;
//     for (size_t i = 0; i < config.servers.size(); ++i)
//     {
//         std::cout << std::endl;
//         printServerConfig(config.servers[i], i + 1);
//     }
// }

int main(int argc, char** argv)
{
    try
    {
        if (argc > 2)
        {
            std::cerr << "Error: Too many arguments!" << std::endl;
            std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
            return 1;
        }
        HttpConfig config;
        
        if (argc == 1)
            config.createDefaultConfig();
        else
        {
            std::string configFile = argv[1];
            ConfigParser parser;
            
            parser.parse(configFile);
            config = parser.getConfig();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
