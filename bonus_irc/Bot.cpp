/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmoundir <kmoundir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 17:52:57 by szapata-          #+#    #+#             */
/*   Updated: 2025/08/11 10:59:42 by kmoundir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

Bot::Bot(const std::string &ip, int port, const std::string &pwd) : sockfd(-1), ip(ip), port(port), password(pwd) {
	try
	{
		connectToServer();
	}
	catch(const std::exception& e)
	{
		(void)e;
	}
}

Bot::~Bot() {}

void Bot::connectToServer()
{
	// Code to connect to the IRC server using ip and port
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		throw std::runtime_error("Error opening socket");
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
		throw std::runtime_error("Invalid address or address not supported");
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		throw std::runtime_error("Connection failed");
	std::cout << "Connected to server " << ip << " on port " << port << std::endl;
	
	std::string passCmd = "PASS " + password + "\r\n";
	send(sockfd, passCmd.c_str(), passCmd.length(), 0);

	std::string nickCmd = "NICK bot\r\n";
	send(sockfd, nickCmd.c_str(), nickCmd.length(), 0);

	std::string userCmd = "USER bot 0 * :bot\r\n";
	send(sockfd, userCmd.c_str(), userCmd.length(), 0);

	char buffer[512];

	while (true)
	{
		ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
		if (n <= 0)
		{
			std::cerr << "server closed\n";
			break;
		}

		buffer[n] = '\0';
		_bufferBot.append(buffer);

		size_t pos;
		while ((pos = _bufferBot.find("\r\n")) != std::string::npos)
		{
			std::string line = _bufferBot.substr(0, pos);
			_bufferBot.erase(0, pos + 2);
			if (handlePing(line))
				continue;
			std::cout << "Received: " << line << std::endl;
			processMessage(line);
		}
	}
}

bool Bot::handlePing(const std::string &message)
{
	if (message.substr(0, 4) == "PING") {
		std::string pongResponse = "PONG " + message.substr(5) + "\r\n";
		sendMessage(pongResponse);
		return true;
	}
	return false;
}

int	copy_str(char c, const std::string &message, std::string &str)
{
	size_t i = 0;
	size_t len = message.length();
	while (i < len && message[i] != c && message[i] != '\r' && message[i] != '\n') {
		str += message[i];
		i++;
	}
	return i + 1; // +1 to skip the character c
}

void	Bot::handleIrcServMessage(const std::string &message)
{
	if (message.find("Password incorrect") != std::string::npos) {
		std::cerr << "Password incorrect. Exiting..." << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}
}

void	Bot::processMessage(const std::string &message)
{
	if (message.empty())
		return;

	if (message.find("ircserv") != std::string::npos)
		return handleIrcServMessage(message);
	Meta meta;
	std::string::size_type	end = message.find('!');
	std::string::size_type	i = 1;
	
	meta.user = message.substr(i, end - i);
	i = end + 1;
	end = message.find('@', i);
	meta.nick = message.substr(i, end - i);
	i = end + 1;
	end = message.find(' ', i);
	meta.host = message.substr(i, end - i);
	i = end + 1;
	end = message.find(' ', i);
	meta.command = message.substr(i, end - i);
	i = end + 1;
	end = message.find(' ', i);
	meta.target = message.substr(i, end - i);
	i = message.find(':', i) + 1; // Skip the ':'
	meta.content = message.substr(i);
	if (meta.target != "bot")
		meta.user = meta.target;
	for (size_t j = 0; j < meta.command.length(); j++)
		meta.command[j] = toupper(meta.command[j]);
	if (meta.command == "INVITE")
	{
		sendMessage("JOIN " + meta.content + "\r\n");
		sendMessage("PRIVMSG " + meta.content + " : Hello " + meta.content + ". I am the irc bot. Did we get the bonus yet?" + "\r\n");
	}
	else if (meta.command == "PRIVMSG")
	{
		if (meta.content.find("!help") != std::string::npos)
			sendMessage("PRIVMSG " + meta.user + " : Here are the commands you can use: !help, !info, !quit\r\n");
		else if (meta.content.find("!info") != std::string::npos)
			sendMessage("PRIVMSG " + meta.user + " : I am a bot created to assist you in the IRC channel.\r\n");
		else if (meta.content.find("!hello") != std::string::npos)
			sendMessage("PRIVMSG " + meta.user + " : Hello " + meta.nick + "! How can I assist you today?\r\n");
		else if (meta.content.find("!quit") != std::string::npos)
		{
			sendMessage("PRIVMSG " + meta.user + " : Goodbye! I will now disconnect.\r\n");
			close(sockfd);
			exit(EXIT_SUCCESS);
		}
		else
			sendMessage("PRIVMSG " + meta.user + " : Hello " + meta.nick + "! Type !help to get some help\r\n");
	}
}

void Bot::sendMessage(const std::string &message) {
	if (send(sockfd, message.c_str(), message.length(), 0) < 0) {
		std::cerr << "Error sending message";
		exit(EXIT_FAILURE);
	}
}