#include "Commands.hpp"
#include "debug.hpp"
#include "ModeHandler.hpp"
#include "Parser.hpp"

static std::string getPrefix(Client* client) {
    return ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname();
}

// Dispatcher for IRC commands
void Commands::execute(const Message& msg, Client* client, Server* server)
{
    std::string cmd = msg.command;
   
    // Skip empty commands or just whitespace
    if (cmd.empty() || cmd.find_first_not_of(" \t\r\n") == std::string::npos) {
        DEBUG_LOG("Ignoring empty command");
        return;
    }
    
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    
    DEBUG_LOG_CMD(cmd, "Received command from " << client->getNickname() << ", params: " << 
        (msg.params.empty() ? "none" : msg.params[0]) << 
        (msg.trailing.empty() ? "" : ", trailing: " + msg.trailing));

    if (cmd == "NICK")
        handleNick(msg, client, server);
    else if (cmd == "USER")
        handleUser(msg, client, server);
    else if (cmd == "JOIN")
        handleJoin(msg, client, server);
	else if (cmd == "PASS")
    	handlePass(msg, client, server);
    else if (cmd == "PART")
        handlePart(msg, client, server);
    else if (cmd == "PRIVMSG")
        handlePrivmsg(msg, client, server);
    else if (cmd == "QUIT")
        handleQuit(msg, client, server);
    else if (cmd == "KICK")
        handleKick(msg, client, server);
    else if (cmd == "INVITE")
        handleInvite(msg, client, server);
    else if (cmd == "TOPIC")
        handleTopic(msg, client, server);
    else if (cmd == "MODE")
        handleMode(msg, client, server);
    else
		client->sendMessage(":ircserv 421 " + client->getNickname() + " " + cmd + " :Unknown command\r\n");
}

// Handler stubs for each command
void Commands::handleNick(const Message& msg, Client* client, Server* server)
{
    if (!client->isAuthenticated() && !server->getPassword().empty()) {
        client->sendMessage(":ircserv 464 * :Password required\r\n");
        return;
    }
    if (msg.params.empty()) {
        client->sendMessage(":ircserv 431 * :No nickname given\r\n");
        return;
    }
    std::string oldNick = client->getNickname();
    std::string newNick = trim(msg.params[0]);

    // Validar nick
    if (!Parser::isValidNickname(newNick)) {
        client->sendMessage(":ircserv 432 * " + newNick + " :Erroneous nickname\r\n");
        return;
    }

    // Comprobar si el nick ya está en uso
    Client* existing = server->getClient(newNick);
    if (existing && existing != client) {
        client->sendMessage(":ircserv 433 * " + newNick + " :Nickname is already in use\r\n");
        return;
    }

    // Notificar cambio de nick si procede
    if (oldNick != "default_nick" && oldNick != "*") {
        std::string nickMsg = ":" + oldNick + "!" + client->getUsername() + "@" + client->getHostname() + " NICK :" + newNick + "\r\n";
        for (std::map<std::string, Channel*>::iterator it = server->getChannels().begin(); it != server->getChannels().end(); ++it) {
            Channel* channel = it->second;
            if (channel->isUserInChannel(client)) {
                std::vector<Client *> users = channel->getUsers();
                for (size_t i = 0; i < users.size(); ++i) {
                    if (users[i] != client)
                        users[i]->sendMessage(nickMsg);
                }
            }
        }
        client->sendMessage(nickMsg);
    }
    client->setNickname(newNick);
    if (client->canRegister())
    {
        client->setRegistered(true);
        client->sendMessage(":ircserv 001 " + client->getNickname() +
            " :Welcome to the IRC server, " + client->getNickname() + "!" +
            client->getUsername() + "@" + client->getHostname() + "\r\n");
    }
}

