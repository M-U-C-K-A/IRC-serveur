/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:20 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:29:23 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the TOPIC command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleTopic(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           TOPIC COMMAND
** ============================================================================
**
**  Format: TOPIC <channel> [topic]
**
**  Action: Queries or sets channel topic.
**  Checks: Topic mode (+t) requires operator privileges to set.
**  Replies: RPL_TOPIC (332) or broadcast topic change.
**
** ============================================================================
*/
