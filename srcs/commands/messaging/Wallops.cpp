/*
** ============================================================================
**                       WALLOPS COMMAND - RFC 1459
** ============================================================================
**
**                  Broadcast to All Operators
**                              |
**                              v
**                    +----------------------+
**                    | WALLOPS :<message>   |
**                    +----------------------+
**                              |
**                              v
**                    +----------------------+
**                    | Check if IRCOP       |
**                    +----------------------+
**                         /          \
**                      YES            NO
**                       |              |
**                       v              v
**              +----------------+  ERR_NOPRIVILEGES (481)
**              | Broadcast to   |  "Permission Denied"
**              | all IRCOPs     |
**              +----------------+
**                       |
**                       v
**              +----------------+
**              | :sender WALLOPS|
**              | :message       |
**              +----------------+
**                       |
**                       v
**                   📡 Sent 📡
**
**  WALLOPS sends a message to all IRC Operators currently online
**  
**  Format: WALLOPS :<message>
**  Example: WALLOPS :Network maintenance in 10 minutes
**           WALLOPS :DDoS attack detected from 192.168.1.1
**
**  Broadcast format:
**  :sender!user@host WALLOPS :message
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
**  ┌─────────────────────────────────────────┐
**  │     parseWallopsMessage() - Parser      │
**  │                                         │
**  │  Extracts message after :               │
**  └─────────────────────────────────────────┘
*/
std::string Server::parseWallopsMessage(const std::string &line) {
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
**  ┌─────────────────────────────────────────┐
**  │  broadcastWallops() - Send to All Ops   │
**  │                                         │
**  │  Sends message to all IRC Operators     │
**  │  currently connected                    │
**  └─────────────────────────────────────────┘
*/
void Server::broadcastWallops(const int &senderFd, const std::string &message) {
	std::string msg = ":" + this->Users[senderFd].getNickname();
	msg += "!" + this->Users[senderFd].getUsername();
	msg += "@localhost WALLOPS :" + message + "
";

	int count = 0;
	
	for (std::map<int, User>::iterator it = this->Users.begin(); 
	     it != this->Users.end(); ++it) {
		if (it->second.isOperator()) {
			send(it->first, msg.c_str(), msg.length(), 0);
			count++;
		}
	}

	std::cout << "WALLOPS from " << this->Users[senderFd].getNickname() 
	          << " sent to " << count << " operators" << std::endl;
}

/*
* this fonction will handle the WALLOPS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleWallops(const int &clientFd, const std::string &line) {
	if (!this->Users[clientFd].isOperator()) {
		sendERR_NOPRIVILEGES(clientFd);
		return;
	}

	std::string message = parseWallopsMessage(line);

	if (message.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, CMD_WALLOPS);
		return;
	}

	broadcastWallops(clientFd, message);
}
