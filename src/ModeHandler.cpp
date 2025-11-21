#include "ModeHandler.hpp"

void ModeHandler::handle(const Message &msg, Client *client, Server *server)
{
	if (!client->isAuthenticated() && !server->getPassword().empty())
	{
		client->sendMessage(":ircserv 464 * :Password required\r\n");
		return ;
	}
	if (!client->isRegistered())
	{
		client->sendMessage(":ircserv 451 * :You have not registered\r\n");
		return ;
	}
	if (msg.params.empty())
	{
		client->sendMessage(":ircserv 461 MODE :Not enough parameters\r\n");
		return ;
	}
	std::string channelName = msg.params[0];
	Channel *channel = server->getChannel(channelName);
	if (!channel)
	{
		client->sendMessage(":ircserv 403 " + channelName + " :No such channel\r\n");
		return ;
	}
	if (msg.params.size() == 1)
	{
		std::string modes = "+";
		if (channel->isInviteOnly())
			modes += "i";
		if (channel->isTopicRestricted())
			modes += "t";
		if (channel->hasKey())
			modes += "k";
		if (channel->getUserLimit() > 0)
			modes += "l";
		client->sendMessage(":ircserv 324 " + client->getNickname() + " " + channelName + " " + modes + "\r\n");
		return ;
	}
	if (!channel->isOperator(client))
	{
		client->sendMessage(":ircserv 482 " + channelName + " :You're not channel operator\r\n");
		return ;
	}
	std::string fullCommand = "MODE " + channelName;
	for (size_t i = 1; i < msg.params.size(); ++i)
		fullCommand += " " + msg.params[i];
	if (!msg.trailing.empty())
		fullCommand += " :" + msg.trailing;
	std::istringstream iss(fullCommand);
	std::string command, chanName;
	iss >> command >> chanName;
	std::vector<std::pair<std::string, std::string> > mode_options;
	std::string token;
	if (!(iss >> token))
		return ;
	if (!checkModeOptions(token, iss, mode_options, client))
		return ;
	addModeArgument(token, mode_options, iss);
	std::string modeChanges;
	std::vector<std::string> modeParams;
    executeMode(mode_options, channelName, client, server);
}

bool ModeHandler::checkModeOptions(std::string &token, std::istringstream &iss, std::vector<std::pair<std::string, std::string> > &mode_options, Client *client)
{
	(void)client;
    std::string mode;
    std::string sign;
    if (token[0] != '+' && token[0] != '-')
        return  false;
    while (!token.empty() && (token[0] == '+' || token[0] == '-'))
    {
        sign = token[0];
        for (size_t i = 1; i < token.size(); i++)
        {
            if (token[i] == 'o' || token[i] == 'i' || token[i] == 'k' || token[i] == 'l' || token[i] == 't')
            {
                mode = sign + token[i];
                mode_options.push_back(std::make_pair(mode, ""));
            }
        }
        if (!(iss >> token))
        {
            token.clear();
            break ;
        }
    }
    return true;
}

void ModeHandler::addModeArgument(std::string &token, std::vector<std::pair<std::string, std::string> > &mode_options, std::istringstream &iss)
{
    size_t i = 0;
    while (!token.empty() && i < mode_options.size())
    {
        if (mode_options[i].first == "+o" || mode_options[i].first == "-o" ||
            mode_options[i].first == "+k" || mode_options[i].first == "+l")
        {
            mode_options[i].second = token;
            if (!(iss >> token))
                break ;
        }
        i++;
    }
}

void ModeHandler::executeMode(std::vector<std::pair<std::string, std::string> > &mode_options, std::string channelName, Client *client, Server *server)
{
    std::vector<Client *> users = server->getChannel(channelName)->getUsers();
    
    for (size_t i = 0; i < mode_options.size(); i++)
    {
        std::string first = mode_options[i].first;
        std::string second = mode_options[i].second;
        std::string singleModeChange = "";
        std::vector<std::string> singleModeParams;
        
        if (first == "+o" || first == "-o")
            oMode(first, second, channelName, client, server, singleModeChange, singleModeParams);
        else if (first == "+i" || first == "-i")
            iMode(first, channelName, server, singleModeChange);
        else if (first == "+k" || first == "-k")
            kMode(first, second, channelName, client, server, singleModeChange, singleModeParams);
        else if (first == "+l" || first == "-l")
            lMode(first, second, channelName, client, server, singleModeChange, singleModeParams);
        else if (first == "+t" || first == "-t")
            tMode(first, channelName, server, singleModeChange);
        if (!singleModeChange.empty())
        {
            std::string modeMsg = ":" + client->getNickname() + " MODE " + channelName + " " + first.substr(0,1) + singleModeChange;
            for (size_t j = 0; j < singleModeParams.size(); ++j)
                modeMsg += " " + singleModeParams[j];
            modeMsg += "\r\n";
            
            for (size_t j = 0; j < users.size(); ++j)
                users[j]->sendMessage(modeMsg);
        }
    }
}

