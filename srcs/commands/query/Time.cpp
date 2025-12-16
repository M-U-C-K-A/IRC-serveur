/*
** ============================================================================
**                         TIME COMMAND - RFC 1459
** ============================================================================
**
**                Query Server Time
**                              |
**                              v
**                    +---------------------+
**                    | TIME [<server>]     |
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
**            | RPL_TIME (391)    |  (multi-server)
**            | Send local time   |
**            +-------------------+
**                       |
**                       v
**                   🕐 Time Info 🕐
**
**  RPL_TIME Format:
**  :server 391 nick <server> :<time string>
**
**  Example reply:
**  :irc.server.com 391 john irc.server.com :Monday December 16 2025 -- 01:30:45 +01:00
**
**  Format: TIME [<server>]
**  Examples:
**  TIME
**  TIME irc.example.com
**
**  Provides:
**  - Current date and time on the server
**  - Timezone information
**  - Day of week
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"
#include <ctime>

/*
**  ┌─────────────────────────────────────────┐
**  │  getFormattedTime() - Format Time String│
**  │                                         │
**  │  Returns current server time as string  │
**  └─────────────────────────────────────────┘
*/
std::string Server::getFormattedTime() {
	time_t now = time(NULL);
	struct tm *timeinfo = localtime(&now);
	
	char buffer[256];
	// Format: Monday December 16 2025 -- 01:30:45 +01:00
	strftime(buffer, sizeof(buffer), %A %B %d %Y -- %H:%M:%S %z, timeinfo);
	
	return std::string(buffer);
}

/*
**  ┌─────────────────────────────────────────┐
**  │  sendRPL_TIME() - Send Time Reply       │
**  │                                         │
**  │  RPL_TIME (391)                         │
**  │  Format: 391 nick <server> :<time>      │
**  └─────────────────────────────────────────┘
*/
void Server::sendRPL_TIME(const int &clientFd) {
	std::string nick = this->Users[clientFd].getNickname();
	std::string timeStr = getFormattedTime();
	
	std::string response = ":" + std::string(SERVER_NAME);
	response +=  391  + nick +  ;
	response += SERVER_NAME;
	response +=  : + timeStr + "
";
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* this fonction will handle the TIME command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleTime(const int &clientFd, const std::string &line) {
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
		sendRPL_TIME(clientFd);
	} else {
		sendERR_NOSUCHSERVER(clientFd, targetServer);
	}
	
	std::cout << TIME query from  << this->Users[clientFd].getNickname() << std::endl;
}
