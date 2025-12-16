/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Oper.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 03:57:53 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 03:57:58 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* This function parses the OPER command
* @param line the raw command line to parse
* @return OperParams struct containing username and password
*/
struct OperParams {
	std::string username;
	std::string password;
};

OperParams Server::parseOperCommand(const std::string &line) {
	OperParams params;
	
	// Find start of parameters (after "OPER ")
	size_t pos = OPER_CMD_LENGTH;
	if (pos >= line.length())
		return params;

	// Skip leading spaces
	while (pos < line.length() && line[pos] == ' ')
		pos++;

	// Extract username (up to next space)
	size_t usernameEnd = line.find(' ', pos);
	if (usernameEnd == std::string::npos)
		return params;
	
	params.username = line.substr(pos, usernameEnd - pos);

	// Extract password (rest of line)
	pos = usernameEnd + 1;
	while (pos < line.length() && line[pos] == ' ')
		pos++;
		
	if (pos < line.length()) {
		params.password = line.substr(pos);
		// Remove trailing CR/LF
		if (!params.password.empty() && params.password[params.password.length() - 1] == '\r')
			params.password.erase(params.password.length() - 1);
		if (!params.password.empty() && params.password[params.password.length() - 1] == '\n')
			params.password.erase(params.password.length() - 1);
	}

	return params;
}

/*
* This function validates the OPER credentials
* TODO: Load from config file
* @param username the username to validate
* @param password the password to validate
* @return true if credentials are valid, false otherwise
*/
bool Server::validateOperCredentials(const std::string &username, 
                                      const std::string &password) {
	// TODO: Load from configuration file
	// For now, hardcoded credentials for testing
	const std::string OPER_USER = "admin";
	const std::string OPER_PASS = "operpass";

	return (username == OPER_USER && password == OPER_PASS);
}

/*
* this fonction will handle the OPER command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleOper(const int &clientFd, const std::string &line) {
	OperParams params = parseOperCommand(line);

	if (params.username.empty() || params.password.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, CMD_OPER);
		return;
	}

	if (!validateOperCredentials(params.username, params.password)) {
		sendERR_PASSWDMISMATCH(clientFd);
		return;
	}

	this->Users[clientFd].setOperator(true);

	sendRPL_YOUREOPER(clientFd);

	std::string modeMsg = ":";
	modeMsg += this->Users[clientFd].getNickname();
	modeMsg += " MODE ";
	modeMsg += this->Users[clientFd].getNickname();
	modeMsg += " :+o\r\n";
	send(clientFd, modeMsg.c_str(), modeMsg.length(), 0);

	std::cout << "User " << this->Users[clientFd].getNickname() 
	          << " (fd: " << clientFd << ") is now an IRC Operator" << std::endl;
}

/*
* Response Functions for OPER command
*/

// RPL_YOUREOPER (381): You are now an IRC operator
void Server::sendRPL_YOUREOPER(const int &clientFd) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":";
	response += SERVER_NAME;
	response += " 381 ";
	response += nick;
	response += " :You are now an IRC operator\r\n";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

// ERR_PASSWDMISMATCH (464): Password incorrect
void Server::sendERR_PASSWDMISMATCH(const int &clientFd) {
	std::string nick = this->Users[clientFd].getNickname();
	if (nick.empty())
		nick = "*";
	
	std::string response = ":";
	response += SERVER_NAME;
	response += " 464 ";
	response += nick;
	response += " :Password incorrect\r\n";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

// ERR_NOOPERHOST (491): No O-lines for your host
void Server::sendERR_NOOPERHOST(const int &clientFd) {
	std::string nick = this->Users[clientFd].getNickname();
	if (nick.empty())
		nick = "*";
	
	std::string response = ":";
	response += SERVER_NAME;
	response += " 491 ";
	response += nick;
	response += " :No O-lines for your host\r\n";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
** ============================================================================
**                         OPER COMMAND - RFC 1459
** ============================================================================
**
**                   IRC Operator Elevation Flow
**                              |
**                              v
**                    +----------------------+
**                    | OPER <user> <pass>   |
**                    +----------------------+
**                              |
**                              v
**                    +----------------------+
**                    | Parse user & password|
**                    +----------------------+
**                              |
**                              v
**                    +----------------------+
**                    | Validate credentials |
**                    | against config       |
**                    +----------------------+
**                         /           \
**                    Valid           Invalid
**                      |                |
**                      v                v
**            +-------------------+  ERR_PASSWDMISMATCH (464)
**            | Set IRCOP mode    |  "Password incorrect"
**            | MODE user +o      |
**            +-------------------+
**                      |
**                      v
**            +-------------------+
**            | RPL_YOUREOPER     |
**            | "You are now an   |
**            |  IRC operator"    |
**            +-------------------+
**                      |
**                      v
**            +-------------------+
**            | Broadcast MODE +o |
**            | to network        |
**            +-------------------+
**                      |
**                      v
**                  ⚡ IRCOP! ⚡
**
**  IRCOP POWERS:
**  - KILL: Force disconnect users
**  - SQUIT: Disconnect servers
**  - CONNECT: Force server connections
**  - REHASH: Reload server config
**  - RESTART: Restart server
**  - WALLOPS: Message all operators
**
**  Format: OPER <username> <password>
**  Example: OPER admin secretpassword
**
** ============================================================================
*/
