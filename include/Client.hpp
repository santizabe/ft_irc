/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nporras- <nporras-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 11:37:03 by kmoundir          #+#    #+#             */
/*   Updated: 2025/07/28 15:37:42 by nporras-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "headers.hpp"


class Client {
    private:
        int socket_fd;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _hostname;
        bool _authenticated;
        bool _registered;
        std::string _buffer;
        //std::vector<Channel*> joined_channels;
        time_t _last_ping;
       // FileTransfer* active_transfer;
    public:
        Client(int fd);
		Client(int fd, const std::string& hostname);
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();

        // Getters and Setters
        void setNickname(const std::string& nick);
        void setUsername(const std::string& user);
        void setRealname(const std::string& real);
        void setHostname(const std::string& host);
        const std::string& getNickname() const;
        const std::string& getUsername() const;
        const std::string& getRealname() const;
        const std::string& getHostname() const;
        int getSocketFd() const ;
        void setSocketFd(int fd);
		void setRegistered(bool value);
		void setAuthenticated(bool value);
        
        void sendMessage(const std::string& message);
        void sendNumericReply(int code,const  std::string& message);
        void sendError(const std::string& message);
        bool isAuthenticated() const;
        bool isRegistered() const;
		bool canRegister() const;
        
        //void joinChannel(Channel* channel);
        //void leaveChannel(Channel* channel);
        //void partAllChannels();
        
        std::string getPrefix();
        void appendToBuffer(const std::string& data);
        bool hasCompleteMessage();
        std::string extractMessage();
        void clearBuffer();
};

#endif
