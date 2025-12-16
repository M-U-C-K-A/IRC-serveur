/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Admin.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 03:59:16 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:31:44 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the ADMIN command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleAdmin(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           ADMIN COMMAND
** ============================================================================
**
**  Format: ADMIN [server]
**
**  Action: Returns administrative info about the server.
**  Replies: RPL_ADMINME, RPL_ADMINLOC1, RPL_ADMINLOC2, RPL_ADMINEMAIL.
**
** ============================================================================
*/
