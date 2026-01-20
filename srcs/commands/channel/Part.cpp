/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrien <adrien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:17 by hdelacou          #+#    #+#             */
/*   Updated: 2026/01/21 00:51:49 by adrien           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the PART command
* Format: PART <channel>[,<channel>] [<message>]
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handlePart(const int &clientFd, const std::string &line) {
	// Parse channel name from command
	size_t spacePos = line.find(' ');
	if (spacePos == std::string::npos) {
		sendERR_NEEDMOREPARAMS(clientFd, "PART");
		return;
	}

	std::string params = line.substr(spacePos + 1);
	std::string channelName;
	std::string partMessage = "Leaving";

	// Extract channel name and optional message
	size_t msgPos = params.find(' ');
	if (msgPos != std::string::npos) {
		channelName = params.substr(0, msgPos);
		partMessage = params.substr(msgPos + 1);
		if (!partMessage.empty() && partMessage[0] == ':')
			partMessage.erase(0, 1);
	} else {
		channelName = params;
	}

	// Remove trailing \r\n
	size_t endPos = channelName.find_first_of("\r\n");
	if (endPos != std::string::npos)
		channelName = channelName.substr(0, endPos);
	endPos = partMessage.find_first_of("\r\n");
	if (endPos != std::string::npos)
		partMessage = partMessage.substr(0, endPos);

	if (channelName.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, "PART");
		return;
	}

	// Find channel
	for (std::vector<Channel>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
		if (it->getName() == channelName) {
			// Check if user is on channel
			if (!it->isMember(clientFd)) {
				sendERR_NOTONCHANNEL(clientFd, channelName);
				return;
			}

			// Notify channel members before removing
			std::string partMsg = ":" + Users[clientFd].getNickname() + "!" +
			                      Users[clientFd].getUsername() + "@localhost PART " +
			                      channelName + " :" + partMessage + IRC_CRLF;
			std::vector<int> members = it->getAllMembers();
			for (size_t i = 0; i < members.size(); i++) {
				send(members[i], partMsg.c_str(), partMsg.length(), 0);
			}

			// Remove user from channel
			it->removeMember(clientFd);

			// If channel is empty, delete it
			if (it->isEmpty()) {
				std::cout << "[IRC] Channel " << channelName << " deleted (empty)" << std::endl;
				channelList.erase(it);
			}
			return;
		}
	}

	// Channel not found
	sendERR_NOSUCHCHANNEL(clientFd, channelName);
}

/*
** ============================================================================
**                           PART COMMAND
** ============================================================================
**
**  Format: PART <channel> [:message]
**
**  Action: Removes user from channel.
**  Notify: Broadcasts part message to channel members.
**  Cleanup: Destroys channel if empty.
**
** ============================================================================
*/