void Commands::handleUser(const Message& msg, Client* client, Server* server)
{
	if (!client->isAuthenticated() && !server->getPassword().empty()) {
        client->sendMessage(":ircserv 464 * :Password required\r\n");
        return;
    }
    if (msg.params.size() < 3 || msg.trailing.empty()) {
        client->sendMessage(":ircserv 461 USER :Not enough parameters\r\n");
        return;
    }
    if (client->isRegistered()) {
        client->sendMessage(":ircserv 462 " + client->getNickname() + " :You may not reregister\r\n");
        return;
    }
    std::string username = msg.params[0];
    std::string realname = msg.trailing.empty() ? msg.params[3] : msg.trailing;

    client->setUsername(username);
    client->setRealname(realname);
    if (client->canRegister())
	{
        client->setRegistered(true);
        client->sendMessage(":ircserv 001 " + client->getNickname() +
            " :Welcome to the IRC server, " + client->getNickname() + "!" +
            client->getUsername() + "@" + client->getHostname() + "\r\n");
    }
}

void Commands::handleJoin(const Message& msg, Client* client, Server* server)
{
    if (!client->isAuthenticated() && !server->getPassword().empty()) {
        client->sendMessage(":ircserv 464 * :Password required\r\n");
        return;
    }
    if (!client->isRegistered()) {
        client->sendMessage(":ircserv 451 * :You have not registered\r\n");
        return;
    }
    if (msg.params.empty()) {
        client->sendMessage(":ircserv 461 JOIN :Not enough parameters\r\n");
        return;
    }

    // Soporte para varios canales separados por coma
    std::stringstream ss(msg.params[0]);
    std::string channelName;
    std::vector<std::string> keys;
    if (msg.params.size() > 1) {
        std::stringstream keyss(msg.params[1]);
        std::string key;
        while (std::getline(keyss, key, ','))
            keys.push_back(key);
    }

    size_t chanIndex = 0;
    while (std::getline(ss, channelName, ',')) {
        // Validar nombre de canal
        if (!Parser::isValidChannelName(channelName))
		{
			client->sendMessage(":ircserv 476 " + client->getNickname() + " " + channelName + " :Invalid channel name\r\n");
			++chanIndex;
			continue ;
		}

        Channel* channel = server->getChannel(channelName);
        if (!channel)
            channel = server->createChannel(channelName);

        // Si ya está en el canal, error 405
        if (channel->isUserInChannel(client)) {
            client->sendMessage(":ircserv 405 " + channelName + " :You are already on that channel\r\n");
            ++chanIndex;
            continue;
        }

        // Comprobar modos en orden de prioridad
        if (channel->isInviteOnly() && !channel->isInvited(client)) {
            client->sendMessage(":ircserv 473 " + channelName + " :Cannot join channel (+i)\r\n");
            ++chanIndex;
            continue;
        }
        std::string key = (chanIndex < keys.size() ? keys[chanIndex] : "");
        if (channel->hasKey() && key != channel->getKey()) {
            client->sendMessage(":ircserv 475 " + channelName + " :Cannot join channel (+k)\r\n");
            ++chanIndex;
            continue;
        }
        if (channel->getUserLimit() > 0 && (int)channel->getUsers().size() >= channel->getUserLimit()) {
            client->sendMessage(":ircserv 471 " + channelName + " :Cannot join channel (+l)\r\n");
            ++chanIndex;
            continue;
        }

        // Añadir usuario al canal
        channel->addUser(client, key);

        std::vector<Client*> users = channel->getUsers();
        std::string prefix = getPrefix(client);

        // Notificar a todos los usuarios del canal del JOIN (incluido el propio)
        for (size_t i = 0; i < users.size(); ++i) {
            users[i]->sendMessage(prefix + " JOIN :" + channelName + "\r\n");
        }

        // Enviar topic
        if (!channel->getTopic().empty())
            client->sendMessage(":ircserv 332 " + client->getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n");
        else
            client->sendMessage(":ircserv 331 " + client->getNickname() + " " + channelName + " :No topic is set\r\n");

        // Enviar lista de nombres con prefijos
        std::string names;
        for (size_t i = 0; i < users.size(); ++i) {
            if (i > 0) names += " ";
            if (channel->isOperator(users[i]))
                names += "@";
            names += users[i]->getNickname();
        }
        client->sendMessage(":ircserv 353 " + client->getNickname() + " = " + channelName + " :" + names + "\r\n");
        client->sendMessage(":ircserv 366 " + client->getNickname() + " " + channelName + " :End of NAMES list\r\n");

        ++chanIndex;
    }
}


void Commands::handlePass(const Message& msg, Client* client, Server* server)
{
   
    if (msg.params.empty()) {
        client->sendMessage(":ircserv 461 PASS :Not enough parameters\r\n");
        return;
    }
    std::string str = trim(msg.params[0]);
    if (client->isRegistered()) {
        client->sendMessage(":ircserv 462 " + client->getNickname() + " :You may not reregister\r\n");
        return;
    }
    if (str == server->getPassword()) {
        client->setAuthenticated(true);
    } else {
        client->sendMessage(":ircserv 464 * :Password incorrect\r\n");
    }
}

void Commands::handlePart(const Message& msg, Client* client, Server* server)
{
    if (!client->isAuthenticated() && !server->getPassword().empty()) {
        client->sendMessage(":ircserv 464 * :Password required\r\n");
        return;
    }
    if (!client->isRegistered()) {
        client->sendMessage(":ircserv 451 * :You have not registered\r\n");
        return;
    }
    if (msg.params.empty()) {
        client->sendMessage(":ircserv 461 PART :Not enough parameters\r\n");
        return;
    }

    std::stringstream ss(msg.params[0]);
    std::string channelName;
    std::string partMsg = msg.trailing.empty() ? client->getNickname() : msg.trailing;

    while (std::getline(ss, channelName, ',')) {
        Channel* channel = server->getChannel(channelName);
        if (!channel || !channel->isUserInChannel(client)) {
            client->sendMessage(":ircserv 442 " + channelName + " :You're not on that channel\r\n");
            continue;
        }

        std::string prefix = getPrefix(client);
        std::vector<Client*> users = channel->getUsers();
        for (size_t i = 0; i < users.size(); ++i) {
            users[i]->sendMessage(prefix + " PART " + channelName + " :" + partMsg + "\r\n");
        }
        channel->removeUser(client);
    }
}

void Commands::handlePrivmsg(const Message& msg, Client* client, Server* server)
{
	DEBUG_LOG_CMD("PRIVMSG", "Client " << client->getNickname() << " trying to send message");
	
	if (!client->isAuthenticated() && !server->getPassword().empty()) {
        DEBUG_LOG_CLIENT(client, "Not authenticated for PRIVMSG");
       client->sendMessage(":ircserv 464 * :Password required\r\n");
        return;
    }
	if (!client->isRegistered()) {
		DEBUG_LOG_CLIENT(client, "Not registered for PRIVMSG");
		client->sendMessage(":ircserv 451 * :You have not registered\r\n");
		return;
	}
    if (msg.params.empty() || msg.trailing.empty()) {
        DEBUG_LOG_CLIENT(client, "Empty params or trailing for PRIVMSG");
        client->sendMessage(":ircserv 411 " + client->getNickname() + " :No recipient given (PRIVMSG)\r\n");
        return;
    }
    std::string target = msg.params[0];
    std::string text = msg.trailing;
    
    DEBUG_LOG_CMD("PRIVMSG", "Client " << client->getNickname() << " sending to target: " << target << ", message: " << text);

    // Check if target is a channel
    if (!target.empty() && target[0] == '#') {
        DEBUG_LOG_CMD("PRIVMSG", "Target is a channel: " << target);
        
        Channel* channel = server->getChannel(target);
        if (!channel) {
            DEBUG_LOG_CMD("PRIVMSG", "Channel " << target << " not found");
           client->sendMessage(":ircserv 401 " + client->getNickname() + " " + target + " :No such nick/channel\r\n");
            return;
        }
        
        DEBUG_LOG_CHANNEL(channel, "Found channel, checking if user is in channel");
        DEBUG_LOG_CLIENT(client, "Is in channel " << target << "? " << (channel->isUserInChannel(client) ? "YES" : "NO"));
        
        if (!channel->isUserInChannel(client)) {
            DEBUG_LOG_CLIENT(client, "Not in channel " << target);
            client->sendMessage(":ircserv 404 " + target + " :Cannot send to channel\r\n");
            return;
        }
        
        std::vector<Client*> users = channel->getUsers();
        DEBUG_LOG_CHANNEL(channel, "Broadcasting to " << users.size() << " users");
        
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i] != client) {
                DEBUG_LOG_CLIENT(users[i], "Receiving message from " << client->getNickname() << " in channel " << target);
				users[i]->sendMessage(getPrefix(client) + " PRIVMSG " + target + " :" + text + "\r\n");
            }
        }
    }
    else {
        // Target is a user
        Client* dest = server->getClient(target);
        if (!dest) {
            client->sendMessage(":ircserv 401 " + target + " :No such nick/channel\r\n");
            return;
        }
        dest->sendMessage(":" + client->getNickname() + " PRIVMSG " + target + " :" + text + "\r\n");
    }
}

