/*
** ============================================================================
**                         NAMES COMMAND - RFC 1459
** ============================================================================
**
**                List Users in Channel(s)
**                              |
**                              v
**                    +----------------------+
**                    | NAMES [#channel]     |
**                    +----------------------+
**                              |
**                        /           \
**               Channel given    No channel
**                      |              |
**                      v              v
**            +----------------+  +----------------+
**            | List users in  |  | List all users |
**            | that channel   |  | in all channels|
**            +----------------+  +----------------+
**                      |              |
**                      v              v
**            +--------------------------------+
**            | RPL_NAMREPLY (353) for each   |
**            | channel with user list        |
**            +--------------------------------+
**                              |
**                              v
**            +--------------------------------+
**            | RPL_ENDOFNAMES (366)          |
**            +--------------------------------+
**                              |
**                              v
**                          📋 Listed 📋
**
**  Format of replies:
**  RPL_NAMREPLY: :server 353 nick = #channel :@op +voice user1 user2
**  RPL_ENDOFNAMES: :server 366 nick #channel :End of /NAMES list
**
**  Prefixes:
**  @ = channel operator
**  + = voice (if implemented)
**
**  Format: NAMES [<channel>{,<channel>}]
**  Examples:
**  NAMES #general
**  NAMES
**  NAMES #chan1,#chan2
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
**  ┌─────────────────────────────────────────┐
**  │     parseNamesCommand() - Parser        │
**  │                                         │
**  │  Extracts channel names (comma-separated)│
**  └─────────────────────────────────────────┘
*/
std::vector<std::string> Server::parseNamesCommand(const std::string &line) {
	std::vector<std::string> channels;
	
	size_t pos = line.find(' ');
	if (pos == std::string::npos)
		return channels; // No channels specified
	
	std::string channelList = line.substr(pos + 1);
	
	// Remove trailing CR/LF
	if (!channelList.empty() && channelList[channelList.length() - 1] == '\r')
		channelList.erase(channelList.length() - 1);
	if (!channelList.empty() && channelList[channelList.length() - 1] == '\n')
		channelList.erase(channelList.length() - 1);
	
	// Parse comma-separated channels
	std::istringstream iss(channelList);
	std::string chan;
	while (std::getline(iss, chan, ',')) {
		if (!chan.empty())
			channels.push_back(chan);
	}
	
	return channels;
}

/*
**  ┌─────────────────────────────────────────┐
**  │  sendRPL_NAMREPLY() - Send Names List   │
**  │                                         │
**  │  Sends list of users in a channel       │
**  │  Format: 353 nick = #chan :@op user1... │
**  └─────────────────────────────────────────┘
*/
void Server::sendRPL_NAMREPLY(const int &clientFd, Channel &channel) {
	std::string nick = this->Users[clientFd].getNickname();
	std::string chanName = channel.getName();
	
	// Build user list with prefixes
	std::string userList = ;
	const std::vector<int> &members = channel.getMembers();
	
	for (size_t i = 0; i < members.size(); i++) {
		if (channel.isOperator(members[i])) {
			userList += @; // Operator prefix
		}
		userList += this->Users[members[i]].getNickname();
		if (i < members.size() - 1)
			userList +=  ;
	}
	
	// Send RPL_NAMREPLY
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  353  + nick +  =  + chanName;
	response +=  : + userList + "
";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
**  ┌─────────────────────────────────────────┐
**  │  sendRPL_ENDOFNAMES() - End of Names    │
**  │                                         │
**  │  Signals end of NAMES list              │
**  │  Format: 366 nick #chan :End of /NAMES  │
**  └─────────────────────────────────────────┘
*/
void Server::sendRPL_ENDOFNAMES(const int &clientFd, Channel &channel) {
	std::string nick = this->Users[clientFd].getNickname();
	std::string chanName = channel.getName();
	
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  366  + nick +   + chanName;
	response +=  :End of /NAMES list\r\n;
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* this fonction will handle the NAMES command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleNames(const int &clientFd, const std::string &line) {
	std::vector<std::string> requestedChannels = parseNamesCommand(line);
	
	if (requestedChannels.empty()) {
		for (std::vector<Channel>::iterator chan = channelList.begin(); 
		     chan != channelList.end(); ++chan) {
			sendRPL_NAMREPLY(clientFd, *chan);
			sendRPL_ENDOFNAMES(clientFd, *chan);
		}
	} else {
		for (size_t i = 0; i < requestedChannels.size(); i++) {
			bool found = false;
			for (std::vector<Channel>::iterator chan = channelList.begin(); 
			     chan != channelList.end(); ++chan) {
				if (chan->getName() == requestedChannels[i]) {
					sendRPL_NAMREPLY(clientFd, *chan);
					sendRPL_ENDOFNAMES(clientFd, *chan);
					found = true;
					break;
				}
			}
			if (!found) {
				std::string response = ":" + std::string(SERVER_NAME);
				response +=  366  + this->Users[clientFd].getNickname();
				response +=   + requestedChannels[i];
				response +=  :End of /NAMES list\r\n;
				send(clientFd, response.c_str(), response.length(), 0);
			}
		}
	}
}
