/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:34:25 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:34:59 by hdelacou         ###   ########.fr       */
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
**                           INVITE COMMAND
** ============================================================================
**
**  Format: INVITE <user> <channel>
**
**  Action: Invites target user to channel (bypasses +i, +l, +k).
**  Checks: Inviter must be on channel. Op privileges if +i.
**  Notify: Sends INVITE message to target user.
**
** ============================================================================
*/
