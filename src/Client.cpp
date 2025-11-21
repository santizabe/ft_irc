/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmoundir <kmoundir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/10 14:03:12 by kmoundir          #+#    #+#             */
/*   Updated: 2025/08/09 14:52:35 by kmoundir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


Client::Client(int socket) : socket_fd(socket), _username("default"), _authenticated(false), _registered(false), _last_ping(time(NULL))
{
    _nickname = "default_nick";
    _realname = "default_realname";
    _hostname = "localhost";
    _buffer = "";
}

Client::Client(int socket, const std::string& hostname)
    : socket_fd(socket), _username("default"), _authenticated(false), _registered(false), _last_ping(time(NULL))
{
    _nickname = "default_nick";
    _realname = "default_realname";
    _hostname = hostname;
    _buffer = "";
}
Client::Client(const Client &other)
{
    socket_fd = other.socket_fd;
    _nickname = other._nickname;
    _username = other._username;
    _realname = other._realname;
    _hostname = other._hostname;
    _authenticated = other._authenticated;
    _registered = other._registered;
    _buffer = other._buffer;
    _last_ping = other._last_ping;
}
Client &Client::operator=(const Client &other)
{
    if (this != &other)
    {
        socket_fd = other.socket_fd;
        _nickname = other._nickname;
        _username = other._username;
        _realname = other._realname;
        _hostname = other._hostname;
        _authenticated = other._authenticated;
        _registered = other._registered;
        _buffer = other._buffer;
        _last_ping = other._last_ping;
    }
    return *this;
}

Client::~Client()
{
}
void Client::sendMessage(const std::string &message)
{
    std::string msg = message;
    if (msg.find("\r\n") == std::string::npos)
        msg += "\r\n";

    // Send the message to the client
    int bytes_sent = send(socket_fd, msg.c_str(), msg.length(), 0);
      
    if (bytes_sent < 0)
    {
        std::cerr << "Error: sending message" << std::endl;
    }
    else if (bytes_sent != static_cast<int>(msg.length()))
    {
        std::cout << "Warning: sent partial message" << std::endl;
    }
}

/*std::string Client::receiveMessage()
{}*/
void Client::sendNumericReply(int code, const std::string& message)
{
    std::stringstream ss;
    ss << ":" << _hostname << " " << std::setw(3) << std::setfill('0') << code << " " << (_nickname.empty() ? "*" : _nickname) << " :";
    
    std::string prefix = ss.str();
    size_t max_size = 512;
    size_t overhead = prefix.size() + 2; // +2 for \r\n
    std::string msg = message; // Initialize with the full message
    
    if (overhead + message.size() > max_size)
    {
        size_t maxMessageToSend = max_size - overhead;
        if (maxMessageToSend > 0)
            msg = message.substr(0, maxMessageToSend);
        else
            msg = "";
    }
    
    std::string reply = prefix + msg + "\r\n";
    
    int bytes_sent = send(socket_fd, reply.c_str(), reply.length(), 0);
    
    if (bytes_sent < 0)
    {
        std::cerr << "Error: sending Numeric reply" << std::endl;
    }
    else if (bytes_sent != static_cast<int>(reply.length()))
    {
       std::cout << "Warning: sent partial message of Numeric reply" << std::endl;
    }
}

// Authentication methods
bool Client::isAuthenticated() const 
{ 
    return _authenticated; 
}

bool Client::isRegistered() const
{
    return _registered;
}


void Client::appendToBuffer(const std::string& data)
{
    _buffer += data;
}

void Client::sendError(const std::string& message)
{
    std::string errorPrefix = "ERROR :";
    std::string fullMessage = errorPrefix + message;
    
    if (fullMessage.find("\r\n") == std::string::npos)
        fullMessage += "\r\n";

    // Send the message to the client
    int bytes_sent = send(socket_fd, fullMessage.c_str(), fullMessage.length(), 0);
    if (bytes_sent < 0)
    {
        std::cerr << "Error: sending error message" << std::endl;
    }
    else if (bytes_sent != static_cast<int>(fullMessage.length()))
    {
        std::cout << "Warning: sent partial message" << std::endl;
    }
}

bool Client::hasCompleteMessage()
{
    return (_buffer.find("\r\n") != std::string::npos || _buffer.find("\n") != std::string::npos);
}

std::string Client::extractMessage()
{
    size_t pos = _buffer.find("\r\n");
    if(pos == std::string::npos)
    {
        pos =_buffer.find("\n");
        if(pos == std::string::npos)
            return "";
    }
    std::string msg = _buffer.substr(0, pos);
   
    _buffer.erase(0, pos + ((_buffer[pos] == '\r') ? 2 : 1));
    return msg;
}

void Client::clearBuffer() {
    _buffer.clear();
}

std::string Client::getPrefix() {
    std::string prefix = ":" + _nickname;
    if (!_username.empty()) {
        prefix += "!" + _username;
    }
    if (!_hostname.empty()) {
        prefix += "@" + _hostname;
    }
    return prefix;
}

bool Client::canRegister() const
{
    return isAuthenticated()
        && !_nickname.empty()
        && _nickname != "default_nick"
        && !_username.empty()
        && _username != "default"
        && !_registered;
}