void Commands::handleQuit(const Message& msg, Client* client, Server* server)
{
	if (!client->isAuthenticated() && !server->getPassword().empty()) {
        client->sendMessage(":ircserv 464 * :Password required\r\n");
        return;
    }
    std::string quitMsgText = msg.trailing.empty() ? "Client Quit" : msg.trailing;
    std::string prefix = getPrefix(client);
    std::string quitMsg = prefix + " QUIT :" + quitMsgText + "\r\n";

    // Notificar a todos los usuarios de todos los canales y eliminar al cliente de cada canal
    for (std::map<std::string, Channel*>::iterator it = server->getChannels().begin(); it != server->getChannels().end(); ++it) {
        Channel* channel = it->second;
        if (channel->isUserInChannel(client)) {
            std::vector<Client*> users = channel->getUsers();
            for (size_t i = 0; i < users.size(); ++i) {
                if (users[i] != client)
                    users[i]->sendMessage(quitMsg);
            }
            channel->removeUser(client);
        }
    }
    client->sendMessage("ERROR :Closing Link: " + client->getNickname() + " (" + quitMsgText + ")\r\n");
    server->removeClient(client->getSocketFd());
}

void Commands::handleKick(const Message& msg, Client* client, Server* server)
{
    if (!client->isAuthenticated() && !server->getPassword().empty()) {
        client->sendMessage(":ircserv 464 * :Password required\r\n");
        return;
    }
    if (!client->isRegistered()) {
        client->sendMessage(":ircserv 451 * :You have not registered\r\n");
        return;
    }
    if (msg.params.size() < 2) {
        client->sendMessage(":ircserv 461 KICK :Not enough parameters\r\n");
        return;
    }
    std::string channelName = msg.params[0];
    std::string targetNick = msg.params[1];
    Channel* channel = server->getChannel(channelName);
    if (!channel) {
        client->sendMessage(":ircserv 403 " + channelName + " :No such channel\r\n");
        return;
    }
    if (!channel->isOperator(client)) {
        client->sendMessage(":ircserv 482 " + channelName + " :You're not channel operator\r\n");
        return;
    }
    Client* target = server->getClient(targetNick);
    if (!target || !channel->isUserInChannel(target)) {
        client->sendMessage(":ircserv 441 " + targetNick + " " + channelName + " :They aren't on that channel\r\n");
        return;
    }
    // Motivo: trailing o, si no hay, el tercer parámetro (si existe), o tu nick
    std::string reason;
    if (!msg.trailing.empty())
        reason = msg.trailing;
    else if (msg.params.size() > 2)
        reason = msg.params[2];
    else
        reason = client->getNickname();

    std::string kickMsg = getPrefix(client) + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
    std::vector<Client*> users = channel->getUsers();
    for (size_t i = 0; i < users.size(); ++i)
        users[i]->sendMessage(kickMsg);
    target->sendMessage(kickMsg);
    channel->removeUser(target);
}

