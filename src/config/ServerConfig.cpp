/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 18:58:18 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

ServerConfig::ServerConfig() {}

const std::vector<LocationConfig>& ServerConfig::getLocations() const 
{ 
    return locations; 
}

void ServerConfig::addLocation(const LocationConfig& loc) 
{ 
    locations.push_back(loc); 
}
