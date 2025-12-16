/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:34:25 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 03:53:43 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the INVITE command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleInvite(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                            INVITE COMMAND
** ============================================================================
**
**      Inviter              Server                Target User
**          |                    |                       |
**          | INVITE bob #chan   |                       |
**          |------------------> |                       |
**          |                    |                       |
**          |                    | Check permissions:    |
**          |                    | - Is inviter on chan? |
**          |                    | - Is inviter op?      |
**          |                    | - Is target online?   |
**          |                    | - Is target on chan?  |
**          |                    |                       |
**          |  RPL_INVITING(341) |                       |
**          | <----------------- |                       |
**          |                    |                       |
**          |                    | :inviter INVITE bob   |
**          |                    |---------------------> |
**          |                    |                       |
**          |                    |                       | Notified
**          |                    |                       | Can now JOIN
**          v                    v                       v
**
**  Flow: Operator invites -> Target notified -> Added to invite list
**
**  Possible Errors:
**  - ERR_NEEDMOREPARAMS (461): Not enough parameters
**  - ERR_NOSUCHNICK (401): Target user doesn't exist
**  - ERR_NOTONCHANNEL (442): Inviter not on channel
**  - ERR_CHANOPRIVSNEEDED (482): Not channel operator
**  - ERR_USERONCHANNEL (443): Target already on channel
**
** ============================================================================
*/
