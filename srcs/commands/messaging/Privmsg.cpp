/*
** ============================================================================
**                        PRIVMSG / MESSAGE COMMAND
** ============================================================================
**
**       Sender                 Server                 Receiver
**         |                       |                        |
**         |  PRIVMSG target :msg  |                        |
**         |---------------------> |                        |
**         |                       |                        |
**         |                       | Check target type:     |
**         |                       | - Channel (#chan)      |
**         |                       | - User (nickname)      |
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
**  Errors:
**  - ERR_NORECIPIENT (411): No recipient given
**  - ERR_NOTEXTTOSEND (412): No text to send
**  - ERR_NOSUCHNICK (401): No such nick/channel
**  - ERR_CANNOTSENDTOCHAN (404): Cannot send to channel
**
** ============================================================================
*/

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
