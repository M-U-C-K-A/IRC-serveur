/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Links.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:03:05 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:03:07 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the LINKS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleLinks(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                        LINKS COMMAND - RFC 1459
** ============================================================================
**
**                Query Server Network Topology
**                              |
**                              v
**                    +---------------------------+
**                    | LINKS [[remote] <mask>]   |
**                    +---------------------------+
**                              |
**                              v
**                    +---------------------------+
**                    | Check mask pattern        |
**                    +---------------------------+
**                         /          \
**                  With mask      No mask
**                       |              |
**                       v              v
**            +-------------------+  List all
**            | Filter servers    |  servers
**            | matching mask     |
**            +-------------------+
**                       |
**                       v
**            +-------------------+
**            | For each server:  |
**            | RPL_LINKS (364)   |
**            | Send server info  |
**            +-------------------+
**                       |
**                       v
**            +-------------------+
**            | RPL_ENDOFLINKS    |
**            | (365)             |
**            +-------------------+
**                       |
**                       v
**                   🔗 Network Map 🔗
**
**  LINKS Information:
**  - Server name
**  - Uplink server
**  - Hopcount
**  - Server info/description
**
**  Reply format:
**  RPL_LINKS (364): :server 364 nick <server> <uplink> :<hopcount> <info>
**  RPL_ENDOFLINKS (365): :server 365 nick <mask> :End of LINKS list
**
**  Format: LINKS [[<remote server>] <server mask>]
**  Examples:
**  LINKS
**  LINKS *.fr
**  LINKS irc.example.com *.net
**
**  Use Cases:
**  - View network topology
**  - Find servers matching pattern
**  - Network administration
**
**  NOTE: In single-server implementation, returns only this server
**
** ============================================================================
*/
