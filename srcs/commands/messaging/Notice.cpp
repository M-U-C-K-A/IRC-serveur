/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Notice.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:40:03 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 02:40:09 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* this fonction will handle the NOTICE command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleNotice(int clientFd, const std::string &line)
{
	size_t idx_after_command = line.find(' ');
	if (idx_after_command == std::string::npos)
		return;

	size_t idx_after_target = line.find(' ', idx_after_command + 1);
	if (idx_after_target == std::string::npos)
		return;

	std::string target = line.substr(idx_after_command + 1, idx_after_target - idx_after_command - 1);
	std::string message = line.substr(idx_after_target + 1);

	if (!message.empty() && message[0] == ':')
		message.erase(0, 1);

	if (target.empty() || message.empty())
		return;

	if (target[0] == '#' || target[0] == '&')
		sendNoticeToChannel(target, message, clientFd);
	else
		sendNoticeToUser(target, message, clientFd);
}

/*
* Send NOTICE to all members of a channel
* @param channel - Channel name
* @param message - Message text
* @param senderFd - Sender file descriptor
* @return void
*/
void Server::sendNoticeToChannel(const std::string &channel, 
                                  const std::string &message, 
                                  int senderFd)
{
	for (std::vector<Channel>::iterator chan = channelList.begin(); 
	     chan != channelList.end(); ++chan)
	{
		if (chan->getName() == channel)
		{
			std::string msg = ":" + this->Users[senderFd].getNickname();
			msg += "!" + this->Users[senderFd].getUsername();
			msg += "@localhost NOTICE " + channel + " :" + message + IRC_CRLF;
			
			const std::vector<int> &members = chan->getMembers();
			for (size_t i = 0; i < members.size(); i++)
			{
				if (members[i] != senderFd)
					send(members[i], msg.c_str(), msg.length(), 0);
			}
			return;
		}
	}
}

/*
* Send NOTICE to a specific user
* @param target - Target nickname
* @param message - Message text
* @param senderFd - Sender file descriptor
* @return void
*/
void Server::sendNoticeToUser(const std::string &target, 
                               const std::string &message, 
                               int senderFd)
{
	for (std::map<int, User>::iterator it = this->Users.begin(); 
	     it != this->Users.end(); ++it)
	{
		if (it->second.getNickname() == target)
		{
			std::string msg = ":" + this->Users[senderFd].getNickname();
			msg += "!" + this->Users[senderFd].getUsername();
			msg += "@localhost NOTICE " + target + " :" + message + IRC_CRLF;
			send(it->first, msg.c_str(), msg.length(), 0);
			return;
		}
	}
}

/*
** ============================================================================
**                         NOTICE COMMAND - RFC 1459
** ============================================================================
**
**       Sender                 Server                 Receiver
**         |                       |                        |
**         |  NOTICE target :msg   |                        |
**         |---------------------> |                        |
**         |                       |                        |
**         |   Similar to PRIVMSG but:                      |
**         |   - NO automatic replies                       |
**         |   - NO error messages                          |
**         |   - Used by servers/bots                       |
**         |                       |                        |
**         |                       | :sender NOTICE target  |
**         |                       |----------------------> |
**         |                       |                        |
**         v                       v                        v
**
**  NOTICE is like PRIVMSG but with these key differences:
**  1. Servers/clients MUST NOT reply to NOTICE
**  2. No error replies are sent for NOTICE
**  3. Used for automated messages to avoid loops
**
**  Format: NOTICE <target> :<message>
**  Example: NOTICE john :This is a notice
**           NOTICE #channel :Server maintenance in 5 minutes
**
** ============================================================================
*/