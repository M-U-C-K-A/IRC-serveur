/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Links.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:03:05 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:32:19 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the LINKS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleLinks(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           LINKS COMMAND
** ============================================================================
**
**  Format: LINKS [remote server] [mask]
**
**  Action: Lists all server links.
**  Replies: RPL_LINKS (364), RPL_ENDOFLINKS (365).
**
** ============================================================================
*/
