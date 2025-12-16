/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 03:59:32 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:00:42 by hdelacou         ###   ########.fr       */
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
**                        ERROR COMMAND - RFC 1459
** ============================================================================
**
**                Report Serious Error
**                              |
**                              v
**                    +---------------------+
**                    | ERROR :<message>    |
**                    +---------------------+
**                              |
**                              v
**                    +---------------------+
**                    | Log error message   |
**                    +---------------------+
**                              |
**                              v
**                    +---------------------+
**                    | Close connection    |
**                    | (if from server)    |
**                    +---------------------+
**                              |
**                              v
**                          ⚠️ Error ⚠️
**
**  ERROR Message Purpose:
**  - Sent before terminating a connection
**  - Reports serious/fatal errors
**  - Used by servers to notify clients of disconnect reason
**  - Can indicate link failures, authentication issues, etc.
**
**  Format: ERROR :<error message>
**  Examples:
**  ERROR :Closing Link: Bad password
**  ERROR :Server shutting down
**  ERROR :Banned from server
**
**  Flow:
**  1. Server/Client encounters fatal error
**  2. Send ERROR with reason
**  3. Close connection immediately after
**
**  Common Error Messages:
**  - Closing Link: <reason>
**  - Server going down
**  - Unauthorized connection
**
** ============================================================================
*/
