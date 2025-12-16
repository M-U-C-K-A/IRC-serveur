/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:41:16 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:10:29 by hdelacou         ###   ########.fr       */
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
**                       PRIVMSG / MESSAGE COMMAND
** ============================================================================
**
**       Sender                 Server                 Receiver
**         |                       |                        |
**         |  PRIVMSG target :msg  |                        |
**         |---------------------> |                        |
**         |                       |                        |
**         |                       | Check target type      |
**         |                       |                        |
**         |  Channel (#chan)      |     User (nickname)    |
**         |         |             |           |            |
**         |         v             |           v            |
**         |                       |                        |
**         |  Broadcast to all     |  Send to specific user |
**         |  channel members      |                        |
**         |  (except sender)      |                        |
**         |                       |                        |
**         |                       | :sender PRIVMSG target |
**         |                       |----------------------> |
**         |                       |                        |
**         |                       |                        |
**         |                       |                        |
**         v                       v                        v
**
**  Flow: Sender -> Server routes -> Receiver(s) get message
**
**  Target Types:
**  - #channel : Broadcast to all channel members
**  - nickname : Private message to one user
**  - Special  : DCC SEND (file transfer - bonus)
**
**  Possible Errors:
**  - ERR_NORECIPIENT (411): No recipient given
**  - ERR_NOTEXTTOSEND (412): No text to send
**  - ERR_NOSUCHNICK (401): Nickname doesn't exist
**  - ERR_CANNOTSENDTOCHAN (404): Cannot send to channel
**
** ============================================================================
*/
