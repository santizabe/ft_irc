#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "headers.hpp"

class	Client;

class Channel
{
	private:
		std::string				_name;
		std::string				_topic;
		Client					*_topicSetter;
		std::vector<Client*>	_users;
		std::set<Client*>		_operators;
		std::set<Client*>		_invited;
		bool					_inviteOnly;
		bool					_topicRestricted;
		std::string				_key;
		bool					_hasKey;
		int						_userLimit;
		bool					_hasUserLimit;

	public:
		Channel(const std::string &name);
		~Channel();

		// Getters
		const std::string &getName() const;
		const std::string &getTopic() const;
		const std::string &getKey() const;
		int getUserLimit() const;
		bool isInviteOnly() const;
		bool isTopicRestricted() const;
		bool hasKey() const;

		// User management
		bool addUser(Client *user, const std::string &key = "");
		void removeUser(Client *user);
		bool isUserInChannel(Client *user) const;
		std::vector<Client *> getUsers() const;

		// Operator management
		void addOperator(Client *user);
		void removeOperator(Client *user);
		bool isOperator(Client *user) const;

		// Invite management
		void inviteUser(Client *user);
		bool isInvited(Client *user) const;
		void removeInvite(Client *user);

		// Topic management
		void setTopic(const std::string &topic, Client *setter);
		void clearTopic();
		Client *getTopicSetter() const;

		// Modes
		void setInviteOnly(bool value);
		void setTopicRestricted(bool value);
		void setKey(const std::string &key);
		void removeKey();
		void setUserLimit(int limit);
		void removeUserLimit();
};

#endif