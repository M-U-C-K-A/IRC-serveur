/*
** ============================================================================
**                         AWAY COMMAND - RFC 1459
** ============================================================================
**
**                   Set/Unset Away Status
**                              |
**                              v
**                    +---------------------+
**                    | AWAY [:<message>]   |
**                    +---------------------+
**                              |
**                        /           \
**                With message    No message
**                      |              |
**                      v              v
**            +----------------+  +----------------+
**            | Set AWAY status|  | Unset AWAY     |
**            | Store message  |  | Clear message  |
**            +----------------+  +----------------+
**                      |              |
**                      v              v
**            +----------------+  +----------------+
**            | RPL_NOWAWAY    |  | RPL_UNAWAY     |
**            | (306)          |  | (305)          |
**            +----------------+  +----------------+
**                      |              |
**                      v              v
**                     Away            Back
**
**  When user is AWAY:
**  - PRIVMSGs trigger RPL_AWAY (301) to sender
**  - WHOIS shows away message
**  - Status visible to other users
**
**  Format: AWAY [:<away message>]
**  Examples:
**  AWAY :Gone to lunch, back in 1 hour
**  AWAY :AFK
**  AWAY                             (unset away)
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* Parse away message from AWAY command
* @param line - Full command line
* @return Away message or empty string if unset
*/
std::string Server::parseAwayMessage(const std::string &line)
{
	size_t msgStart = line.find(':');
	
	if (msgStart == std::string::npos)
		return "";

	std::string message = line.substr(msgStart + 1);
	
	if (!message.empty() && message[message.length() - 1] == '\r')
		message.erase(message.length() - 1);
	if (!message.empty() && message[message.length() - 1] == '\n')
		message.erase(message.length() - 1);

	return message;
}

/*
* this fonction will handle the AWAY command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleAway(const int &clientFd, const std::string &line)
{
	std::string awayMessage = parseAwayMessage(line);

	if (awayMessage.empty())
	{
		sendRPL_UNAWAY(clientFd);
		std::cout << "User " << this->Users[clientFd].getNickname() 
		          << " is no longer away" << std::endl;
	}
	else
	{
		sendRPL_NOWAWAY(clientFd);
		std::cout << "User " << this->Users[clientFd].getNickname() 
		          << " is now away: " << awayMessage << std::endl;
	}
}
