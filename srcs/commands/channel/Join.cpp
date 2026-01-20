/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrien <adrien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:34:35 by hdelacou          #+#    #+#             */
/*   Updated: 2026/01/21 00:51:49 by adrien           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the JOIN command
* Format: JOIN <channel>[,<channel>] [<key>[,<key>]]
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleJoin(const int &clientFd, const std::string &line) {
	// Parse channel name from command
	size_t spacePos = line.find(' ');
	if (spacePos == std::string::npos) {
		sendERR_NEEDMOREPARAMS(clientFd, "JOIN");
		return;
	}

	std::string params = line.substr(spacePos + 1);
	std::string channelName;
	std::string key = "";

	// Extract channel name and optional key
	size_t keyPos = params.find(' ');
	if (keyPos != std::string::npos) {
		channelName = params.substr(0, keyPos);
		key = params.substr(keyPos + 1);
	} else {
		channelName = params;
	}

	// Remove trailing \r\n
	size_t endPos = channelName.find_first_of("\r\n");
	if (endPos != std::string::npos)
		channelName = channelName.substr(0, endPos);
	endPos = key.find_first_of("\r\n");
	if (endPos != std::string::npos)
		key = key.substr(0, endPos);

	// Validate channel name
	if (channelName.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, "JOIN");
		return;
	}

	if (channelName[0] != '#' && channelName[0] != '&') {
		sendERR_BADCHANMASK(clientFd, channelName);
		return;
	}

	// Check if channel exists
	bool found = false;
	for (std::vector<Channel>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
		if (it->getName() == channelName) {
			found = true;

			// Check if already member
			if (it->isMember(clientFd)) {
				return; // Already on channel
			}

			// Check if can join (invite-only, key, limit)
			std::string reason;
			if (!it->canJoin(clientFd, key, reason)) {
				if (it->getInviteOnly())
					sendERR_INVITEONLYCHAN(clientFd, channelName);
				else if (it->getHasKey())
					sendERR_BADCHANNELKEY(clientFd, channelName);
				else if (it->getUserLimit() > 0)
					sendERR_CHANNELISFULL(clientFd, channelName);
				return;
			}

			// Add user to channel
			it->addMember(clientFd);
			it->clearInvite(clientFd); // Remove from invite list if was invited

			// Notify channel members
			std::string joinMsg = ":" + Users[clientFd].getNickname() + "!" +
			                      Users[clientFd].getUsername() + "@localhost JOIN " +
			                      channelName + IRC_CRLF;
			std::vector<int> members = it->getAllMembers();
			for (size_t i = 0; i < members.size(); i++) {
				send(members[i], joinMsg.c_str(), joinMsg.length(), 0);
			}

			// Send topic if exists
			if (!it->getTopic().empty()) {
				sendRPL_TOPIC(clientFd, *it);
			} else {
				sendRPL_NOTOPIC(clientFd, *it);
			}

			// Send names list
			sendRPL_NAMEREPLY(clientFd, *it);
			sendRPL_ENDOFNAMES(clientFd, *it);
			return;
		}
	}

	// Channel doesn't exist - create it
	if (!found) {
		Channel newChannel(channelName, clientFd);
		channelList.push_back(newChannel);

		// Notify user of join
		std::string joinMsg = ":" + Users[clientFd].getNickname() + "!" +
		                      Users[clientFd].getUsername() + "@localhost JOIN " +
		                      channelName + IRC_CRLF;
		send(clientFd, joinMsg.c_str(), joinMsg.length(), 0);

		// Send names list (just the creator)
		Channel &chan = channelList.back();
		sendRPL_NAMEREPLY(clientFd, chan);
		sendRPL_ENDOFNAMES(clientFd, chan);

		std::cout << "[IRC] Channel " << channelName << " created by "
		          << Users[clientFd].getNickname() << std::endl;
	}
}


/*
** ============================================================================
**                           JOIN COMMAND
** ============================================================================
**
**  Format: JOIN <channel> [key]
**
**  Action: Add user to channel (create if new).
**  Checks: Invite-only? Key? Limit? Banned?
**  Replies: Topic, Name list, End of names.
**
** ============================================================================
*/
