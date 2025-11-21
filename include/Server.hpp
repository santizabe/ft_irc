/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmoundir <kmoundir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 15:30:23 by kmoundir          #+#    #+#             */
/*   Updated: 2025/08/06 17:12:01 by kmoundir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "headers.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Server 
{
  private:
        int _port;
        std::string _password;
        int server_socket;
        std::vector<pollfd> pollFds;
        std::map<int, Client*> clients;
        struct addrinfo hints, *result;
        std::map<std::string, Channel*> channels;
       // Bot* bot;
  public:
        Server(int port, std::string password);
        ~Server();
        Server(const Server& other);
        Server& operator=(const Server& other);
        
        void start();
        void run();
        void stop();
        
        void handleNewConnection();
        void handleClientMessage(int fd);
        void removeClient(int fd);
        //void broadcastMessage(std::string message);
        Channel* createChannel(std::string name);
        Channel* getChannel(std::string name);
        Client* getClient(std::string nickname);
	  std::map<std::string, Channel*> &getChannels();
       // void addClient(Client* client);
       // bool authenticateClient(std::string password);

        // Getters and Setters
        int getPort() const;
        std::string getPassword() const;
        void setPort(int port);
        void setPassword(const std::string& password);
        
};

#endif