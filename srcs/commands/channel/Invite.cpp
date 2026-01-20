/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrien <adrien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:34:25 by hdelacou          #+#    #+#             */
/*   Updated: 2026/01/21 00:51:49 by adrien           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the INVITE command
* Format: INVITE <nickname> <channel>
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleInvite(const int &clientFd, const std::string &line) {
	// Parse: INVITE <nickname> <channel>
	size_t spacePos = line.find(' ');
	if (spacePos == std::string::npos) {
		sendERR_NEEDMOREPARAMS(clientFd, "INVITE");
		return;
	}

	std::string params = line.substr(spacePos + 1);

	// Get target nickname
	size_t nickEnd = params.find(' ');
	if (nickEnd == std::string::npos) {
		sendERR_NEEDMOREPARAMS(clientFd, "INVITE");
		return;
	}
	std::string targetNick = params.substr(0, nickEnd);
	std::string channelName = params.substr(nickEnd + 1);

	// Remove trailing \r\n
	size_t endPos = channelName.find_first_of("\r\n");
	if (endPos != std::string::npos)
		channelName = channelName.substr(0, endPos);

	// Find target user
	int targetFd = -1;
	for (std::map<int, User>::iterator it = Users.begin(); it != Users.end(); ++it) {
		if (it->second.getNickname() == targetNick) {
			targetFd = it->first;
			break;
		}
	}

	if (targetFd == -1) {
		sendERR_NOSUCHNICK(clientFd, targetNick);
		return;
	}

	// Find channel
	for (std::vector<Channel>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
		if (it->getName() == channelName) {
			// Check if inviter is on channel
			if (!it->isMember(clientFd)) {
				sendERR_NOTONCHANNEL(clientFd, channelName);
				return;
			}

			// Check if target is already on channel
			if (it->isMember(targetFd)) {
				sendERR_USERONCHANNEL(clientFd, targetNick, channelName);
				return;
			}

			// If channel is invite-only, check if inviter is operator
			if (it->getInviteOnly() && !it->isOperator(clientFd)) {
				sendERR_CHANOPRIVSNEEDED(clientFd, channelName);
				return;
			}

			// Add target to invite list
			it->invite(targetFd);

			// Send RPL_INVITING to inviter
			std::string invitingMsg = ":" + std::string(SERVER_NAME) + " 341 " +
			                          Users[clientFd].getNickname() + " " +
			                          targetNick + " " + channelName + IRC_CRLF;
			send(clientFd, invitingMsg.c_str(), invitingMsg.length(), 0);

			// Send INVITE to target
			std::string inviteMsg = ":" + Users[clientFd].getNickname() + "!" +
			                        Users[clientFd].getUsername() + "@localhost INVITE " +
			                        targetNick + " " + channelName + IRC_CRLF;
			send(targetFd, inviteMsg.c_str(), inviteMsg.length(), 0);

			std::cout << "[IRC] " << Users[clientFd].getNickname() << " invited "
			          << targetNick << " to " << channelName << std::endl;
			return;
		}
	}

	sendERR_NOSUCHCHANNEL(clientFd, channelName);
}

/*
** ============================================================================
**                           INVITE COMMAND
** ============================================================================
**
**  Format: INVITE <user> <channel>
**
**  Action: Invites target user to channel (bypasses +i, +l, +k).
**  Checks: Inviter must be on channel. Op privileges if +i.
**  Notify: Sends INVITE message to target user.
**
** ============================================================================
*/
