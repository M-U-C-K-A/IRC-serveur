/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Userhost.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:05:25 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:05:26 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the USERHOST command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleUserhost(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                      USERHOST COMMAND - RFC 1459
** ============================================================================
**
**                Query User Host Information
**                              |
**                              v
**                    +---------------------------+
**                    | USERHOST <nick> [nick...] |
**                    +---------------------------+
**                              |
**                              v
**                    +---------------------------+
**                    | Parse nickname list       |
**                    | (max 5 nicknames)         |
**                    +---------------------------+
**                              |
**                              v
**                    +---------------------------+
**                    | For each nickname:        |
**                    | - Find user               |
**                    | - Get host info           |
**                    | - Check away status       |
**                    | - Check operator status   |
**                    +---------------------------+
**                              |
**                              v
**                    +---------------------------+
**                    | Send RPL_USERHOST (302)   |
**                    | Format: nick[*]=+/-@host  |
**                    +---------------------------+
**                              |
**                              v
**                           🏠 Host Info 🏠
**
**  USERHOST Reply Format:
**  RPL_USERHOST (302): :server 302 nick :nick1[*]=+user@host nick2[*]=...
**
**  Format breakdown:
**  nick[*] - nickname, * if IRC operator
**  = - separator
**  +/-  - + if here, - if away
**  user@host - username and hostname
**
**  Format: USERHOST <nickname> [<nickname> ...]
**  Examples:
**  USERHOST john
**  USERHOST alice bob charlie
**
**  Reply examples:
**  :server 302 me :john*=+user@host alice=-user2@host2
**
**  Information provided:
**  - Username
**  - Hostname
**  - Away status (+/-)
**  - Operator status (*)
**
**  Maximum: 5 nicknames per query
**  Only returns info for found/valid nicknames
**
**  Use Cases:
**  - Get user connection info
**  - Check away status quickly
**  - Verify operator status
**
** ============================================================================
*/
