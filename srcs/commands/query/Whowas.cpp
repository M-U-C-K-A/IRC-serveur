/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Whowas.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:06:10 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:33:24 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the WHOWAS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleWhowas(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           WHOWAS COMMAND
** ============================================================================
**
**  Format: WHOWAS <nick> [count]
**
**  Action: Queries history of used nicknames.
**  Replies: RPL_WHOWASUSER (314), RPL_ENDOFWHOWAS (369).
**
** ============================================================================
*/
