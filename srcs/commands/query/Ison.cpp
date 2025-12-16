/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ison.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:00:06 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:01:23 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the ISON command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleIson(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                         ISON COMMAND - RFC 1459
** ============================================================================
**
**                Check if Users Are Online
**                              |
**                              v
**                    +-------------------------+
**                    | ISON <nick1> [nick2...] |
**                    +-------------------------+
**                              |
**                              v
**                    +-------------------------+
**                    | Parse nickname list     |
**                    +-------------------------+
**                              |
**                              v
**                    +-------------------------+
**                    | Check each nickname     |
**                    | against online users    |
**                    +-------------------------+
**                              |
**                              v
**                    +-------------------------+
**                    | Build list of online    |
**                    | nicknames found         |
**                    +-------------------------+
**                              |
**                              v
**                    +-------------------------+
**                    | Send RPL_ISON (303)     |
**                    | :nick1 nick2...         |
**                    +-------------------------+
**                              |
**                              v
**                          ✓ Online Check ✓
**
**  ISON Purpose:
**  - Efficiently check multiple users' online status
**  - Returns only nicknames that are currently online
**  - Used by clients to monitor friends/contacts
**  - More efficient than multiple WHOIS queries
**
**  Reply format:
**  RPL_ISON (303): :server 303 nick :[nick1] [nick2] [nick3]...
**
**  Format: ISON <nickname> [<nickname> ...]
**  Examples:
**  ISON john alice bob
**  ISON friend1 friend2 friend3
**
**  Returns:
**  Only nicknames that are currently connected
**  Empty response if none are online
**
** ============================================================================
*/
