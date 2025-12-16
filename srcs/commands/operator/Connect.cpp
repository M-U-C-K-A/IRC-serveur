/*
** ============================================================================
**                      CONNECT COMMAND - RFC 1459
** ============================================================================
**
**          Force Server Connection Flow (IRCOP Only)
**                              |
**                              v
**                 +-------------------------------+
**                 | CONNECT <server> <port>       |
**                 | [<remote server>]             |
**                 +-------------------------------+
**                              |
**                              v
**                 +-------------------------------+
**                 | Check if user is IRCOP        |
**                 +-------------------------------+
**                         /              \
**                      YES                NO
**                       |                  |
**                       v                  v
**           +---------------------+  ERR_NOPRIVILEGES (481)
**           | Parse target server |  "Permission Denied"
**           | and port            |
**           +---------------------+
**                       |
**                       v
**           +---------------------+
**           | Validate parameters |
**           +---------------------+
**                       |
**                       v
**           +---------------------+
**           | Attempt connection  |
**           | to remote server    |
**           +---------------------+
**                   /        \
**              Success      Fail
**                 |           |
**                 v           v
**        +-----------+   ERR_NOSUCHSERVER (402)
**        | Broadcast |   "No such server"
**        | to network|
**        +-----------+
**                |
**                v
**         🔗 Connected 🔗
**
**  Format: CONNECT <target server> <port> [<remote server>]
**  Example: CONNECT irc.example.com 6667
**
**  NOTE: In single-server implementation, this is mainly
**        for protocol compliance
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
**  ┌─────────────────────────────────────────┐
**  │     parseConnectCommand() - Parser      │
**  │                                         │
**  │  Extracts target server and port        │
**  └─────────────────────────────────────────┘
*/
struct ConnectParams {
	std::string targetServer;
	std::string port;
	std::string remoteServer;
};

/*
* this fonction will parse the CONNECT command
* @param line the line to parse
* @return ConnectParams the parsed parameters
*/
ConnectParams Server::parseConnectCommand(const std::string &line) {
	ConnectParams params;
	
	std::istringstream iss(line.substr(CONNECT_CMD_LENGTH));
	iss >> params.targetServer >> params.port >> params.remoteServer;

	return params;
}

/*
* this fonction will handle the CONNECT command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleConnect(const int &clientFd, const std::string &line) {
	if (!this->Users[clientFd].isOperator()) {
		sendERR_NOPRIVILEGES(clientFd);
		return;
	}
	ConnectParams params = parseConnectCommand(line);

	if (params.targetServer.empty() || params.port.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, CMD_CONNECT);
		return;
	}

	std::string response = ":";
	response += SERVER_NAME;
	response += " NOTICE ";
	response += this->Users[clientFd].getNickname();
	response += " :";
	response += MSG_ERR_SINGLE_SERVER_CONNECT;
	response += "\r\n";
	send(clientFd, response.c_str(), response.length(), 0);

	std::cout << "IRCOP " << this->Users[clientFd].getNickname() 
	          << " attempted CONNECT to " << params.targetServer 
	          << ":" << params.port << std::endl;
}
