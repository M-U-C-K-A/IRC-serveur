/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Users.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:05:29 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:33:16 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the USERS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleUsers(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           USERS COMMAND
** ============================================================================
**
**  Format: USERS [server]
**
**  Action: Lists users logged into the server host (deprecated).
**  Reply: Often ERR_USERS_DISABLED (446).
**
** ============================================================================
*/
