/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trace.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:05:21 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:33:07 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the TRACE command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleTrace(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           TRACE COMMAND
** ============================================================================
**
**  Format: TRACE [server]
**
**  Action: Traces the route to a server.
**  Replies: RPL_TRACE* replies (200-209, 262).
**
** ============================================================================
*/
