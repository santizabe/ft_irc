/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverGetterSetter.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmoundir <kmoundir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 22:00:29 by kmoundir          #+#    #+#             */
/*   Updated: 2025/08/04 21:48:44 by kmoundir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "debug.hpp"

int Server::getPort() const {
    return _port;
}

std::string Server::getPassword() const {
    return _password;
}

void Server::setPort(int port) {
    _port = port;
}

void Server::setPassword(const std::string& password) {
    _password = password;
}

std::map<std::string, Channel*> &Server::getChannels() {
    return channels;
}

Client* Server::getClient(std::string nickname)
{
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
        if (it->second->getNickname() == nickname)
            return it->second;
    }
    return NULL;
}

Channel* Server::getChannel(std::string name)
{
    DEBUG_LOG_SERVER("Looking for channel: '" << name << "'");
    std::map<std::string, Channel*>::iterator it = channels.find(name);
    if (it != channels.end())
    {   
            DEBUG_LOG_SERVER("Found channel: '" << it->second->getName() << "'");
            return it->second;
    }
    DEBUG_LOG_SERVER("Channel not found: '" << name << "'");
    return NULL;
}