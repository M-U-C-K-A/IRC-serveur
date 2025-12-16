/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Squit.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 03:52:03 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:31:40 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* This function parses the SQUIT command
* @param line the raw command line to parse
* @return SQuitParams struct containing server name and comment
*/
struct SQuitParams {
	std::string server;
	std::string comment;
};

/*
* This function parses the SQUIT command
* @param line the raw command line to parse
* @return SQuitParams struct containing server name and comment
*/
SQuitParams Server::parseSQuitCommand(const std::string &line) {
	SQuitParams params;
	
	// Find start of parameters (after "SQUIT ")
	size_t pos = SQUIT_CMD_LENGTH;
	if (pos >= line.length())
		return params;

	// Skip leading spaces
	while (pos < line.length() && line[pos] == ' ')
		pos++;

	// Extract server name (up to space or :)
	size_t serverEnd = line.find_first_of(" :", pos);
	if (serverEnd == std::string::npos)
		serverEnd = line.length();
	
	params.server = line.substr(pos, serverEnd - pos);

	// Extract comment (after ':')
	size_t commentStart = line.find(':', pos);
	if (commentStart != std::string::npos) {
		params.comment = line.substr(commentStart + 1);
		// Remove trailing CR/LF
		if (!params.comment.empty() && params.comment[params.comment.length() - 1] == '\r')
			params.comment.erase(params.comment.length() - 1);
		if (!params.comment.empty() && params.comment[params.comment.length() - 1] == '\n')
			params.comment.erase(params.comment.length() - 1);
	}

	if (params.comment.empty())
		params.comment = "No reason given";

	return params;
}

/*
* this fonction will handle the SQUIT command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleSQuit(const int &clientFd, const std::string &line) {
	if (!this->Users[clientFd].isOperator()) {
		sendERR_NOPRIVILEGES(clientFd);
		return;
	}

	SQuitParams params = parseSQuitCommand(line);

	if (params.server.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, CMD_SQUIT);
		return;
	}

	if (params.server != SERVER_NAME) {
		sendERR_NOSUCHSERVER(clientFd, params.server);
		return;
	}

	std::string response = ":";
	response += SERVER_NAME;
	response += " NOTICE ";
	response += this->Users[clientFd].getNickname();
	response += " :";
	response += MSG_ERR_SINGLE_SERVER_SQUIT;
	response += "\r\n";
	send(clientFd, response.c_str(), response.length(), 0);

	std::cout << "IRCOP " << this->Users[clientFd].getNickname() 
	          << " attempted SQUIT " << params.server 
	          << " (reason: " << params.comment << ")" << std::endl;
}

/*
* this fonction will send the ERR_NOSUCHSERVER error
* @param clientFd the client file descriptor
* @param serverName the server name
* @return void
*/
void Server::sendERR_NOSUCHSERVER(const int &clientFd, const std::string &serverName) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":";
	response += SERVER_NAME;
	response += " 402 ";
	response += nick;
	response += " ";
	response += serverName;
	response += " :No such server\r\n";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
** ============================================================================
**                           SQUIT COMMAND
** ============================================================================
**
**  Format: SQUIT <server> <comment>
**
**  Action: Disconnects a linked server.
**  Checks: Requires operator privileges.
**
** ============================================================================
*/
