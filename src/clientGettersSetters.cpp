#include "Client.hpp"

void Client::setRealname(const std::string& real) 
{
    _realname = real;
}

void Client::setHostname(const std::string& host)
{
    _hostname = host;
}

const std::string& Client::getRealname() const
{
    return _realname;
}

const std::string& Client::getHostname() const
{
    return _hostname;
}

void Client::setSocketFd(int fd) { 
    socket_fd = fd; 
}

void Client::setNickname(const std::string& nick)
{
   _nickname = nick; 
}
void Client::setUsername(const std::string& user)
{
    _username = user;
}

const std::string& Client::getNickname() const
{
    return _nickname;
}
const std::string& Client::getUsername() const 
{
    return _username;
}

int Client::getSocketFd() const 
{
    return  socket_fd;
}

void Client::setRegistered(bool value) { _registered = value; }

void Client::setAuthenticated(bool value) { _authenticated = value; }
