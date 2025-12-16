/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kill.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 03:57:42 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 03:58:00 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* This function parses the KILL command
* @param line the raw command line to parse
* @return KillParams struct containing target nickname and reason
*/
struct KillParams {
	std::string target;
	std::string reason;
};

KillParams Server::parseKillCommand(const std::string &line) {
	KillParams params;
	
	// Find start of parameters (after "KILL ")
	size_t pos = KILL_CMD_LENGTH;
	if (pos >= line.length())
		return params;

	// Skip leading spaces
	while (pos < line.length() && line[pos] == ' ')
		pos++;

	// Extract target nickname (up to next space or :)
	size_t targetEnd = line.find_first_of(" :", pos);
	if (targetEnd == std::string::npos)
		targetEnd = line.length();
	
	params.target = line.substr(pos, targetEnd - pos);

	// Extract reason (after ':')
	size_t reasonStart = line.find(':', pos);
	if (reasonStart != std::string::npos) {
		params.reason = line.substr(reasonStart + 1);
		// Remove trailing CR/LF
		if (!params.reason.empty() && params.reason[params.reason.length() - 1] == '\r')
			params.reason.erase(params.reason.length() - 1);
		if (!params.reason.empty() && params.reason[params.reason.length() - 1] == '\n')
			params.reason.erase(params.reason.length() - 1);
	}

	if (params.reason.empty())
		params.reason = "No reason given";

	return params;
}

/*
* This function finds a user by their nickname
* @param nickname the nickname to search for
* @return file descriptor of the user, or -1 if not found
*/
int Server::findUserByNickname(const std::string &nickname) {
	for (std::map<int, User>::iterator it = this->Users.begin(); 
	     it != this->Users.end(); ++it) {
		if (strcasecmp(it->second.getNickname().c_str(), nickname.c_str()) == 0)
			return it->first;
	}
	return -1;
}

/*
* this fonction will broadcast the KILL command
* @param operatorFd the operator file descriptor
* @param victimFd the victim file descriptor
* @param reason the reason for the kill
* @return void
*/
void Server::broadcastKill(const int &operatorFd, const int &victimFd, 
                           const std::string &reason) {
	std::string message = ":";
	message += this->Users[operatorFd].getNickname();
	message += "!" + this->Users[operatorFd].getUsername();
	message += "@localhost KILL ";
	message += this->Users[victimFd].getNickname();
	message += " :";
	message += reason + "\r\n";

	// Track who we've notified
	std::set<int> notified;
	notified.insert(operatorFd);
	notified.insert(victimFd);

	// Send to all users in shared channels with victim
	for (std::vector<Channel>::iterator chan = channelList.begin(); 
	     chan != channelList.end(); ++chan) {
		if (chan->isMember(victimFd)) {
			const std::vector<int> &members = chan->getMembers();
			for (size_t i = 0; i < members.size(); i++) {
				if (notified.find(members[i]) == notified.end()) {
					send(members[i], message.c_str(), message.length(), 0);
					notified.insert(members[i]);
				}
			}
		}
	}
}

/*
* this fonction will handle the KILL command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleKill(const int &clientFd, const std::string &line) {
	if (!this->Users[clientFd].isOperator()) {
		sendERR_NOPRIVILEGES(clientFd);
		return;
	}

	KillParams params = parseKillCommand(line);

	if (params.target.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, CMD_KILL);
		return;
	}

	if (params.target.find('.') != std::string::npos) {
		sendERR_CANTKILLSERVER(clientFd);
		return;
	}

	int targetFd = findUserByNickname(params.target);
	if (targetFd == -1) {
		sendERR_NOSUCHNICK(clientFd, params.target);
		return;
	}

	std::cout << "IRCOP " << this->Users[clientFd].getNickname() 
	          << " killed " << params.target 
	          << " (reason: " << params.reason << ")" << std::endl;

	broadcastKill(clientFd, targetFd, params.reason);

	std::string errorMsg = "ERROR :Closing Link: localhost (Killed (";
	errorMsg += this->Users[clientFd].getNickname();
	errorMsg += " (" + params.reason + ")))\r\n";
	send(targetFd, errorMsg.c_str(), errorMsg.length(), 0);

	removeFromAllChannels(targetFd);
	close(targetFd);
	removeFromPollFds(targetFd);
	this->Users.erase(targetFd);
}

	std::cout << "User " << params.target << " (fd: " << targetFd 
	          << ") has been killed" << std::endl;
}

/*
* Error Response Functions for KILL command
*/

// ERR_NOPRIVILEGES (481): Permission Denied - You're not an IRC operator
void Server::sendERR_NOPRIVILEGES(const int &clientFd) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":";
	response += SERVER_NAME;
	response += " 481 ";
	response += nick;
	response += " :Permission Denied- You're not an IRC operator\r\n";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

// ERR_CANTKILLSERVER (483): You can't kill a server!
void Server::sendERR_CANTKILLSERVER(const int &clientFd) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":";
	response += SERVER_NAME;
	response += " 483 ";
	response += nick;
	response += " :You can't kill a server!\r\n";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
** ============================================================================
**                         KILL COMMAND - RFC 1459
** ============================================================================
**
**                 Force Disconnect User Flow
**                              |
**                              v
**                    +------------------------+
**                    | KILL <nick> :<reason>  |
**                    +------------------------+
**                              |
**                              v
**                    +------------------------+
**                    | Check if user is IRCOP |
**                    +------------------------+
**                         /           \
**                      YES              NO
**                       |                |
**                       v                v
**              +------------------+  ERR_NOPRIVILEGES (481)
**              | Find target user |  "Permission Denied"
**              +------------------+
**                       |
**                  /          \
**              Found         Not Found
**                |              |
**                v              v
**       +----------------+  ERR_NOSUCHNICK (401)
**       | Check if server|  "No such nick"
**       +----------------+
**                |
**           /         \
**       User        Server
**         |            |
**         v            v
**  +----------+  ERR_CANTKILLSERVER (483)
**  | Broadcast|  "You can't kill a server!"
**  | KILL msg |
**  +----------+
**         |
**         v
**  +----------+
**  | Send ERROR|
**  | to victim |
**  +----------+
**         |
**         v
**  +----------+
**  | Close conn|
**  | Remove user|
**  +----------+
**         |
**         v
**      💀 KILLED 💀
**
**  Format: KILL <nickname> :<reason>
**  Example: KILL BadUser :Flooding the network
**
**  KILL is broadcasted as:
**  :operator KILL victim :reason
**
** ============================================================================
*/
