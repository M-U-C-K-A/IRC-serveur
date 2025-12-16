/*
** ============================================================================
**                         PING COMMAND - RFC 1459
** ============================================================================
**
**                  Test Connection Alive
**                              |
**                              v
**                    +---------------------+
**                    | PING <token>        |
**                    +---------------------+
**                              |
**                              v
**                    +---------------------+
**                    | Receive ping token  |
**                    +---------------------+
**                              |
**                              v
**                    +---------------------+
**                    | PONG <token>        |
**                    | Send back same token|
**                    +---------------------+
**                              |
**                              v
**                       🏓 Pong! 🏓
**
**  PING/PONG Mechanism:
**  - Client or server sends PING with a token
**  - Receiver MUST respond with PONG and same token
**  - Used to test connection is alive
**  - Used to measure latency
**  - Timeout if no PONG received
**
**  Format: PING <server1> [<server2>]
**  Examples:
**  PING :localhost
**  PING irc.server.com
**
**  Reply format:
**  PONG <server> :<token>
**
**  Common usage:
**  Client -> Server: PING :1234567890
**  Server -> Client: PONG irc.server.com :1234567890
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
**  ┌─────────────────────────────────────────┐
**  │     parsePingToken() - Extract Token    │
**  │                                         │
**  │  Extracts the ping token/server name    │
**  └─────────────────────────────────────────┘
*/
std::string Server::parsePingToken(const std::string &line)
{
	size_t pos = line.find(' ');
	if (pos == std::string::npos)
		return "";
	
	std::string token = line.substr(pos + 1);
	
	if (!token.empty() && token[0] == ':')
		token.erase(0, 1);
	
	if (!token.empty() && token[token.length() - 1] == '\r')
		token.erase(token.length() - 1);
	if (!token.empty() && token[token.length() - 1] == '\n')
		token.erase(token.length() - 1);
	
	return token;
}

/*
**  ┌─────────────────────────────────────────┐
**  │     sendPong() - Send PONG Reply        │
**  │                                         │
**  │  Sends PONG with same token as PING    │
**  └─────────────────────────────────────────┘
*/
void Server::sendPong(const int &clientFd, const std::string &token)
{
	std::string response = ":" + std::string(SERVER_NAME);
	response += " PONG " + std::string(SERVER_NAME);
	response += " :" + token + IRC_CRLF;
	
	send(clientFd, response.c_str(), response.length(), 0);
}

/*
* this fonction will handle the PING command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handlePing(const int &clientFd, const std::string &line)
{
	std::string token = parsePingToken(line);
	
	if (token.empty())
		token = SERVER_NAME;
	
	sendPong(clientFd, token);
	
	std::cout << "PING from " << this->Users[clientFd].getNickname() 
	          << " (token: " << token << ") - PONG sent" << std::endl;
}

/*
**  ┌─────────────────────────────────────────┐
**  │     handlePong() - Process PONG         │
**  │                                         │
**  │  Receives PONG replies from clients     │
**  │  Updates last activity timestamp        │
**  └─────────────────────────────────────────┘
*/
void Server::handlePong(const int &clientFd, const std::string &line)
{
	std::string token = parsePingToken(line);
	
	this->Users[clientFd].updateLastActivity();
	
	std::cout << "PONG received from " << this->Users[clientFd].getNickname() 
	          << " (token: " << token << ")" << std::endl;
}
