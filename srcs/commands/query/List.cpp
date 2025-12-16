/*
** ============================================================================
**                          LIST COMMAND - RFC 1459
** ============================================================================
**
**                  List Channels and Topics
**                              |
**                              v
**                    +----------------------+
**                    | LIST [#channel]      |
**                    +----------------------+
**                              |
**                        /           \
**               Channel given    No channel
**                      |              |
**                      v              v
**            +----------------+  +----------------+
**            | List that      |  | List all       |
**            | specific chan  |  | channels       |
**            +----------------+  +----------------+
**                      |              |
**                      v              v
**            +--------------------------------+
**            | RPL_LIST (322) for each channel|
**            | Format: #chan <users> :<topic> |
**            +--------------------------------+
**                              |
**                              v
**            +--------------------------------+
**            | RPL_LISTEND (323)             |
**            | :End of /LIST                 |
**            +--------------------------------+
**                              |
**                              v
**                          📜 Listed 📜
**
**  Reply format:
**  RPL_LIST: :server 322 nick #channel <visible_users> :<topic>
**  RPL_LISTEND: :server 323 nick :End of /LIST
**
**  Format: LIST [<channel>{,<channel>}]
**  Examples:
**  LIST
**  LIST #general
**  LIST #chan1,#chan2
**
**  Shows: Channel name, number of visible users, topic
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
**  ┌─────────────────────────────────────────┐
**  │     parseListCommand() - Parser         │
**  │                                         │
**  │  Extracts channel names (comma-separated)│
**  └─────────────────────────────────────────┘
*/
std::vector<std::string> Server::parseListCommand(const std::string &line) {
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
**  │  sendRPL_LIST() - Send Channel Info     │
**  │                                         │
**  │  Sends info about one channel           │
**  │  Format: 322 nick #chan <users> :<topic>│
**  └─────────────────────────────────────────┘
*/
void Server::sendRPL_LIST(const int &clientFd, Channel &channel) {
	std::string nick = this->Users[clientFd].getNickname();
	std::string chanName = channel.getName();
	
	// Count visible users
	size_t userCount = channel.getMembers().size();
	
	// Get topic
	std::string topic = channel.getTopic();
	if (topic.empty())
		topic = "No topic is set";
	
	// Build response
	std::string response = ":" + std::string(SERVER_NAME);
	response += " 322 " + nick +   + chanName +  ;
	
	// Add user count
	std::ostringstream oss;
	oss << userCount;
	response += oss.str();
	
	response +=  : + topic + "
";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
**  ┌─────────────────────────────────────────┐
**  │  sendRPL_LISTEND() - End of LIST        │
**  │                                         │
**  │  Signals end of LIST                    │
**  │  Format: 323 nick :End of /LIST         │
**  └─────────────────────────────────────────┘
*/
void Server::sendRPL_LISTEND(const int &clientFd) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  323  + nick;
	response +=  :End of /LIST\r\n;
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* this fonction will handle the LIST command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleList(const int &clientFd, const std::string &line) {
	std::vector<std::string> requestedChannels = parseListCommand(line);
	
	if (requestedChannels.empty()) {
		for (std::vector<Channel>::iterator chan = channelList.begin(); 
		     chan != channelList.end(); ++chan) {
			sendRPL_LIST(clientFd, *chan);
		}
	} else {
		for (size_t i = 0; i < requestedChannels.size(); i++) {
			for (std::vector<Channel>::iterator chan = channelList.begin(); 
			     chan != channelList.end(); ++chan) {
				if (chan->getName() == requestedChannels[i]) {
					sendRPL_LIST(clientFd, *chan);
					break;
				}
			}
		}
	}
	
	sendRPL_LISTEND(clientFd);
}
