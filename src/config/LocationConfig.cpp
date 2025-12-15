/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dikhalil <dikhalil@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 00:00:00 by dikhalil          #+#    #+#             */
/*   Updated: 2025/12/15 20:43:45 by dikhalil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

LocationConfig::LocationConfig()
    : path("")
    , upload(false)
    , uploadPath("")
    , cgi(false)
    , redirectCode(0)
    , redirectUrl("")
{}