void ModeHandler::oMode(std::string mode, std::string arg, std::string channelName, Client *client, Server *server, std::string &modeChanges, std::vector<std::string> &modeParams)
{
	Channel *channel = server->getChannel(channelName);
	if (!channel)
		return ;
	if (arg.empty())
	{
		client->sendMessage(":ircserv 461 MODE :Not enough parameters\r\n");
		return ;
	}
	Client *target = server->getClient(arg);
	if (!target || !channel->isUserInChannel(target))
	{
		client->sendMessage(":ircserv 441 " + arg + " " + channelName + " :They aren't on that channel\r\n");
		return ;
	}
	if (mode == "+o")
	{
		if (!channel->isOperator(target))
		{
			channel->addOperator(target);
			modeChanges += "o";
			modeParams.push_back(target->getNickname());
		}
	}
	else if (mode == "-o")
	{
		if (channel->isOperator(target))
		{
			channel->removeOperator(target);
			modeChanges += "o";
			modeParams.push_back(target->getNickname());
		}
	}
}

void ModeHandler::iMode(std::string mode, std::string channelName, Server *server, std::string &modeChanges)
{
	Channel *channel = server->getChannel(channelName);
	if (!channel)
		return ;
	if (mode == "+i")
	{
		if (!channel->isInviteOnly())
		{
			channel->setInviteOnly(true);
			modeChanges += "i";
		}
	}
	else
	{
		if (channel->isInviteOnly())
		{
			channel->setInviteOnly(false);
			modeChanges += "i";
		}
	}
}

void ModeHandler::kMode(std::string mode, std::string arg, std::string channelName, Client *client, Server *server, std::string &modeChanges, std::vector<std::string> &modeParams)
{
	Channel *channel = server->getChannel(channelName);
	if (!channel)
		return ;
	if (mode == "+k")
	{
		if (arg.empty())
		{
			client->sendMessage(":ircserv 461 MODE :Not enough parameters\r\n");
			return ;
		}
		channel->setKey(arg);
		modeChanges += "k";
		modeParams.push_back(arg);
	}
	else
	{
		if (channel->hasKey())
		{
			channel->removeKey();
			modeChanges += "k";
		}
	}
}

void ModeHandler::lMode(std::string mode, std::string arg, std::string channelName, Client *client, Server *server, std::string &modeChanges, std::vector<std::string> &modeParams)
{
	Channel *channel = server->getChannel(channelName);
	if (!channel)
		return ;
	if (mode == "+l")
	{
		if (arg.empty())
		{
			client->sendMessage(":ircserv 461 MODE :Not enough parameters\r\n");
			return ;
		}
		int limit = atoi(arg.c_str());
		if (limit <= 0)
		{
			client->sendMessage(":ircserv 696 " + client->getNickname() + " " + channelName + " l " + arg + " :Invalid mode parameter\r\n");
			return ;
		}
		channel->setUserLimit(limit);
		modeChanges += "l";
		modeParams.push_back(arg);
	}
	else
	{
		if (channel->getUserLimit() > 0)
		{
			channel->removeUserLimit();
			modeChanges += "l";
		}
	}
}

void ModeHandler::tMode(std::string mode, std::string channelName, Server *server, std::string &modeChanges)
{
	Channel *channel = server->getChannel(channelName);
	if (!channel)
		return ;
	if (mode == "+t")
	{
		if (!channel->isTopicRestricted())
		{
			channel->setTopicRestricted(true);
			modeChanges += "t";
		}
	}
	else
	{
		if (channel->isTopicRestricted())
		{
			channel->setTopicRestricted(false);
			modeChanges += "t";
		}
	}
}