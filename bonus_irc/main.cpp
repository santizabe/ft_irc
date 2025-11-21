/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmoundir <kmoundir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 16:48:37 by szapata-          #+#    #+#             */
/*   Updated: 2025/08/11 11:00:33 by kmoundir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

int	main(int ac, char **av)
{
	if (ac != 4) {
		std::cerr << "Usage: " << av[0] << " <IP> <Port> <Password>" << std::endl;
		return 1;
	}
	std::string ip = av[1];
	int port = std::atoi(av[2]);
	Bot bot(ip, port, std::string(av[3]));
	try {
		bot.connectToServer();
		std::string message;
		// while (true) {
			std::cout << "Enter message to send (or 'exit' to quit): ";
			std::getline(std::cin, message);
			// if (message == "exit")
			// 	break;
			bot.sendMessage(message);
			// std::string response = bot.receiveMessage();
			// std::cout << "Received: " << response << std::endl;
		//}
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}