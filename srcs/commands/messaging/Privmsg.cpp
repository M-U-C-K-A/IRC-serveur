/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:41:16 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:29:52 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* this fonction will handle the PRIVMSG command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handlePrivateMessage(int clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}

/*
** ============================================================================
**                           PRIVMSG COMMAND
** ============================================================================
**
**  Format: PRIVMSG <target> :<message>
**
**  Action: Sends message to target (User or Channel).
**  Routing: Channel -> Broadcast to members. User -> Direct message.
**
** ============================================================================
*/