void Commands::handleInvite(const Message& msg, Client* client, Server* server)
{
	if (!client->isAuthenticated() && !server->getPassword().empty()) {
        client->sendMessage(":ircserv 464 * :Password required\r\n");
        return;
    }
	if (!client->isRegistered()) {
		client->sendMessage(":ircserv 451 * :You have not registered\r\n");
		return;
	}
    if (msg.params.size() < 2) {
        client->sendMessage(":ircserv 461 INVITE :Not enough parameters\r\n");
        return;
    }
    std::string targetNick = msg.params[0];
    std::string channelName = msg.params[1];
    Channel* channel = server->getChannel(channelName);
    if (!channel) {
        client->sendMessage(":ircserv 403 " + channelName + " :No such channel\r\n");
        return;
    }
    if (!channel->isOperator(client)) {
        client->sendMessage(":ircserv 482 " + channelName + " :You're not channel operator\r\n");
        return;
    }
    Client* target = server->getClient(targetNick);
    if (!target) {
        client->sendMessage(":ircserv 401 " + targetNick + " :No such nick/channel\r\n");
        return;
    }
    if (channel->isUserInChannel(target)) {
        client->sendMessage(":ircserv 443 " + targetNick + " " + channelName + " :is already on channel\r\n");
        return;
    }
    channel->inviteUser(target);
	target->sendMessage(getPrefix(client) + " INVITE " + targetNick + " :" + channelName + "\r\n");
	client->sendMessage(":ircserv 341 " + client->getNickname() + " " + targetNick + " " + channelName + "\r\n");
}

