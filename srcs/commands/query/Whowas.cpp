/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Whowas.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:06:10 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:06:11 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the WHOWAS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleWhowas(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                       WHOWAS COMMAND - RFC 1459
** ============================================================================
**
**                Query Nickname History
**                              |
**                              v
**                    +---------------------------+
**                    | WHOWAS <nick> [<count>]   |
**                    +---------------------------+
**                              |
**                              v
**                    +---------------------------+
**                    | Search nickname history   |
**                    | database                  |
**                    +---------------------------+
**                         /          \
**                    Found         Not found
**                       |              |
**                       v              v
**            +-------------------+  ERR_WASNOSUCHNICK
**            | For each entry:   |  (404) No history
**            | RPL_WHOWASUSER    |
**            | (314)             |
**            +-------------------+
**                       |
**                       v
**            +-------------------+
**            | RPL_ENDOFWHOWAS   |
**            | (369)             |
**            +-------------------+
**                       |
**                       v
**                   📜 Nickname History 📜
**
**  WHOWAS Purpose:
**  - Query information about disconnected users
**  - View history of nickname usage
**  - Find when/why user left
**  - Track nickname changes
**
**  Reply format:
**  RPL_WHOWASUSER (314):
**  :server 314 nick <nick> <user> <host> * :<real name>
**
**  RPL_ENDOFWHOWAS (369):
**  :server 369 nick <nick> :End of WHOWAS
**
**  ERR_WASNOSUCHNICK (406):
**  :server 406 nick <nick> :There was no such nickname
**
**  Format: WHOWAS <nickname> [<count> [<server>]]
**  Examples:
**  WHOWAS john
**  WHOWAS alice 5
**  WHOWAS bob 10 irc.example.com
**
**  Information provided:
**  - Previous user information
**  - Username and hostname
**  - Real name
**  - When they were last seen
**
**  History retention:
**  - Server keeps history for limited time
**  - Usually stores last N entries
**  - Configurable per server
**
**  Use cases:
**  - Find information about disconnected users
**  - Track nickname usage patterns
**  - Investigate abuse/problems
**
** ============================================================================
*/
