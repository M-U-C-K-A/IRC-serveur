/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Summon.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:04:32 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:04:33 by hdelacou         ###   ########.fr       */
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
**                       SUMMON COMMAND - RFC 1459
** ============================================================================
**
**                Summon User to IRC
**                              |
**                              v
**                    +-------------------------+
**                    | SUMMON <user> [server]  |
**                    +-------------------------+
**                              |
**                              v
**                    +-------------------------+
**                    | Check if user exists    |
**                    | on server system        |
**                    +-------------------------+
**                         /          \
**                   Exists         Doesn't exist
**                       |              |
**                       v              v
**            +-------------------+  ERR_NOSUCHUSER
**            | Check if already  |  (No such user)
**            | on IRC            |
**            +-------------------+
**                   /      \
**          Already on    Not on
**                |          |
**                v          v
**        RPL_SUMMONING  Send notification
**        (already here) to user's terminal
**                          |
**                          v
**                   📢 User Summoned 📢
**
**  SUMMON Purpose:
**  - Notify system users to join IRC
**  - Sends message to user's terminal
**  - Legacy command, rarely implemented
**  - Security/privacy concerns in modern systems
**
**  Reply codes:
**  RPL_SUMMONING (342): User being summoned
**  ERR_NOSUCHUSER (No such user on system)
**  ERR_SUMMON_DISABLED (445): SUMMON has been disabled
**
**  Format: SUMMON <user> [<server>]
**  Examples:
**  SUMMON john
**  SUMMON alice irc.example.com
**
**  NOTE: Often disabled for security reasons
**  Most modern servers return ERR_SUMMON_DISABLED
**
**  Implementation:
**  In most cases, this command should return:
**  ERR_SUMMON_DISABLED (445): SUMMON has been disabled
**
** ============================================================================
*/
