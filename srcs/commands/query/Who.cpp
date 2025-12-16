/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:05:56 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:30:05 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the WHO command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleWho(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           WHO COMMAND
** ============================================================================
**
**  Format: WHO [<mask> [o]]
**
**  Action: List users matching mask (channel, wildcard, nick).
**  Flags: 'o' restricts to operators.
**  Replies: RPL_WHOREPLY (352) -> RPL_ENDOFWHO (315).
**
** ============================================================================
*/
