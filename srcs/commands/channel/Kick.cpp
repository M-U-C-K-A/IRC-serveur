/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:05 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:29:27 by hdelacou         ###   ########.fr       */
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
**                           KICK COMMAND
** ============================================================================
**
**  Format: KICK <channel> <user> [comment]
**
**  Action: Forcibly removes a user from channel.
**  Checks: Operator privileges required. Kicker must be on channel.
**  Notify: Broadcasts kick message to channel members.
**
** ============================================================================
*/
