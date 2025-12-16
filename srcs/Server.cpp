/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:10:00 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:21:42 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                      IRC SERVER - CORE EVENT LOOP
** ============================================================================
**
**  Architecture: epoll() multiplexing + non-blocking I/O
**
**  Flow: initSocket() → initEpoll() → runServer() event loop
**  Events: New connection → acceptUser() | Data ready → parseInput()
**  Commands: Routed via handleLine() to specific handlers
**
** ============================================================================
*/

#include "../includes/Server.hpp"

bool Server::running = true;

/*
 * Server constructor that initialize the server
 * @param port the port number to listen on
 * @param socketfd the socket file descriptor
 * @param password the server password
 * @param epollFd the epoll file descriptor
 * @return void
 */
Server::Server() : port(0), socketfd(-1), password(""), epollFd(-1)
{
	Users = std::map<int, User>();
}

/*
 * Server copy constructor
 * @param src the server to copy
 * @return void
 */
Server::Server(const Server &src)
{
	*this = src;
}

/*
 * Server assignment operator
 * @param src the server to copy
 * @return void
 */
Server Server::operator=(const Server &src)
{
	if (this != &src)
	{
		this->port = src.port;
		this->socketfd = src.socketfd;
		this->password = src.password;
		this->channelList = src.channelList;
		this->epollFd = src.epollFd;
		this->Users = src.Users;
	}
	return *this;
}

/*
 * Server destructor
 * @return void
 */
Server::~Server()
{
	if (epollFd != -1)
		close(epollFd);
	if (socketfd != -1)
		close(socketfd);
}

/*
 * Signal handler for SIGINT and SIGTERM
 * @param signum the signal number
 */
void Server::signalHandler(int signum)
{
	(void)signum;
	running = false;
	std::cout << "\n[IRC] Server shutting down..." << std::endl;
}

/*
 * Initialize socket and set it to non-blocking mode
 * if socket creation fails, throw an exception
 * if setsockopt fails, throw an exception
 * if fcntl fails, throw an exception
 * if bind fails, throw an exception
 * if listen fails, throw an exception
 * @return void
 */
void Server::initSocket()
{
	struct sockaddr_in serverAddr;
	int opt = 1;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
	{
		throw std::runtime_error("Failed to create socket");
	}

	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(socketfd);
		throw std::runtime_error("Failed to set socket options");
	}

	if (fcntl(socketfd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(socketfd);
		throw std::runtime_error("Failed to set non-blocking mode");
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	if (bind(socketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		close(socketfd);
		throw std::runtime_error("Failed to bind socket");
	}

	if (listen(socketfd, MAX_USER) < 0)
	{
		close(socketfd);
		throw std::runtime_error("Failed to listen on socket");
	}

	std::cout << "[IRC] Server listening on port " << port << std::endl;
}

/*
 * Initialize epoll and add socket to it
 * if epoll creation fails, throw an exception
 * if adding socket to epoll fails, throw an exception
 * @return void
 */
void Server::initEpoll()
{
	epollFd = epoll_create1(0);
	if (epollFd < 0)
	{
		throw std::runtime_error("Failed to create epoll instance");
	}

	event.events = EPOLLIN;
	event.data.fd = socketfd;

	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, socketfd, &event) < 0)
	{
		close(epollFd);
		throw std::runtime_error("Failed to add socket to epoll");
	}

	std::cout << "[IRC] Epoll initialized" << std::endl;
}

/*
 * Initialize server
 * @param port the port to listen on
 * @param password the server password
 * @return void
 */
void Server::initServer(const int &port, const std::string &password)
{
	this->port = port;
	this->password = password;

	initSocket();
	initEpoll();

	std::cout << "[IRC] Server initialized successfully" << std::endl;
}

/*
 * Run server main loop
 * @return void
 */
void Server::runServer()
{
	std::cout << "[IRC] Server running. Press Ctrl+C to stop." << std::endl;

	while (running)
	{
		int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, 1000);

		if (numEvents < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			std::cerr << "[IRC] Error in epoll_wait" << std::endl;
			break;
		}

		for (int i = 0; i < numEvents; i++)
		{
			if (events[i].data.fd == socketfd)
			{
				acceptUser();
			}
			else
			{
				parseInput(events[i].data.fd);
			}
		}
	}

	std::cout << "[IRC] Server stopped" << std::endl;
}