void Commands::handleTopic(const Message& msg, Client* client, Server* server)
{
    if (!client->isAuthenticated() && !server->getPassword().empty()) {
        client->sendMessage(":ircserv 464 * :Password required\r\n");
        return;
    }
    if (!client->isRegistered()) {
        client->sendMessage(":ircserv 451 * :You have not registered\r\n");
        return;
    }
    if (msg.params.empty()) {
        client->sendMessage(":ircserv 461 TOPIC :Not enough parameters\r\n");
        return;
    }
    std::string channelName = msg.params[0];
    Channel* channel = server->getChannel(channelName);
    if (!channel) {
        client->sendMessage(":ircserv 403 " + channelName + " :No such channel\r\n");
        return;
    }
    if (!channel->isUserInChannel(client)) {
        client->sendMessage(":ircserv 442 " + channelName + " :You're not on that channel\r\n");
        return;
    }
    // Set topic
    if (!msg.trailing.empty()) {
        if (channel->isTopicRestricted() && !channel->isOperator(client)) {
            client->sendMessage(":ircserv 482 " + channelName + " :You're not channel operator\r\n");
            return;
        }
        channel->setTopic(msg.trailing, client);
        std::vector<Client*> users = channel->getUsers();
        for (size_t i = 0; i < users.size(); ++i)
            users[i]->sendMessage(getPrefix(client) + " TOPIC " + channelName + " :" + channel->getTopic() + "\r\n");
    }
    // Get topic
    else {
        if (channel->getTopic().empty())
            client->sendMessage(":ircserv 331 " + client->getNickname() + " " + channelName + " :No topic is set\r\n");
        else
            client->sendMessage(":ircserv 332 " + client->getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n");
    }
}

void Commands::handleMode(const Message& msg, Client* client, Server* server)
{
    ModeHandler::handle(msg, client, server);
}
