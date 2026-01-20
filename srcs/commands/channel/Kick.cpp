/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrien <adrien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:05 by hdelacou          #+#    #+#             */
/*   Updated: 2026/01/21 00:51:49 by adrien           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the KICK command
* Format: KICK <channel> <user> [<comment>]
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleKick(const int &clientFd, const std::string &line) {
	// Parse: KICK <channel> <user> [<comment>]
	size_t spacePos = line.find(' ');
	if (spacePos == std::string::npos) {
		sendERR_NEEDMOREPARAMS(clientFd, "KICK");
		return;
	}

	std::string params = line.substr(spacePos + 1);

	// Get channel name
	size_t chanEnd = params.find(' ');
	if (chanEnd == std::string::npos) {
		sendERR_NEEDMOREPARAMS(clientFd, "KICK");
		return;
	}
	std::string channelName = params.substr(0, chanEnd);
	params = params.substr(chanEnd + 1);

	// Get target user
	size_t userEnd = params.find(' ');
	std::string targetNick;
	std::string comment = "Kicked";

	if (userEnd != std::string::npos) {
		targetNick = params.substr(0, userEnd);
		comment = params.substr(userEnd + 1);
		if (!comment.empty() && comment[0] == ':')
			comment.erase(0, 1);
	} else {
		targetNick = params;
	}

	// Remove trailing \r\n
	size_t endPos = targetNick.find_first_of("\r\n");
	if (endPos != std::string::npos)
		targetNick = targetNick.substr(0, endPos);
	endPos = comment.find_first_of("\r\n");
	if (endPos != std::string::npos)
		comment = comment.substr(0, endPos);

	// Find channel
	for (std::vector<Channel>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
		if (it->getName() == channelName) {
			// Check if kicker is on channel
			if (!it->isMember(clientFd)) {
				sendERR_NOTONCHANNEL(clientFd, channelName);
				return;
			}

			// Check if kicker is operator
			if (!it->isOperator(clientFd)) {
				sendERR_CHANOPRIVSNEEDED(clientFd, channelName);
				return;
			}

			// Find target user fd
			int targetFd = -1;
			for (std::map<int, User>::iterator uit = Users.begin(); uit != Users.end(); ++uit) {
				if (uit->second.getNickname() == targetNick) {
					targetFd = uit->first;
					break;
				}
			}

			if (targetFd == -1) {
				sendERR_NOSUCHNICK(clientFd, targetNick);
				return;
			}

			// Check if target is on channel
			if (!it->isMember(targetFd)) {
				sendERR_USERNOTINCHANNEL(clientFd, targetNick, channelName);
				return;
			}

			// Broadcast KICK message to channel
			std::string kickMsg = ":" + Users[clientFd].getNickname() + "!" +
			                      Users[clientFd].getUsername() + "@localhost KICK " +
			                      channelName + " " + targetNick + " :" + comment + IRC_CRLF;
			std::vector<int> members = it->getAllMembers();
			for (size_t i = 0; i < members.size(); i++) {
				send(members[i], kickMsg.c_str(), kickMsg.length(), 0);
			}

			// Remove target from channel
			it->removeMember(targetFd);

			// If channel is empty, delete it
			if (it->isEmpty()) {
				channelList.erase(it);
			}
			return;
		}
	}

	sendERR_NOSUCHCHANNEL(clientFd, channelName);
}

/*
** ============================================================================
**                           KICK COMMAND
** ============================================================================
**
**  Format: KICK <channel> <user> [comment]
**
**  Action: Forcibly removes a user from channel.
**  Checks: Operator privileges required. Kicker must be on channel.
**  Notify: Broadcasts kick message to channel members.
**
** ============================================================================
*/