/*
 * Accept new user connection
 * @return void
 */
void Server::acceptUser()
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	int clientFd = accept(socketfd, (struct sockaddr *)&clientAddr, &clientLen);
	if (clientFd < 0)
	{
		std::cerr << "[IRC] Failed to accept connection" << std::endl;
		return;
	}

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "[IRC] Failed to set client socket to non-blocking" << std::endl;
		close(clientFd);
		return;
	}

	event.events = EPOLLIN | EPOLLET;
	event.data.fd = clientFd;

	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) < 0)
	{
		std::cerr << "[IRC] Failed to add client to epoll" << std::endl;
		close(clientFd);
		return;
	}

	User newUser;
	Users[clientFd] = newUser;

	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN);

	std::cout << "[IRC] New connection from " << ipStr
			  << ":" << ntohs(clientAddr.sin_port)
			  << " (fd: " << clientFd << ")" << std::endl;
}

/*
 * Parse input from user
 * @param userFd the user file descriptor
 * @return void
 */
void Server::parseInput(int userFd)
{
	char buffer[1024];
	int bytesRead = recv(userFd, buffer, sizeof(buffer) - 1, 0);

	if (bytesRead <= 0)
	{
		if (bytesRead == 0)
		{
			std::cout << "[IRC] Client disconnected (fd: " << userFd << ")" << std::endl;
		}
		else
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				std::cerr << "[IRC] Error reading from client (fd: " << userFd << ")" << std::endl;
			}
		}

		epoll_ctl(epollFd, EPOLL_CTL_DEL, userFd, NULL);
		close(userFd);
		Users.erase(userFd);
		return;
	}

	buffer[bytesRead] = '\0';
	std::string line(buffer);

	handleLine(userFd, line);
}

/*
 * Handle incoming line from user
 * @param clientFd the client file descriptor
 * @param line the line to parse
 * @return void
 */
void Server::handleLine(const int &clientFd, const std::string &line)
{
	std::string buffer = line;
	size_t pos = 0;

	while ((pos = buffer.find("\r\n")) != std::string::npos)
	{
		std::string command = buffer.substr(0, pos);
		buffer.erase(0, pos + 2);

		if (command.empty())
			continue;

		std::cout << "[IRC] Client " << clientFd << ": " << command << std::endl;

		std::string cmdName;
		size_t spacePos = command.find(' ');
		if (spacePos != std::string::npos)
		{
			cmdName = command.substr(0, spacePos);
		}
		else
		{
			cmdName = command;
		}

		for (size_t i = 0; i < cmdName.length(); i++)
		{
			cmdName[i] = toupper(cmdName[i]);
		}

		if (cmdName == "CAP")
		{
			handleCap(clientFd, command);
		}
		else if (cmdName == "PASS")
		{
			handlePass(clientFd, command);
		}
		else if (cmdName == "NICK")
		{
			handleNick(clientFd, command);
		}
		else if (cmdName == "USER")
		{
			handleUsername(clientFd, command);
		}
		else if (cmdName == "PING")
		{
			handlePing(clientFd, command);
		}
		else if (cmdName == "PONG")
		{
			handlePong(clientFd, command);
		}
		else if (cmdName == "JOIN")
		{
			if (!Users[clientFd].getIsRegister())
			{
				sendERR_NOTREGISTERED(clientFd);
				continue;
			}
			handleJoin(clientFd, command);
		}
		else if (cmdName == "PART")
		{
			if (!Users[clientFd].getIsRegister())
			{
				sendERR_NOTREGISTERED(clientFd);
				continue;
			}
			handlePart(clientFd, command);
		}
		else if (cmdName == "PRIVMSG")
		{
			if (!Users[clientFd].getIsRegister())
			{
				sendERR_NOTREGISTERED(clientFd);
				continue;
			}
			handlePrivateMessage(clientFd, command);
		}
		else if (cmdName == "NOTICE")
		{
			if (!Users[clientFd].getIsRegister())
			{
				sendERR_NOTREGISTERED(clientFd);
				continue;
			}
			handleNotice(clientFd, command);
		}
		else if (cmdName == "TOPIC")
		{
			if (!Users[clientFd].getIsRegister())
			{
				sendERR_NOTREGISTERED(clientFd);
				continue;
			}
			handleTopic(clientFd, command);
		}
		else if (cmdName == "KICK")
		{
			if (!Users[clientFd].getIsRegister())
			{
				sendERR_NOTREGISTERED(clientFd);
				continue;
			}
			handleKick(clientFd, command);
		}
		else if (cmdName == "INVITE")
		{
			if (!Users[clientFd].getIsRegister())
			{
				sendERR_NOTREGISTERED(clientFd);
				continue;
			}
			handleInvite(clientFd, command);
		}
		else if (cmdName == "MODE")
		{
			if (!Users[clientFd].getIsRegister())
			{
				sendERR_NOTREGISTERED(clientFd);
				continue;
			}
			handleMode(clientFd, command);
		}
		else if (cmdName == "WHO")
		{
			if (!Users[clientFd].getIsRegister())
			{
				sendERR_NOTREGISTERED(clientFd);
				continue;
			}
			handleWho(clientFd, command);
		}
		else if (cmdName == "WHOIS")
		{
			if (!Users[clientFd].getIsRegister())
			{
				sendERR_NOTREGISTERED(clientFd);
				continue;
			}
			handleWho(clientFd, command);
		}
		else if (cmdName == "QUIT")
		{
			std::cout << "[IRC] Client " << clientFd << " quit" << std::endl;
			epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
			close(clientFd);
			Users.erase(clientFd);
			return;
		}
		else
		{
			if (Users[clientFd].getIsRegister())
			{
				sendERR_UNKNOWNCOMMAND(clientFd, cmdName);
			}
		}
	}
}

