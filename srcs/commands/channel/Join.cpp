/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:34:35 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:29:00 by hdelacou         ###   ########.fr       */
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
**                           JOIN COMMAND
** ============================================================================
**
**  Format: JOIN <channel> [key]
**
**  Action: Add user to channel (create if new).
**  Checks: Invite-only? Key? Limit? Banned?
**  Replies: Topic, Name list, End of names.
**
** ============================================================================
*/