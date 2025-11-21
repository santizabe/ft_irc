#ifndef MODEHANDLER_HPP
#define MODEHANDLER_HPP

#include "headers.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Parser.hpp"

class ModeHandler
{
	public:
		static void handle(const Message &msg, Client *client, Server *server);

	private:
		static bool checkModeOptions(std::string &token, std::istringstream &iss, std::vector<std::pair<std::string,std::string> > &mode_options, Client *client);
		static void addModeArgument(std::string &token, std::vector<std::pair<std::string, std::string> > &mode_options,
		std::istringstream &iss);
		static void executeMode(std::vector<std::pair<std::string, std::string> > &mode_options, std::string channelName, Client *client, Server *server);
		static void oMode(std::string mode, std::string arg, std::string channelName, Client *client, Server *server,
		std::string &modeChanges, std::vector<std::string> &modeParams);
		static void iMode(std::string mode, std::string channelName, Server *server, std::string &modeChanges);
		static void kMode(std::string mode, std::string arg, std::string channelName, Client *client, Server *server,
		std::string &modeChanges, std::vector<std::string> &modeParams);
		static void lMode(std::string mode, std::string arg, std::string channelName, Client *client, Server *server,
		std::string &modeChanges, std::vector<std::string> &modeParams);
		static void tMode(std::string mode, std::string channelName, Server *server, std::string &modeChanges);
};

#endif