/*
 * Handle CAP (Client Capability) negotiation
 * @param clientFd the client file descriptor
 * @param line the command line
 * @return void
 */
void Server::handleCap(const int &clientFd, const std::string &line)
{
	if (line.find("LS") != std::string::npos || line.find("LIST") != std::string::npos)
	{
		std::string response = ":";
		response += SERVER_NAME;
		response += " CAP * LS :\r\n";
		send(clientFd, response.c_str(), response.length(), 0);
	}
	else if (line.find("END") != std::string::npos)
	{
		return;
	}
	else if (line.find("REQ") != std::string::npos)
	{
		std::string response = ":";
		response += SERVER_NAME;
		response += " CAP * NAK :\r\n";
		send(clientFd, response.c_str(), response.length(), 0);
	}
}

/*
 * Check if user is registered and send welcome message if needed
 * @param clientFd the client file descriptor
 * @return void
 */
void Server::checkUserRegistration(const int &clientFd)
{
	User &user = Users[clientFd];
	if (user.getIsRegister() && !user.getWelcomeMessage())
	{
		std::string nick = user.getNickname();
		std::string msg = ":";
		msg += SERVER_NAME;
		msg += " 001 " + nick + " :Welcome to IRC\r\n";
		send(clientFd, msg.c_str(), msg.length(), 0);
		user.hasWelcomeMessage();
	}
}

/*
 * Handle INVITE command
 * @param clientFd the client file descriptor
 * @param line the command line
 * @return void
 */
void Server::handleInvite(const int &clientFd, const std::string &line)
{
	(void)clientFd;
	(void)line;
}

/*
 * Handle MODE command
 * @param clientFd the client file descriptor
 * @param line the command line
 * @return void
 */
void Server::handleMode(const int &clientFd, const std::string &line)
{
	(void)clientFd;
	(void)line;
}

/*
 * Handle WHO command
 * @param clientFd the client file descriptor
 * @param line the command line
 * @return void
 */
void Server::handleWho(const int &clientFd, const std::string &line)
{
	(void)clientFd;
	(void)line;
}

/*
 * Send error message for unknown command
 * @param clientFd the client file descriptor
 * @param command the command that was not recognized
 * @return void
 */
void Server::sendERR_UNKNOWNCOMMAND(const int &clientFd, const std::string &command)
{
	std::string msg = ":";
	msg += SERVER_NAME;
	msg += " 421 * " + command + " :Unknown command\r\n";
	send(clientFd, msg.c_str(), msg.length(), 0);
}
