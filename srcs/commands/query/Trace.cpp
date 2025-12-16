/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Trace.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:05:21 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:05:22 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the TRACE command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleTrace(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                        TRACE COMMAND - RFC 1459
** ============================================================================
**
**                Trace Route to Server
**                              |
**                              v
**                    +-------------------------+
**                    | TRACE [<server>]        |
**                    +-------------------------+
**                              |
**                              v
**                    +-------------------------+
**                    | Trace path to target    |
**                    +-------------------------+
**                              |
**                              v
**                    +-------------------------+
**                    | For each hop:           |
**                    | Send RPL_TRACE*         |
**                    | - Class                 |
**                    | - Server/User info      |
**                    | - Link info             |
**                    +-------------------------+
**                              |
**                              v
**                    +-------------------------+
**                    | Send RPL_TRACEEND       |
**                    +-------------------------+
**                              |
**                              v
**                        🔍 Route Traced 🔍
**
**  TRACE Reply Types:
**  RPL_TRACELINK (200): Link to another server
**  RPL_TRACECONNECTING (201): Connection being established
**  RPL_TRACEHANDSHAKE (202): Handshake in progress
**  RPL_TRACEUNKNOWN (203): Unknown connection
**  RPL_TRACEOPERATOR (204): IRC Operator
**  RPL_TRACEUSER (205): Regular user
**  RPL_TRACESERVER (206): Server connection
**  RPL_TRACESERVICE (207): Service
**  RPL_TRACENEWTYPE (208): New connection type
**  RPL_TRACECLASS (209): Class information
**  RPL_TRACEEND (262): End of trace
**
**  Format: TRACE [<server>]
**  Examples:
**  TRACE
**  TRACE irc.example.com
**
**  Information Provided:
**  - Path from this server to target
**  - Each server/hop in between
**  - Connection classes
**  - User/operator information
**
**  Use Cases:
**  - Network debugging
**  - Finding routing problems
**  - Network topology analysis
**
**  NOTE: In single-server, shows only local connections
**
** ============================================================================
*/
