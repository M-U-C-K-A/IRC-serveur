/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Away.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:30 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:30:54 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* Parse away message from AWAY command
* @param line - Full command line
* @return Away message or empty string if unset
*/
std::string Server::parseAwayMessage(const std::string &line)
{
	size_t msgStart = line.find(':');
	
	if (msgStart == std::string::npos)
		return "";

	std::string message = line.substr(msgStart + 1);
	
	if (!message.empty() && message[message.length() - 1] == '\r')
		message.erase(message.length() - 1);
	if (!message.empty() && message[message.length() - 1] == '\n')
		message.erase(message.length() - 1);

	return message;
}

/*
* this fonction will handle the AWAY command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleAway(const int &clientFd, const std::string &line)
{
	std::string awayMessage = parseAwayMessage(line);

	if (awayMessage.empty())
	{
		sendRPL_UNAWAY(clientFd);
		std::cout << "User " << this->Users[clientFd].getNickname() 
		          << " is no longer away" << std::endl;
	}
	else
	{
		sendRPL_NOWAWAY(clientFd);
		std::cout << "User " << this->Users[clientFd].getNickname() 
		          << " is now away: " << awayMessage << std::endl;
	}
}

/*
** ============================================================================
**                           AWAY COMMAND
** ============================================================================
**
**  Format: AWAY [message]
**
**  Action: Sets/removes away status.
**  Effect: Auto-replies to PRIVMSG/WHOIS with away message.
**
** ============================================================================
*/