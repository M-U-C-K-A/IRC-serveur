/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:20 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 02:39:21 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the TOPIC command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleTopic(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                            TOPIC COMMAND
** ============================================================================
**
**         User                 Server                  Channel
**          |                      |                         |
**          |  TOPIC #channel      |                         |
**          |  [:new topic]        |                         |
**          |--------------------> |                         |
**          |                      |                         |
**          |                      |  Two cases:             |
**          |                      |  1) Query topic         |
**          |                      |  2) Set topic           |
**          |                      |                         |
**          |  Query (no message): |                         |
**          |  RPL_TOPIC (332) or  |                         |
**          |  RPL_NOTOPIC (331)   |                         |
**          | <------------------- |                         |
**          |                      |                         |
**          |  Set (with message): |                         |
**          |                      | Check permissions:      |
**          |                      | - User on channel?      |
**          |                      | - Mode +t? Need op?     |
**          |                      |                         |
**          |  :nick TOPIC #chan   |                         |
**          | <------------------- |-----------------------> |
**          |                      |                         |
**          |                      | Broadcast to all        |
**          |                      |                         |
**          v                      v                         v
**
**  Flow: User queries/sets topic -> Check permissions -> Broadcast change
**
**  Possible Errors:
**  - ERR_NEEDMOREPARAMS (461): Not enough parameters
**  - ERR_NOSUCHCHANNEL (403): Channel doesn't exist
**  - ERR_NOTONCHANNEL (442): User not on channel
**  - ERR_CHANOPRIVSNEEDED (482): Need op (mode +t)
**
** ============================================================================
*/
