/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Summon.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:04:32 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:32:38 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the SUMMON command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleSummon(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           SUMMON COMMAND
** ============================================================================
**
**  Format: SUMMON <user> [server]
**
**  Action: Invites a user to IRC (deprecated).
**  Reply: Often ERR_SUMMON_DISABLED (445).
**
** ============================================================================
*/
