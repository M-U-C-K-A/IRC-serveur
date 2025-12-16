/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   List.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:03:12 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:32:23 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* This function parses the LIST command
* @param line the raw command line to parse
* @return vector of channel names
*/
std::vector<std::string> Server::parseListCommand(const std::string &line) {
	std::vector<std::string> channels;
	
	size_t pos = line.find(' ');
	if (pos == std::string::npos)
		return channels; // No channels specified
	
	std::string channelList = line.substr(pos + 1);
	
	// Remove trailing CR/LF
	if (!channelList.empty() && channelList[channelList.length() - 1] == '\r')
		channelList.erase(channelList.length() - 1);
	if (!channelList.empty() && channelList[channelList.length() - 1] == '\n')
		channelList.erase(channelList.length() - 1);
	
	// Parse comma-separated channels
	std::istringstream iss(channelList);
	std::string chan;
	while (std::getline(iss, chan, ',')) {
		if (!chan.empty())
			channels.push_back(chan);
	}
	
	return channels;
}

/*
* This function sends RPL_LIST (322) for one channel
* @param clientFd the client file descriptor
* @param channel the channel to send info for
* @return void
*/
void Server::sendRPL_LIST(const int &clientFd, Channel &channel) {
	std::string nick = this->Users[clientFd].getNickname();
	std::string chanName = channel.getName();
	
	// Count visible users
	size_t userCount = channel.getMembers().size();
	
	// Get topic
	std::string topic = channel.getTopic();
	if (topic.empty())
		topic = "No topic is set";
	
	// Build response
	std::string response = ":" + std::string(SERVER_NAME);
	response += " 322 " + nick +   + chanName +  ;
	
	// Add user count
	std::ostringstream oss;
	oss << userCount;
	response += oss.str();
	
	response +=  : + topic + "
";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* This function sends RPL_LISTEND (323) to signal end of LIST
* @param clientFd the client file descriptor
* @return void
*/
void Server::sendRPL_LISTEND(const int &clientFd) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  323  + nick;
	response +=  :End of /LIST\r\n;
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* this fonction will handle the LIST command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleList(const int &clientFd, const std::string &line) {
	std::vector<std::string> requestedChannels = parseListCommand(line);
	
	if (requestedChannels.empty()) {
		for (std::vector<Channel>::iterator chan = channelList.begin(); 
		     chan != channelList.end(); ++chan) {
			sendRPL_LIST(clientFd, *chan);
		}
	} else {
		for (size_t i = 0; i < requestedChannels.size(); i++) {
			for (std::vector<Channel>::iterator chan = channelList.begin(); 
			     chan != channelList.end(); ++chan) {
				if (chan->getName() == requestedChannels[i]) {
					sendRPL_LIST(clientFd, *chan);
					break;
				}
			}
		}
	}
	
	sendRPL_LISTEND(clientFd);
}

/*
** ============================================================================
**                           LIST COMMAND
** ============================================================================
**
**  Format: LIST [channel(s)]
**
**  Action: Lists channels and their topics.
**  Replies: RPL_LIST (322), RPL_LISTEND (323).
**
** ============================================================================
*/
