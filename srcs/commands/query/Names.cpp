/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:03:33 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:03:35 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* This function parses the NAMES command
* @param line the raw command line to parse
* @return vector of channel names
*/
std::vector<std::string> Server::parseNamesCommand(const std::string &line) {
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
* This function sends RPL_NAMREPLY (353) to the client
* @param clientFd the client file descriptor
* @param channel the channel to send names for
* @return void
*/
void Server::sendRPL_NAMREPLY(const int &clientFd, Channel &channel) {
	std::string nick = this->Users[clientFd].getNickname();
	std::string chanName = channel.getName();
	
	// Build user list with prefixes
	std::string userList = ;
	const std::vector<int> &members = channel.getMembers();
	
	for (size_t i = 0; i < members.size(); i++) {
		if (channel.isOperator(members[i])) {
			userList += @; // Operator prefix
		}
		userList += this->Users[members[i]].getNickname();
		if (i < members.size() - 1)
			userList +=  ;
	}
	
	// Send RPL_NAMREPLY
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  353  + nick +  =  + chanName;
	response +=  : + userList + "
";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* This function sends RPL_ENDOFNAMES (366) to signal end of NAMES list
* @param clientFd the client file descriptor
* @param channel the channel to send end of names for
* @return void
*/
void Server::sendRPL_ENDOFNAMES(const int &clientFd, Channel &channel) {
	std::string nick = this->Users[clientFd].getNickname();
	std::string chanName = channel.getName();
	
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  366  + nick +   + chanName;
	response +=  :End of /NAMES list\r\n;
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* this fonction will handle the NAMES command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleNames(const int &clientFd, const std::string &line) {
	std::vector<std::string> requestedChannels = parseNamesCommand(line);
	
	if (requestedChannels.empty()) {
		for (std::vector<Channel>::iterator chan = channelList.begin(); 
		     chan != channelList.end(); ++chan) {
			sendRPL_NAMREPLY(clientFd, *chan);
			sendRPL_ENDOFNAMES(clientFd, *chan);
		}
	} else {
		for (size_t i = 0; i < requestedChannels.size(); i++) {
			bool found = false;
			for (std::vector<Channel>::iterator chan = channelList.begin(); 
			     chan != channelList.end(); ++chan) {
				if (chan->getName() == requestedChannels[i]) {
					sendRPL_NAMREPLY(clientFd, *chan);
					sendRPL_ENDOFNAMES(clientFd, *chan);
					found = true;
					break;
				}
			}
			if (!found) {
				std::string response = ":" + std::string(SERVER_NAME);
				response +=  366  + this->Users[clientFd].getNickname();
				response +=   + requestedChannels[i];
				response +=  :End of /NAMES list\r\n;
				send(clientFd, response.c_str(), response.length(), 0);
			}
		}
	}
}

/*
** ============================================================================
**                         NAMES COMMAND - RFC 1459
** ============================================================================
**
**                List Users in Channel(s)
**                              |
**                              v
**                    +----------------------+
**                    | NAMES [#channel]     |
**                    +----------------------+
**                              |
**                        /           \
**               Channel given    No channel
**                      |              |
**                      v              v
**            +----------------+  +----------------+
**            | List users in  |  | List all users |
**            | that channel   |  | in all channels|
**            +----------------+  +----------------+
**                      |              |
**                      v              v
**            +--------------------------------+
**            | RPL_NAMREPLY (353) for each   |
**            | channel with user list        |
**            +--------------------------------+
**                              |
**                              v
**            +--------------------------------+
**            | RPL_ENDOFNAMES (366)          |
**            +--------------------------------+
**                              |
**                              v
**                          📋 Listed 📋
**
**  Format of replies:
**  RPL_NAMREPLY: :server 353 nick = #channel :@op +voice user1 user2
**  RPL_ENDOFNAMES: :server 366 nick #channel :End of /NAMES list
**
**  Prefixes:
**  @ = channel operator
**  + = voice (if implemented)
**
**  Format: NAMES [<channel>{,<channel>}]
**  Examples:
**  NAMES #general
**  NAMES
**  NAMES #chan1,#chan2
**
** ============================================================================
*/
