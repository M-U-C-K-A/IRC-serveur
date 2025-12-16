/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Restart.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 03:58:44 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:31:36 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* This function broadcasts the server restart message
* @param operatorFd the operator file descriptor
* @return void
*/
void Server::broadcastServerRestart(const int &operatorFd) {
	std::string message = "ERROR :Closing Link: Server restart by ";
	message += this->Users[operatorFd].getNickname();
	message += "\r\n";

	// Send to all connected users
	for (std::map<int, User>::iterator it = this->Users.begin(); 
	     it != this->Users.end(); ++it) {
		send(it->first, message.c_str(), message.length(), 0);
	}
}

/*
* This function closes all client connections
* @return void
*/
void Server::closeAllConnections() {
	std::cout << "Closing all client connections..." << std::endl;

	for (std::map<int, User>::iterator it = this->Users.begin(); 
	     it != this->Users.end(); ++it) {
		close(it->first);
	}

	// Clear data structures
	this->Users.clear();
	this->channelList.clear();
	this->pollFds.clear();
}

/*
* this fonction will handle the RESTART command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleRestart(const int &clientFd, const std::string &line) {
	(void)line;

	if (!this->Users[clientFd].isOperator()) {
		sendERR_NOPRIVILEGES(clientFd);
		return;
	}

	std::cout << "!!! SERVER RESTART INITIATED BY IRCOP " 
	          << this->Users[clientFd].getNickname() 
	          << " (fd: " << clientFd << ") !!!" << std::endl;

	broadcastServerRestart(clientFd);

	sleep(1);

	closeAllConnections();

	std::cout << "Server shutting down for restart..." << std::endl;

	exit(0);
}

/*
* This function handles graceful restart of the server
* For production, you might want to:
* 1. Set a restart flag
* 2. Stop accepting new connections
* 3. Wait for existing ops to complete
* 4. Then restart
* @param clientFd the client file descriptor
* @return void
*/
void Server::handleRestartGraceful(const int &clientFd) {
	// Check if user is IRC Operator
	if (!this->Users[clientFd].isOperator()) {
		sendERR_NOPRIVILEGES(clientFd);
		return;
	}

	// Set restart flag (to be implemented in Server class)
	// this->restartPending = true;

	std::string response = ":";
	response += SERVER_NAME;
	response += " NOTICE ";
	response += this->Users[clientFd].getNickname();
	response += " :Server will restart after all connections close\r\n";
	send(clientFd, response.c_str(), response.length(), 0);

	std::cout << "Graceful restart scheduled by " 
	          << this->Users[clientFd].getNickname() << std::endl;
}

/*
** ============================================================================
**                           RESTART COMMAND
** ============================================================================
**
**  Format: RESTART
**
**  Action: Restarts the server instance.
**  Warning: Disconnects all clients.
**  Checks: Requires operator privileges.
**
** ============================================================================
*/
