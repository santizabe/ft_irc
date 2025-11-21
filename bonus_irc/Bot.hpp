/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmoundir <kmoundir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 17:50:23 by szapata-          #+#    #+#             */
/*   Updated: 2025/08/11 10:59:53 by kmoundir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

struct Meta
{
	std::string user;
	std::string nick;
	std::string host;
	std::string command;
	std::string target;
	std::string content;
	Meta(): user(""), nick(""), host(""), command(""), target(""), content("") {};
	~Meta() {};
};


class Bot {
	private:
		int sockfd;
		struct sockaddr_in server_addr;
		std::string ip;
		int port;
		std::string password;
		std::string _bufferBot;
	public:
		Bot(const std::string &, int, const std::string &);
		~Bot();
		void connectToServer();
		void sendMessage(const std::string &message);
		std::string receiveMessage();
		bool	handlePing(const std::string &message);
		void processMessage(const std::string &message);
		void handleIrcServMessage(const std::string &message);
};