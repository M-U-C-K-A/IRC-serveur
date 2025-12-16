/*
** ============================================================================
**                         NICK COMMAND - RFC 1459
** ============================================================================
**
**                       Nickname Flow
**                              |
**                              v
**                    +-------------------+
**                    | NICK <nickname>   |
**                    +-------------------+
**                              |
**                              v
**                    +-------------------+
**                    |  Validate Format  |
**                    |  (length, chars)  |
**                    +-------------------+
**                         /         \
**                   Valid           Invalid
**                     |                |
**                     v                v
**            +----------------+   ERR_ERRONEUSNICKNAME (432)
**            | Check if taken |   "Erroneous nickname"
**            +----------------+
**                   /         \
**              Available    In Use
**                 |            |
**                 v            v
**        +---------------+  ERR_NICKNAMEINUSE (433)
**        | Set Nickname  |  "Nickname is already in use"
**        +---------------+
**                 |
**                 v
**        +---------------+
**        | Broadcast to  |
**        | all channels  |
**        +---------------+
**
**  NICKNAME RULES:
**  - Maximum 9 characters
**  - Must start with a letter
**  - Can contain: letters, digits, special chars: [ ] \ ` _ ^ { | }
**  - Case insensitive for collision detection
**  - No spaces allowed
**
**  Format: NICK <nickname>
**  Example: NICK JohnDoe
**           NICK Alice_42
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
**  ┌─────────────────────────────────────────┐
**  │     isValidNickname() - Validator       │
**  │                                         │
**  │  Checks:                                │
**  │  - Not empty                            │
**  │  - Max 9 characters                     │
**  │  - Starts with letter                   │
**  │  - Valid characters only                │
**  └─────────────────────────────────────────┘
*/
bool Server::isValidNickname(const std::string &nickname) {
	// Check if empty or too long
	if (nickname.empty() || nickname.length() > 9)
		return false;

	// First character must be a letter
	if (!isalpha(nickname[0]))
		return false;

	// Check all characters
	for (size_t i = 0; i < nickname.length(); i++) {
		char c = nickname[i];
		// Valid: letters, digits, and special IRC chars
		if (!isalnum(c) && c != '[' && c != ']' && c != '\\' && 
		    c != '`' && c != '_' && c != '^' && c != '{' && 
		    c != '|' && c != '}' && c != '-')
			return false;
	}

	return true;
}

/*
**  ┌─────────────────────────────────────────┐
**  │     isNicknameTaken() - Collision Check │
**  │                                         │
**  │  Iterates through all users to check   │
**  │  if nickname is already in use          │
**  │  (case-insensitive comparison)          │
**  └─────────────────────────────────────────┘
*/
bool Server::isNicknameTaken(const std::string &nickname, const int &clientFd) {
	for (std::map<int, User>::iterator it = this->Users.begin(); 
	     it != this->Users.end(); ++it) {
		// Skip checking against self
		if (it->first == clientFd)
			continue;
			
		// Case-insensitive comparison
		if (strcasecmp(it->second.getNickname().c_str(), nickname.c_str()) == 0)
			return true;
	}
	return false;
}

/*
* this fonction will handle the NICK command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleNick(const int &clientFd, const std::string &line) {
	std::string newNick = getParam(NICK_CMD_LENGTH, line);

	if (newNick.empty()) {
		sendERR_NONICKNAMEGIVEN(clientFd);
		return;
	}

	if (!isValidNickname(newNick)) {
		sendERR_ERRONEUSNICKNAME(clientFd, newNick);
		return;
	}

	if (isNicknameTaken(newNick, clientFd)) {
		sendERR_NICKNAMEINUSE(clientFd, newNick);
		return;
	}

	std::string oldNick = this->Users[clientFd].getNickname();

	this->Users[clientFd].setNickname(newNick);

	if (!oldNick.empty()) {
		broadcastNickChange(clientFd, oldNick, newNick);
	}

	std::cout << "Nickname set: " << oldNick << " -> " << newNick 
	          << " (fd: " << clientFd << ")" << std::endl;

	checkUserRegistration(clientFd);
}

/*
**  ┌─────────────────────────────────────────┐
**  │     broadcastNickChange() - Notify      │
**  │                                         │
**  │  Sends NICK change to all users who     │
**  │  share a channel with this user         │
**  │  Format: :oldnick!user@host NICK :new   │
**  └─────────────────────────────────────────┘
*/
void Server::broadcastNickChange(const int &clientFd, 
                                  const std::string &oldNick,
                                  const std::string &newNick) {
	std::string message = ":";
	message += oldNick + "!";
	message += this->Users[clientFd].getUsername() + "@";
	message += "localhost NICK :";
	message += newNick + "\r\n";

	// Send to the user themselves
	send(clientFd, message.c_str(), message.length(), 0);

	// Send to all users in shared channels
	std::set<int> notified; // Track who we've notified
	notified.insert(clientFd);

	for (std::vector<Channel>::iterator chan = channelList.begin(); 
	     chan != channelList.end(); ++chan) {
		if (chan->isMember(clientFd)) {
			const std::vector<int> &members = chan->getMembers();
			for (size_t i = 0; i < members.size(); i++) {
				if (notified.find(members[i]) == notified.end()) {
					send(members[i], message.c_str(), message.length(), 0);
					notified.insert(members[i]);
				}
			}
		}
	}
}

/*
**  ┌─────────────────────────────────────────┐
**  │     Error Response Functions            │
**  └─────────────────────────────────────────┘
*/

// ERR_NONICKNAMEGIVEN (431): No nickname given
void Server::sendERR_NONICKNAMEGIVEN(const int &clientFd) {
	std::string response = ":";
	response += SERVER_NAME;
	response += " 431 * :No nickname given\r\n";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

// ERR_ERRONEUSNICKNAME (432): Invalid nickname format
void Server::sendERR_ERRONEUSNICKNAME(const int &clientFd, const std::string &nick) {
	std::string response = ":";
	response += SERVER_NAME;
	response += " 432 * ";
	response += nick;
	response += " :Erroneous nickname\r\n";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

// ERR_NICKNAMEINUSE (433): Nickname already in use
void Server::sendERR_NICKNAMEINUSE(const int &clientFd, const std::string &nick) {
	std::string response = ":";
	response += SERVER_NAME;
	response += " 433 * ";
	response += nick;
	response += " :Nickname is already in use\r\n";
	
	send(clientFd, response.c_str(), response.length(), 0);
}
