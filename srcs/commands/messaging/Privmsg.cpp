/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrien <adrien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:41:16 by hdelacou          #+#    #+#             */
/*   Updated: 2026/01/21 00:51:49 by adrien           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* this fonction will handle the PRIVMSG command
* Format: PRIVMSG <target> :<message>
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handlePrivateMessage(int clientFd, const std::string &line) {
	// Parse target and message
	size_t spacePos = line.find(' ');
	if (spacePos == std::string::npos) {
		sendERR_NORECIPIENT(clientFd, "PRIVMSG");
		return;
	}

	std::string params = line.substr(spacePos + 1);
	size_t msgPos = params.find(' ');
	if (msgPos == std::string::npos) {
		sendERR_NOTEXTTOSEND(clientFd);
		return;
	}

	std::string target = params.substr(0, msgPos);
	std::string message = params.substr(msgPos + 1);

	// Remove leading ':' from message
	if (!message.empty() && message[0] == ':')
		message.erase(0, 1);

	// Remove trailing \r\n
	size_t endPos = message.find_first_of("\r\n");
	if (endPos != std::string::npos)
		message = message.substr(0, endPos);

	if (target.empty()) {
		sendERR_NORECIPIENT(clientFd, "PRIVMSG");
		return;
	}

	if (message.empty()) {
		sendERR_NOTEXTTOSEND(clientFd);
		return;
	}

	// Build message prefix
	std::string prefix = ":" + Users[clientFd].getNickname() + "!" +
	                     Users[clientFd].getUsername() + "@localhost";

	// Check if target is a channel
	if (target[0] == '#' || target[0] == '&') {
		// Find channel
		for (std::vector<Channel>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
			if (it->getName() == target) {
				// Check if user is on channel
				if (!it->isMember(clientFd)) {
					sendERR_CANNOTSENDTOCHAN(clientFd, target);
					return;
				}

				// Broadcast to all channel members except sender
				std::string fullMsg = prefix + " PRIVMSG " + target + " :" + message + IRC_CRLF;
				std::vector<int> members = it->getAllMembers();
				for (size_t i = 0; i < members.size(); i++) {
					if (members[i] != clientFd) {
						send(members[i], fullMsg.c_str(), fullMsg.length(), 0);
					}
				}
				return;
			}
		}
		sendERR_NOSUCHCHANNEL(clientFd, target);
		return;
	}

	// Target is a user - find by nickname
	for (std::map<int, User>::iterator it = Users.begin(); it != Users.end(); ++it) {
		if (it->second.getNickname() == target) {
			std::string fullMsg = prefix + " PRIVMSG " + target + " :" + message + IRC_CRLF;
			send(it->first, fullMsg.c_str(), fullMsg.length(), 0);
			return;
		}
	}

	sendERR_NOSUCHNICK(clientFd, target);
}

/*
** ============================================================================
**                           PRIVMSG COMMAND
** ============================================================================
**
**  Format: PRIVMSG <target> :<message>
**
**  Action: Sends message to target (User or Channel).
**  Routing: Channel -> Broadcast to members. User -> Direct message.
**
** ============================================================================
*/
