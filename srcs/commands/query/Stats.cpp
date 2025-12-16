/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Stats.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:04:26 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:32:34 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the STATS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleStats(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           STATS COMMAND
** ============================================================================
**
**  Format: STATS [query] [server]
**
**  Action: Query server statistics (uptime, command usage, etc.).
**  Replies: Varying RPL_STATS* (210-249).
**
** ============================================================================
*/
