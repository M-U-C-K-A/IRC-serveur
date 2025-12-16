/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Userhost.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:05:25 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:33:12 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the USERHOST command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleUserhost(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           USERHOST COMMAND
** ============================================================================
**
**  Format: USERHOST <nick> [nick] ...
**
**  Action: Returns user information for up to 5 nicknames.
**  Reply: RPL_USERHOST (302).
**
** ============================================================================
*/
