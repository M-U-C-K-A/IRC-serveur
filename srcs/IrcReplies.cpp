/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcReplies.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:10:00 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:22:22 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                         IRC NUMERIC REPLIES - RFC 1459
** ============================================================================
**
**  Format: :<server> <code> <nick> [params] :<message>
**
**  ERR_* (4xx-5xx): Errors (NOSUCHNICK, NICKNAMEINUSE, etc.)
**  RPL_* (2xx-3xx): Success replies (AWAY, ENDOFWHOIS, etc.)
**
**  All replies centralized here for consistency and RFC compliance
**
** ============================================================================
*/

#include "../includes/Server.hpp"
#include "../includes/IrcReplies.hpp"
#include <sstream>

/* Send a generic numeric reply to a client */
void Server::sendNumericReply(const int &clientFd, int code,
							  const std::string &params,
							  const std::string &message)
{
	std::string nick = this->Users[clientFd].getNickname();
	if (nick.empty())
		nick = "*";

	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " " << code << " " << nick;
	if (!params.empty())
		oss << " " << params;
	oss << " :" << message << IRC_CRLF;

	std::string response = oss.str();
	send(clientFd, response.c_str(), response.length(), 0);
}

/* ERR_NOSUCHNICK (401): No such nick/channel */
void Server::sendERR_NOSUCHNICK(const int &clientFd, const std::string &nickname)
{
	sendNumericReply(clientFd, ERR_NOSUCHNICK, nickname, MSG_ERR_NOSUCHNICK);
}

/* ERR_NOSUCHSERVER (402): No such server */
void Server::sendERR_NOSUCHSERVER(const int &clientFd, const std::string &server)
{
	sendNumericReply(clientFd, ERR_NOSUCHSERVER, server, MSG_ERR_NOSUCHSERVER);
}

/* ERR_NORECIPIENT (411): No recipient given */
void Server::sendERR_NORECIPIENT(const int &clientFd, const std::string &command)
{
	sendNumericReply(clientFd, ERR_NORECIPIENT, command, MSG_ERR_NORECIPIENT);
}

/* ERR_NOTEXTTOSEND (412): No text to send */
void Server::sendERR_NOTEXTTOSEND(const int &clientFd)
{
	sendNumericReply(clientFd, ERR_NOTEXTTOSEND, "", MSG_ERR_NOTEXTTOSEND);
}

/* ERR_NONICKNAMEGIVEN (431): No nickname given */
void Server::sendERR_NONICKNAMEGIVEN(const int &clientFd)
{
	sendNumericReply(clientFd, ERR_NONICKNAMEGIVEN, "", MSG_ERR_NONICKNAMEGIVEN);
}

/* ERR_ERRONEUSNICKNAME (432): Erroneous nickname */
void Server::sendERR_ERRONEUSNICKNAME(const int &clientFd, const std::string &nick)
{
	sendNumericReply(clientFd, ERR_ERRONEUSNICKNAME, nick, MSG_ERR_ERRONEUSNICKNAME);
}

/* ERR_NICKNAMEINUSE (433): Nickname is already in use */
void Server::sendERR_NICKNAMEINUSE(const int &clientFd, const std::string &nick)
{
	sendNumericReply(clientFd, ERR_NICKNAMEINUSE, nick, MSG_ERR_NICKNAMEINUSE);
}

/* ERR_NOTONCHANNEL (442): You're not on that channel */
void Server::sendERR_NOTONCHANNEL(const int &clientFd, const std::string &channel)
{
	sendNumericReply(clientFd, ERR_NOTONCHANNEL, channel, MSG_ERR_NOTONCHANNEL);
}

/* ERR_NOTREGISTERED (451): You have not registered */
void Server::sendERR_NOTREGISTERED(const int &clientFd)
{
	sendNumericReply(clientFd, ERR_NOTREGISTERED, "", MSG_ERR_NOTREGISTERED);
}

/* ERR_NEEDMOREPARAMS (461): Not enough parameters */
void Server::sendERR_NEEDMOREPARAMS(const int &clientFd, const std::string &command)
{
	sendNumericReply(clientFd, ERR_NEEDMOREPARAMS, command, MSG_ERR_NEEDMOREPARAMS);
}

/* ERR_ALREADYREGISTRED (462): You may not reregister */
void Server::sendERR_ALREADYREGISTRED(const int &clientFd)
{
	sendNumericReply(clientFd, ERR_ALREADYREGISTRED, "", MSG_ERR_ALREADYREGISTRED);
}

/* ERR_PASSWDMISMATCH (464): Password incorrect */
void Server::sendERR_PASSWDMISMATCH(const int &clientFd)
{
	sendNumericReply(clientFd, ERR_PASSWDMISMATCH, "", MSG_ERR_PASSWDMISMATCH);
}

/* ERR_CHANNELISFULL (471): Cannot join channel (+l) */
void Server::sendERR_CHANNELISFULL(const int &clientFd, const std::string &channel)
{
	sendNumericReply(clientFd, ERR_CHANNELISFULL, channel, MSG_ERR_CHANNELISFULL);
}

