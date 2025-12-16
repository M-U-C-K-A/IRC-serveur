/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:04:03 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:30:09 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* This function extracts the ping token from the command
* @param line the raw command line to parse
* @return the ping token or empty string
*/
std::string Server::parsePingToken(const std::string &line)
{
	size_t pos = line.find(' ');
	if (pos == std::string::npos)
		return "";
	
	std::string token = line.substr(pos + 1);
	
	if (!token.empty() && token[0] == ':')
		token.erase(0, 1);
	
	if (!token.empty() && token[token.length() - 1] == '\r')
		token.erase(token.length() - 1);
	if (!token.empty() && token[token.length() - 1] == '\n')
		token.erase(token.length() - 1);
	
	return token;
}

/*
* This function sends PONG reply to the client
* @param clientFd the client file descriptor
* @param token the ping token to echo back
* @return void
*/
void Server::sendPong(const int &clientFd, const std::string &token)
{
	std::string response = ":" + std::string(SERVER_NAME);
	response += " PONG " + std::string(SERVER_NAME);
	response += " :" + token + IRC_CRLF;
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* this fonction will handle the PING command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handlePing(const int &clientFd, const std::string &line)
{
	std::string token = parsePingToken(line);
	
	if (token.empty())
		token = SERVER_NAME;
	
	sendPong(clientFd, token);
	
	std::cout << "PING from " << this->Users[clientFd].getNickname() 
	          << " (token: " << token << ") - PONG sent" << std::endl;
}

/*
* Handle PONG response from client
* @param client*Fd the client file descriptor
* @param line the command line
*/
void Server::handlePong(const int &clientFd, const std::string &line) {
	(void)line;
	std::cout << "[IRC] Received PONG from client " << clientFd << std::endl;
	// PONG received, connection is alive
}


/*
** ============================================================================
**                           PING COMMAND
** ============================================================================
**
**  Format: PING <token>
**
**  Action: Tests connection vitality.
**  Reply: PONG <token> (Must match sent token).
**
** ============================================================================
*/
