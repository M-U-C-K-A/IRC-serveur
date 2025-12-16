/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Info.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 03:59:59 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:00:44 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the INFO command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleInfo(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                         INFO COMMAND - RFC 1459
** ============================================================================
**
**                Query Server Information
**                              |
**                              v
**                    +---------------------+
**                    | INFO [<server>]     |
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
**            | Send server info: |  (multi-server)
**            | - RPL_INFO (371)  |
**            | Multiple lines    |
**            | - RPL_ENDOFINFO   |
**            +-------------------+
**                       |
**                       v
**                   ℹ️ Server Info ℹ️
**
**  INFO Command Returns:
**  - Server version and build information
**  - Authors and contributors
**  - Birth date of server
**  - Compile-time options
**  - Administrative information
**  - Any other server-specific information
**
**  Reply format:
**  RPL_INFO (371): :server 371 nick :<info line>
**  RPL_ENDOFINFO (374): :server 374 nick :End of INFO list
**
**  Format: INFO [<server>]
**  Examples:
**  INFO
**  INFO irc.example.com
**
**  Typical Information Provided:
**  - ft_irc Server v1.0
**  - Created by: [authors]
**  - Compiled: [date and time]
**  - Configuration options
**
** ============================================================================
*/
