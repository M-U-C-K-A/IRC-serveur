/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Notice.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:40:03 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:30:03 by hdelacou         ###   ########.fr       */
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
void Server::handleNotice(const int &clientFd, const std::string &line)
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

	// Send notice - simplified for now
	std::string msg = ":" + this->Users[clientFd].getNickname();
	msg += "!" + this->Users[clientFd].getUsername();
	msg += "@localhost NOTICE " + target + " :" + message + IRC_CRLF;
	
	if (target[0] == '#' || target[0] == '&') {
		// Channel notice - broadcast to channel members
		for (std::vector<Channel>::iterator chan = channelList.begin(); 
		     chan != channelList.end(); ++chan) {
			if (chan->getName() == target) {
				const std::vector<int> &members = chan->getAllMembers();
				for (size_t i = 0; i < members.size(); i++) {
					if (members[i] != clientFd)
						send(members[i], msg.c_str(), msg.length(), 0);
				}
				return;
			}
		}
	} else {
		// User notice
		for (std::map<int, User>::iterator it = this->Users.begin(); 
		     it != this->Users.end(); ++it) {
			if (it->second.getNickname() == target) {
				send(it->first, msg.c_str(), msg.length(), 0);
				return;
			}
		}
	}
}


/*
** ============================================================================
**                           NOTICE COMMAND
** ============================================================================
**
**  Format: NOTICE <target> :<message>
**
**  Action: Same as PRIVMSG but strictly for notifications.
**  Rules: No automatic replies. No error messages returned.
**
** ============================================================================
*/