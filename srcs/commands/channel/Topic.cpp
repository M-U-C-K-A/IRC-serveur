/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrien <adrien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:20 by hdelacou          #+#    #+#             */
/*   Updated: 2026/01/21 00:51:49 by adrien           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the TOPIC command
* Format: TOPIC <channel> [<topic>]
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleTopic(const int &clientFd, const std::string &line) {
	// Parse: TOPIC <channel> [<topic>]
	size_t spacePos = line.find(' ');
	if (spacePos == std::string::npos) {
		sendERR_NEEDMOREPARAMS(clientFd, "TOPIC");
		return;
	}

	std::string params = line.substr(spacePos + 1);
	std::string channelName;
	std::string newTopic;
	bool settingTopic = false;

	// Check if setting or querying topic
	size_t topicPos = params.find(' ');
	if (topicPos != std::string::npos) {
		channelName = params.substr(0, topicPos);
		newTopic = params.substr(topicPos + 1);
		if (!newTopic.empty() && newTopic[0] == ':')
			newTopic.erase(0, 1);
		settingTopic = true;
	} else {
		channelName = params;
	}

	// Remove trailing \r\n
	size_t endPos = channelName.find_first_of("\r\n");
	if (endPos != std::string::npos)
		channelName = channelName.substr(0, endPos);
	endPos = newTopic.find_first_of("\r\n");
	if (endPos != std::string::npos)
		newTopic = newTopic.substr(0, endPos);

	// Find channel
	for (std::vector<Channel>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
		if (it->getName() == channelName) {
			// Check if user is on channel
			if (!it->isMember(clientFd)) {
				sendERR_NOTONCHANNEL(clientFd, channelName);
				return;
			}

			if (!settingTopic) {
				// Query topic
				if (it->getTopic().empty()) {
					sendRPL_NOTOPIC(clientFd, *it);
				} else {
					sendRPL_TOPIC(clientFd, *it);
				}
				return;
			}

			// Setting topic - check permissions
			if (it->getTopicOpOnly() && !it->isOperator(clientFd)) {
				sendERR_CHANOPRIVSNEEDED(clientFd, channelName);
				return;
			}

			// Set topic
			it->setTopic(clientFd, newTopic, Users[clientFd].getNickname());

			// Broadcast topic change to channel
			std::string topicMsg = ":" + Users[clientFd].getNickname() + "!" +
			                       Users[clientFd].getUsername() + "@localhost TOPIC " +
			                       channelName + " :" + newTopic + IRC_CRLF;
			std::vector<int> members = it->getAllMembers();
			for (size_t i = 0; i < members.size(); i++) {
				send(members[i], topicMsg.c_str(), topicMsg.length(), 0);
			}

			std::cout << "[IRC] Topic of " << channelName << " set to: " << newTopic << std::endl;
			return;
		}
	}

	sendERR_NOSUCHCHANNEL(clientFd, channelName);
}

/*
** ============================================================================
**                           TOPIC COMMAND
** ============================================================================
**
**  Format: TOPIC <channel> [topic]
**
**  Action: Queries or sets channel topic.
**  Checks: Topic mode (+t) requires operator privileges to set.
**  Replies: RPL_TOPIC (332) or broadcast topic change.
**
** ============================================================================
*/
