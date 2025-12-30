/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 20:27:57 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/30 23:18:07 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

unsigned long strToUL(const std::string& str);
std::string trim(const std::string& line);
std::string addSlash(const std::string& path);
bool dirExists(const std::string& path);
bool fileExists(const std::string& path);
bool hasAccess(const std::string& path, int mode);
std::string buildPath(const std::string& root, const std::string& locationPath, const std::string& uri);
std::string addSlash(const std::string& path, const std::string& next = "");
std::string joinPath(const std::string& a, const std::string& b);
void stripCRLF(std::string& body);

#endif