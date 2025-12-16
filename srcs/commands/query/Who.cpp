/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:05:56 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:05:57 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the WHO command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleWho(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                          WHO COMMAND - RFC 1459
** ============================================================================
**
**                Query Users Matching Criteria
**                              |
**                              v
**                    +---------------------------+
**                    | WHO <mask> [o]            |
**                    +---------------------------+
**                              |
**                              v
**                    +---------------------------+
**                    | Parse mask and flags      |
**                    | [o] = operators only      |
**                    +---------------------------+
**                              |
**            +---------+---------+---------+
**            |         |         |         |
**            v         v         v         v
**        Channel    Wildcard   Nick    All users
**         mask       mask      mask     (no mask)
**            |         |         |         |
**            v         v         v         v
**      +------------------------------------+
**      | Filter users matching the criteria |
**      +------------------------------------+
**                       |
**                       v
**            +-------------------+
**            | For each match:   |
**            | RPL_WHOREPLY(352) |
**            | Send user info    |
**            +-------------------+
**                       |
**                       v
**            +-------------------+
**            | RPL_ENDOFWHO(315) |
**            +-------------------+
**                       |
**                       v
**                   🔍 WHO Query 🔍
**
**  WHO Reply Format:
**  RPL_WHOREPLY (352):
**  :server 352 nick <channel> <user> <host> <server> <nick> <flags> :<hopcount>  <real name>
**
**  Flags:
**  H - Here (not away)
**  G - Gone (away)
**  * - IRC operator
**  @ - Channel operator
**  + - Voiced
**
**  Format: WHO [<mask> [o]]
**  Examples:
**  WHO #channel
**  WHO *.fr
**  WHO john
**  WHO * o  (operators only)
**
**  Information provided:
**  - Channel (or * if not in channel context)
**  - Username
**  - Hostname
**  - Server
**  - Nickname
**  - Flags (H/G, *, @, +)
**  - Hopcount
**  - Real name
**
**  Common uses:
**  - List users in a channel
**  - Find users matching wildcard
**  - Locate specific user
**  - Find operators
**
** ============================================================================
*/
