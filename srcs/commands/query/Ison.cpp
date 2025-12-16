/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ison.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:00:06 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:31:55 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the ISON command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleIson(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           ISON COMMAND
** ============================================================================
**
**  Format: ISON <nick1> [nick2] ...
**
**  Action: Checks if users are currently online.
**  Replies: RPL_ISON (303) with list of online nicks.
**
** ============================================================================
*/
