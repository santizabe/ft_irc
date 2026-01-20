
# FT_IRC

## Overview

FT_IRC is a C++ implementation of an IRC (Internet Relay Chat) server, developed as part of the 42 school curriculum. The project aims to provide a functional IRC server that supports multiple clients, channels, and standard IRC commands, following the specifications outlined in the subject.

## Objectives

- Implement a fully functional IRC server in C++.
- Support multiple simultaneous clients and channels.
- Handle standard IRC commands and protocol.
- Ensure robust error handling and stability.
- Provide a bonus bot for extended functionality.

## Mandatory Features

- TCP server handling multiple clients.
- User authentication and nickname management.
- Channel creation, joining, parting, and topic management.
- Message broadcasting within channels.
- Support for IRC commands: NICK, USER, JOIN, PART, PRIVMSG, NOTICE, KICK, INVITE, TOPIC, MODE, etc.
- Proper error messages and protocol compliance.

## Bonus Features

- IRC bot implementation (`bonus_irc/`).
- Additional commands or features beyond the mandatory requirements.

## Directory Structure

```
FT_IRC/
│
├── src/              # Main server source files
├── include/          # Header files
├── bonus_irc/        # Bonus IRC bot implementation
│   ├── Bot.cpp
│   ├── Bot.hpp
│   ├── main.cpp
│   └── Makefile
└── Makefile          # Main project Makefile
```

## Build Instructions

To build the server:
```sh
make
```
To build the bonus bot:
```sh
cd bonus_irc && make
```

## Usage

To start the IRC server:
```sh
./ircserv <port> <password>
```
- `<port>`: Port number to listen on.
- `<password>`: Server password required for clients to connect.

## Testing

A bandwidth test script is provided:
```sh
python3 bandwidth_test.py
```

## Evaluation

- The server will be tested for protocol compliance, stability, and feature completeness.
- Bonus points for bot implementation and extra features.

## Authors

- kamalmoundir
- noehum7
- Santiago Zapata Bedoya

## License

This project is for educational use only.
