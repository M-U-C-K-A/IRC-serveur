/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Admin.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 03:59:16 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:00:33 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the ADMIN command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleAdmin(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                        ADMIN COMMAND - RFC 1459
** ============================================================================
**
**                Query Server Admin Info
**                              |
**                              v
**                    +---------------------+
**                    | ADMIN [<server>]    |
**                    +---------------------+
**                              |
**                              v
**                    +---------------------+
**                    | Check target server |
**                    +---------------------+
**                         /          \
**                This server     Other server
**                       |              |
**                       v              v
**            +-------------------+  Forward query
**            | Send admin info:  |  (multi-server)
**            | - RPL_ADMINME     |
**            | - RPL_ADMINLOC1   |
**            | - RPL_ADMINLOC2   |
**            | - RPL_ADMINEMAIL  |
**            +-------------------+
**                       |
**                       v
**                   👤 Admin Info 👤
**
**  Admin Info Replies:
**  RPL_ADMINME (256): Server name
**  RPL_ADMINLOC1 (257): Administrative location
**  RPL_ADMINLOC2 (258): Administrative location details
**  RPL_ADMINEMAIL (259): Administrative contact email
**
**  Format: ADMIN [<server>]
**  Examples:
**  ADMIN
**  ADMIN irc.example.com
**
**  Provides:
**  - Server administrator contact information
**  - Server location details
**  - Email contact for administrative issues
**
** ============================================================================
*/
