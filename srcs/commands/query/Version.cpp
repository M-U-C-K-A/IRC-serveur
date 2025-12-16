/*
** ============================================================================
**                       VERSION COMMAND - RFC 1459
** ============================================================================
**
**                Query Server Version
**                              |
**                              v
**                    +---------------------+
**                    | VERSION [<server>]  |
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
**            | RPL_VERSION (351) |  (multi-server)
**            | Send version info |
**            +-------------------+
**                       |
**                       v
**               ℹ️ Version Info ℹ️
**
**  RPL_VERSION Format:
**  :server 351 nick <version>.<debuglevel> <server> :<comments>
**
**  Example reply:
**  :irc.server.com 351 john ft_irc-1.0 irc.server.com :IRC Server
**
**  Format: VERSION [<server>]
**  Examples:
**  VERSION
**  VERSION irc.example.com
**
**  Provides:
**  - Server software name and version
**  - Debug level / build info
**  - Additional comments about the server
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
**  ┌─────────────────────────────────────────┐
**  │  sendRPL_VERSION() - Send Version Info  │
**  │                                         │
**  │  RPL_VERSION (351)                      │
**  │  Format: 351 nick <ver> <server> :<info>│
**  └─────────────────────────────────────────┘
*/
void Server::sendRPL_VERSION(const int &clientFd) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  351  + nick +  ;
	response += ft_irc-1.0.0 ;  // Version
	response += SERVER_NAME;
	response +=  :Internet Relay Chat Server\r\n;
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* this fonction will handle the VERSION command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleVersion(const int &clientFd, const std::string &line) {
	std::string targetServer = ;
	size_t pos = line.find(' ');
	if (pos != std::string::npos) {
		targetServer = line.substr(pos + 1);
		if (!targetServer.empty() && targetServer[targetServer.length() - 1] == '\r')
			targetServer.erase(targetServer.length() - 1);
		if (!targetServer.empty() && targetServer[targetServer.length() - 1] == '\n')
			targetServer.erase(targetServer.length() - 1);
	}
	
	if (targetServer.empty() || targetServer == SERVER_NAME) {
		sendRPL_VERSION(clientFd);
	} else {
		sendERR_NOSUCHSERVER(clientFd, targetServer);
	}
	
	std::cout << VERSION query from  << this->Users[clientFd].getNickname() << std::endl;
}