/* ERR_INVITEONLYCHAN (473): Cannot join channel (+i) */
void Server::sendERR_INVITEONLYCHAN(const int &clientFd, const std::string &channel)
{
	sendNumericReply(clientFd, ERR_INVITEONLYCHAN, channel, MSG_ERR_INVITEONLYCHAN);
}

/* ERR_BANNEDFROMCHAN (474): Cannot join channel (+b) */
void Server::sendERR_BANNEDFROMCHAN(const int &clientFd, const std::string &channel)
{
	sendNumericReply(clientFd, ERR_BANNEDFROMCHAN, channel, MSG_ERR_BANNEDFROMCHAN);
}

/* ERR_BADCHANNELKEY (475): Cannot join channel (+k) */
void Server::sendERR_BADCHANNELKEY(const int &clientFd, const std::string &channel)
{
	sendNumericReply(clientFd, ERR_BADCHANNELKEY, channel, MSG_ERR_BADCHANNELKEY);
}

/* ERR_BADCHANMASK (476): Bad Channel Mask */
void Server::sendERR_BADCHANMASK(const int &clientFd, const std::string &channel)
{
	sendNumericReply(clientFd, ERR_BADCHANMASK, channel, MSG_ERR_BADCHANMASK);
}

/* ERR_NOPRIVILEGES (481): Permission Denied- You're not an IRC operator */
void Server::sendERR_NOPRIVILEGES(const int &clientFd)
{
	sendNumericReply(clientFd, ERR_NOPRIVILEGES, "", MSG_ERR_NOPRIVILEGES);
}

/* ERR_CHANOPRIVSNEEDED (482): You're not channel operator */
void Server::sendERR_CHANOPRIVSNEEDED(const int &clientFd, const std::string &channel)
{
	sendNumericReply(clientFd, ERR_CHANOPRIVSNEEDED, channel, MSG_ERR_CHANOPRIVSNEEDED);
}

/* ERR_CANTKILLSERVER (483): You can't kill a server! */
void Server::sendERR_CANTKILLSERVER(const int &clientFd)
{
	sendNumericReply(clientFd, ERR_CANTKILLSERVER, "", MSG_ERR_CANTKILLSERVER);
}

/* ERR_NOOPERHOST (491): No O-lines for your host */
void Server::sendERR_NOOPERHOST(const int &clientFd)
{
	sendNumericReply(clientFd, ERR_NOOPERHOST, "", MSG_ERR_NOOPERHOST);
}

/* RPL_AWAY (301): User is away */
void Server::sendRPL_AWAY(const int &clientFd, const std::string &nick,
						  const std::string &awayMsg)
{
	sendNumericReply(clientFd, RPL_AWAY, nick, awayMsg);
}

/* RPL_UNAWAY (305): You are no longer marked as being away */
void Server::sendRPL_UNAWAY(const int &clientFd)
{
	sendNumericReply(clientFd, RPL_UNAWAY, "", MSG_RPL_UNAWAY);
}

/* RPL_NOWAWAY (306): You have been marked as being away */
void Server::sendRPL_NOWAWAY(const int &clientFd)
{
	sendNumericReply(clientFd, RPL_NOWAWAY, "", MSG_RPL_NOWAWAY);
}

/* RPL_YOUREOPER (381): You are now an IRC operator */
void Server::sendRPL_YOUREOPER(const int &clientFd)
{
	sendNumericReply(clientFd, RPL_YOUREOPER, "", MSG_RPL_YOUREOPER);
}

/* RPL_REHASHING (382): Rehashing configuration file */
void Server::sendRPL_REHASHING(const int &clientFd)
{
	sendNumericReply(clientFd, RPL_REHASHING, "server.conf", MSG_RPL_REHASHING);
}

/* RPL_ENDOFWHOIS (318): End of WHOIS list */
void Server::sendRPL_ENDOFWHOIS(const int &clientFd, const std::string &nick)
{
	sendNumericReply(clientFd, RPL_ENDOFWHOIS, nick, MSG_RPL_ENDOFWHOIS);
}

/* RPL_LISTEND (323): End of LIST */
void Server::sendRPL_LISTEND(const int &clientFd)
{
	sendNumericReply(clientFd, RPL_LISTEND, "", MSG_RPL_LISTEND);
}

/* RPL_ENDOFNAMES (366): End of NAMES list */
void Server::sendRPL_ENDOFNAMES(const int &clientFd, Channel &channel)
{
	sendNumericReply(clientFd, RPL_ENDOFNAMES, channel.getName(), MSG_RPL_ENDOFNAMES);
}

/* Send an ERROR message to client (used before disconnection) */
void Server::sendError(const int &clientFd, const std::string &message)
{
	std::string response = "ERROR :" + message + IRC_CRLF;
	send(clientFd, response.c_str(), response.length(), 0);
}

/* ERR_NOSUCHCHANNEL (403): No such channel */
void Server::sendERR_NOSUCHCHANNEL(const int &clientFd, const std::string &channel)
{
	sendNumericReply(clientFd, 403, channel, "No such channel");
}
