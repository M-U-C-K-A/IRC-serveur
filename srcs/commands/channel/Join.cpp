/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:34:35 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 02:38:44 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the JOIN command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleJoin(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}


/*
** ============================================================================
**                              JOIN COMMAND
** ============================================================================
**
**         User                  Server                   Channel
**          |                       |                          |
**          |   JOIN #channel [key] |                          |
**          |---------------------> |                          |
**          |                       |                          |
**          |                       | Check channel exists?    |
**          |                       |                          |
**          |                       |  YES                 NO  |
**          |                       |  |                   |   |
**          |                       |  v                   v   |
**          |                       |                          |
**          |                       | Check permissions:       |
**          |                       | - Invite only? (+i)      |
**          |                       | - Password match? (+k)   |
**          |                       | - User limit? (+l)       |
**          |                       | - Banned? (+b)           |
**          |                       |                          |
**          |   :nick JOIN #chan    |                          |
**          |<--------------------- |------------------------> |
**          |                       |                          |
**          |   RPL_TOPIC (332)     |                          |
**          |<--------------------- |                          |
**          |                       |                          |
**          |   RPL_NAMREPLY (353)  |                          |
**          |<--------------------- |                          |
**          |                       |                          |
**          |   RPL_ENDOFNAMES(366) |                          |
**          |<--------------------- |                          |
**          v                       v                          v
**
**  Flow: User joins -> Channel created/joined -> Names list sent
**
**  Possible Errors:
**  - ERR_NEEDMOREPARAMS (461): Not enough parameters
**  - ERR_BANNEDFROMCHAN (474): Cannot join (banned)
**  - ERR_INVITEONLYCHAN (473): Cannot join (+i)
**  - ERR_BADCHANNELKEY (475): Cannot join (wrong password)
**  - ERR_CHANNELISFULL (471): Cannot join (+l limit reached)
**  - ERR_BADCHANMASK (476): Bad channel name format
**
** ============================================================================
*/