/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:05 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 02:39:06 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the KICK command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleKick(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                             KICK COMMAND
** ============================================================================
**
**      Operator             Server                  Victim
**          |                    |                       |
**          | KICK #chan bob     |                       |
**          | [:reason]          |                       |
**          |------------------> |                       |
**          |                    |                       |
**          |                    | Check permissions:    |
**          |                    | - Is kicker on chan?  |
**          |                    | - Is kicker op?       |
**          |                    | - Is victim on chan?  |
**          |                    |                       |
**          |                    | :kicker KICK #chan bob|
**          |                    |---------------------> |
**          |                    |                       |
**          |                    | Broadcast to all:     |
**          | <----------------- |---------------------> |
**          |                    |                       |
**          |                    | Remove from channel   |
**          |                    |---------------------> | Removed
**          v                    v                       v
**
**  Flow: Operator kicks -> Victim removed -> All members notified
**
**  Possible Errors:
**  - ERR_NEEDMOREPARAMS (461): Not enough parameters
**  - ERR_NOSUCHCHANNEL (403): Channel doesn't exist
**  - ERR_NOTONCHANNEL (442): Kicker not on channel
**  - ERR_CHANOPRIVSNEEDED (482): Not channel operator
**  - ERR_USERNOTINCHANNEL (441): Victim not on channel
**
** ============================================================================
*/
