/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:17 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 02:39:17 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the PART command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handlePart(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                             PART COMMAND
** ============================================================================
**
**         User                 Server                  Channel
**          |                      |                         |
**          | PART #channel        |                         |
**          | [:goodbye message]   |                         |
**          |--------------------> |                         |
**          |                      |                         |
**          |                      | Check user on channel?  |
**          |                      |                         |
**          |  :nick PART #chan    |                         |
**          | <-------------------- |-----------------------> |
**          |                      |                         |
**          |                      | Broadcast to all members|
**          |                      |                         |
**          |                      | Remove from channel     |
**          |                      |-----------------------> |
**          v                      v                         v
**
**  Flow: User parts -> Removed from channel -> All members notified
**
**  Possible Errors:
**  - ERR_NEEDMOREPARAMS (461): Not enough parameters
**  - ERR_NOSUCHCHANNEL (403): Channel doesn't exist
**  - ERR_NOTONCHANNEL (442): User not on channel
**
** ============================================================================
*/
