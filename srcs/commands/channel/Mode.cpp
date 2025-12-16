/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:09 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 02:39:10 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the MODE command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleMode(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                             MODE COMMAND
** ============================================================================
**
**      Operator             Server                  Channel
**          |                    |                        |
**          | MODE #chan +itk    |                        |
**          | [password]         |                        |
**          |------------------> |                        |
**          |                    |                        |
**          |                    | Check permissions:     |
**          |                    | - Is user on channel?  |
**          |                    | - Is user operator?    |
**          |                    |                        |
**          |                    | Apply modes:           |
**          |                    | +i : Invite-only       |
**          |                    | +t : Topic restricted  |
**          |                    | +k : Channel password  |
**          |                    | +o : Operator status   |
**          |                    | +l : User limit        |
**          |                    |                        |
**          |  :nick MODE #chan  |                        |
**          | <----------------- |----------------------> |
**          |                    |                        |
**          |                    | Broadcast to all       |
**          |                    |                        |
**          v                    v                        v
**
**  Flow: Operator sets mode -> Permissions checked -> Mode applied -> Broadcast
**
**  Modes:
**  +i : Invite-only channel (need INVITE to join)
**  +t : Topic can only be changed by operators
**  +k : Channel requires password to join
**  +o : Grant/revoke channel operator status
**  +l : Set/remove user limit on channel
**
**  Possible Errors:
**  - ERR_NEEDMOREPARAMS (461): Not enough parameters
**  - ERR_NOSUCHCHANNEL (403): Channel doesn't exist
**  - ERR_CHANOPRIVSNEEDED (482): Not channel operator
**  - ERR_UNKNOWNMODE (472): Unknown mode character
**  - ERR_KEYSET (467): Channel key already set
**
** ============================================================================
*/
