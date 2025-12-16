/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 03:59:32 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:31:48 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the ERROR command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleError(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           ERROR COMMAND
** ============================================================================
**
**  Format: ERROR <message>
**
**  Action: Reports a serious error and closes connection.
**  Usage: Sent by server before disconnecting a client permanently.
**
** ============================================================================
*/
