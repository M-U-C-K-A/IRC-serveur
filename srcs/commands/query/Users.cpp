/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Users.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:05:29 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:05:30 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the USERS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleUsers(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                        USERS COMMAND - RFC 1459
** ============================================================================
**
**                Query Logged-in System Users
**                              |
**                              v
**                    +-------------------------+
**                    | USERS [<server>]        |
**                    +-------------------------+
**                              |
**                              v
**                    +-------------------------+
**                    | Check target server     |
**                    +-------------------------+
**                         /          \
**                This server     Other server
**                       |              |
**                       v              v
**            +-------------------+  Forward query
**            | Query system for  |  (multi-server)
**            | logged-in users   |
**            +-------------------+
**                       |
**                       v
**            +-------------------+
**            | For each user:    |
**            | RPL_USERSSTART    |
**            | RPL_USERS (393)   |
**            | RPL_ENDOFUSERS    |
**            +-------------------+
**                       |
**                       v
**                   👥 System Users 👥
**
**  USERS Purpose:
**  - Show users logged into server's host system
**  - Similar to Unix 'who' or 'w' command
**  - Legacy command, rarely implemented
**  - Privacy/security concerns in modern systems
**
**  Reply codes:
**  RPL_USERSSTART (392): Start of users list
**  RPL_USERS (393): :username terminal host
**  RPL_ENDOFUSERS (394): End of users list
**  ERR_USERS_DISABLED (446): USERS has been disabled
**
**  Format: USERS [<server>]
**  Examples:
**  USERS
**  USERS irc.example.com
**
**  Information typically shown:
**  - System username
**  - Terminal/TTY
**  - Hostname/IP
**
**  NOTE: Often disabled for security/privacy reasons
**  Most modern servers return ERR_USERS_DISABLED
**
**  Implementation:
**  In most cases, this command should return:
**  ERR_USERS_DISABLED (446): USERS has been disabled
**
** ============================================================================
*/
