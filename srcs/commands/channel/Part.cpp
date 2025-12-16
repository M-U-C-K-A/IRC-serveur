/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:17 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:29:05 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the PART command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handlePart(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           PART COMMAND
** ============================================================================
**
**  Format: PART <channel> [:message]
**
**  Action: Removes user from channel.
**  Notify: Broadcasts part message to channel members.
**  Cleanup: Destroys channel if empty.
**
** ============================================================================
*/
