/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:09 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:29:07 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the MODE command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleMode(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           MODE COMMAND
** ============================================================================
**
**  Format: MODE <channel> <modes> [params]
**
**  Action: Apply/Remove channel modes (+i, +t, +k, +l, +o).
**  Checks: Operator privileges required for most changes.
**
** ============================================================================
*/
