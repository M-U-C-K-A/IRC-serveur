/*
** ============================================================================
**                         QUIT COMMAND - RFC 1459
** ============================================================================
**
**                       Disconnection Flow
**                              |
**                              v
**                    +-------------------+
**                    | QUIT [:message]   |
**                    +-------------------+
**                              |
**                              v
**                    +-------------------+
**                    | Parse quit message|
**                    +-------------------+
**                              |
**                              v
**                    +-------------------+
**                    | Broadcast to all  |
**                    | shared channels:  |
**                    | :nick QUIT :msg   |
**                    +-------------------+
**                              |
**                              v
**                    +-------------------+
**                    | Remove from all   |
**                    | channels          |
**                    +-------------------+
**                              |
**                              v
**                    +-------------------+
**                    | Send ERROR to     |
**                    | client            |
**                    +-------------------+
**                              |
**                              v
**                    +-------------------+
**                    | Close connection  |
**                    | Remove from Users |
**                    +-------------------+
**                              |
**                              v
**                         Goodbye! 👋
**
**  QUIT MESSAGE FORMAT:
**  QUIT :<quit message>
**  
**  Examples:
**  QUIT :Leaving
**  QUIT :Gone to lunch
**  QUIT
**
**  If no message provided, default "Client Quit" is used
**
**  BROADCAST FORMAT:
**  :nick!user@host QUIT :quit message
**  Sent to all users who share at least one channel with quitting user
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
**  ┌─────────────────────────────────────────┐
**  │     parseQuitMessage() - Parser         │
**  │                                         │
**  │  Extracts quit message after :          │
**  │  Returns "Client Quit" if none provided │
**  └─────────────────────────────────────────┘
*/
std::string Server::parseQuitMessage(const std::string &line) {
	// Find quit message (after ':')
	size_t msgStart = line.find(':');
	
	if (msgStart == std::string::npos) {
		return "Client Quit"; // Default message
	}

	std::string quitMsg = line.substr(msgStart + 1);
	
	// Remove trailing CR/LF
	if (!quitMsg.empty() && quitMsg[quitMsg.length() - 1] == '\r')
		quitMsg.erase(quitMsg.length() - 1);
	if (!quitMsg.empty() && quitMsg[quitMsg.length() - 1] == '\n')
		quitMsg.erase(quitMsg.length() - 1);

	return quitMsg.empty() ? "Client Quit" : quitMsg;
}

/*
**  ┌─────────────────────────────────────────┐
**  │     broadcastQuit() - Notification      │
**  │                                         │
**  │  Sends QUIT message to all users who    │
**  │  share a channel with the quitting user │
**  │  Format: :nick!user@host QUIT :message  │
**  └─────────────────────────────────────────┘
*/
void Server::broadcastQuit(const int &clientFd, const std::string &quitMsg) {
	User &user = this->Users[clientFd];
	
	std::string message = ":";
	message += user.getNickname() + "!";
	message += user.getUsername() + "@";
	message += "localhost QUIT :";
	message += quitMsg + "\r\n";

	// Track who we've notified (avoid duplicates)
	std::set<int> notified;

	// Send to all users in shared channels
	for (std::vector<Channel>::iterator chan = channelList.begin(); 
	     chan != channelList.end(); ++chan) {
		if (chan->isMember(clientFd)) {
			const std::vector<int> &members = chan->getMembers();
			for (size_t i = 0; i < members.size(); i++) {
				int memberFd = members[i];
				// Don't send to quitting user or already notified users
				if (memberFd != clientFd && notified.find(memberFd) == notified.end()) {
					send(memberFd, message.c_str(), message.length(), 0);
					notified.insert(memberFd);
				}
			}
		}
	}
}

/*
**  ┌─────────────────────────────────────────┐
**  │     removeFromAllChannels() - Cleanup   │
**  │                                         │
**  │  Removes user from all channels they    │
**  │  are a member of                        │
**  └─────────────────────────────────────────┘
*/
void Server::removeFromAllChannels(const int &clientFd) {
	for (std::vector<Channel>::iterator chan = channelList.begin(); 
	     chan != channelList.end(); ) {
		if (chan->isMember(clientFd)) {
			chan->removeMember(clientFd);
			
			// If channel is now empty, remove it
			if (chan->isEmpty()) {
				std::cout << "Removing empty channel: " << chan->getName() << std::endl;
				chan = channelList.erase(chan);
				continue;
			}
		}
		++chan;
	}
}

/*
* this fonction will handle the QUIT command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleQuit(const int &clientFd, const std::string &line) {
	std::string quitMsg = parseQuitMessage(line);

	std::cout << "User " << this->Users[clientFd].getNickname() 
	          << " (fd: " << clientFd << ") quitting: " 
	          << quitMsg << std::endl;

	broadcastQuit(clientFd, quitMsg);

	removeFromAllChannels(clientFd);

	std::string errorMsg = "ERROR :Closing Link: localhost (";
	errorMsg += quitMsg + ")\r\n";
	send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);

	close(clientFd);

	removeFromPollFds(clientFd);

	this->Users.erase(clientFd);

	std::cout << "Connection closed for fd: " << clientFd << std::endl;
}

/*
**  ┌─────────────────────────────────────────┐
**  │     removeFromPollFds() - FD Cleanup    │
**  │                                         │
**  │  Removes file descriptor from poll      │
**  │  monitoring array                       │
**  └─────────────────────────────────────────┘
*/
void Server::removeFromPollFds(const int &clientFd) {
	for (size_t i = 0; i < this->pollFds.size(); i++) {
		if (this->pollFds[i].fd == clientFd) {
			this->pollFds.erase(this->pollFds.begin() + i);
			break;
		}
	}
}
