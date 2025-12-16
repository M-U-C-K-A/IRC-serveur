/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Whois.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:06:01 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:30:07 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* This function sends RPL_WHOISUSER (311) to the client
* @param clientFd the client file descriptor
* @param target the target user
* @return void
*/
void Server::sendRPL_WHOISUSER(const int &clientFd, const User &target) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  311  + nick +  ;
	response += target.getNickname() +  ;
	response += target.getUsername() +  ;
	response += localhost * :;
	response += target.getRealname() + "
";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* This function sends RPL_WHOISCHANNELS (319) to the client
* @param clientFd the client file descriptor
* @param targetFd the target user file descriptor
* @return void
*/
void Server::sendRPL_WHOISCHANNELS(const int &clientFd, int targetFd) {
	std::string nick = this->Users[clientFd].getNickname();
	std::string targetNick = this->Users[targetFd].getNickname();
	
	// Build channel list
	std::string chanList = ;
	for (std::vector<Channel>::iterator chan = channelList.begin(); 
	     chan != channelList.end(); ++chan) {
		if (chan->isMember(targetFd)) {
			if (!chanList.empty())
				chanList +=  ;
			if (chan->isOperator(targetFd))
				chanList += @;
			chanList += chan->getName();
		}
	}
	
	if (!chanList.empty()) {
		std::string response = ":" + std::string(SERVER_NAME);
		response +=  319  + nick +   + targetNick;
		response +=  : + chanList + "
";
		send(clientFd, response.c_str(), response.length(), 0);
	}
}

/*
* This function sends RPL_WHOISSERVER (312) to the client
* @param clientFd the client file descriptor
* @param targetNick the target user's nickname
* @return void
*/
void Server::sendRPL_WHOISSERVER(const int &clientFd, const std::string &targetNick) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  312  + nick +   + targetNick +  ;
	response += SERVER_NAME;
	response +=  :ft_irc server\r\n;
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* This function sends RPL_WHOISOPERATOR (313) to the client
* @param clientFd the client file descriptor
* @param targetNick the target user's nickname
* @return void
*/
void Server::sendRPL_WHOISOPERATOR(const int &clientFd, const std::string &targetNick) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  313  + nick +   + targetNick;
	response +=  :is an IRC operator\r\n;
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* This function sends RPL_ENDOFWHOIS (318) to the client
* @param clientFd the client file descriptor
* @param targetNick the target user's nickname
* @return void
*/
void Server::sendRPL_ENDOFWHOIS(const int &clientFd, const std::string &targetNick) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  318  + nick +   + targetNick;
	response +=  :End of /WHOIS list\r\n;
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* this fonction will handle the WHOIS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleWhois(const int &clientFd, const std::string &line) {
	size_t pos = line.find(' ');
	if (pos == std::string::npos) {
		sendERR_NONICKNAMEGIVEN(clientFd);
		return;
	}
	
	std::string targetNick = line.substr(pos + 1);
	if (!targetNick.empty() && targetNick[targetNick.length() - 1] == '\r')
		targetNick.erase(targetNick.length() - 1);
	if (!targetNick.empty() && targetNick[targetNick.length() - 1] == '\n')
		targetNick.erase(targetNick.length() - 1);
	
	int targetFd = -1;
	for (std::map<int, User>::iterator it = this->Users.begin(); 
	     it != this->Users.end(); ++it) {
		if (it->second.getNickname() == targetNick) {
			targetFd = it->first;
			break;
		}
	}
	
	if (targetFd == -1) {
		sendERR_NOSUCHNICK(clientFd, targetNick);
		return;
	}
	
	sendRPL_WHOISUSER(clientFd, this->Users[targetFd]);
	sendRPL_WHOISCHANNELS(clientFd, targetFd);
	sendRPL_WHOISSERVER(clientFd, targetNick);
	
	if (this->Users[targetFd].isAway()) {
		sendRPL_AWAY(clientFd, targetNick, this->Users[targetFd].getAwayMessage());
	}
	
	if (this->Users[targetFd].isOperator()) {
		sendRPL_WHOISOPERATOR(clientFd, targetNick);
	}
	
	sendRPL_ENDOFWHOIS(clientFd, targetNick);
}

/*
** ============================================================================
**                           WHOIS COMMAND
** ============================================================================
**
**  Format: WHOIS <nickname>
**
**  Action: Returns detailed info about a specific user.
**  Replies: WHOISUSER (311), CHANNELS (319), SERVER (312), OPERATOR (313),
**           AWAY (301), ENDOFWHOIS (318).
**
** ============================================================================
*/
