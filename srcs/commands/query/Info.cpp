/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Info.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 03:59:59 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:31:53 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the INFO command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleInfo(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           INFO COMMAND
** ============================================================================
**
**  Format: INFO [server]
**
**  Action: Returns information describing the server.
**  Replies: RPL_INFO (371), RPL_ENDOFINFO (374).
**
** ============================================================================
*/
