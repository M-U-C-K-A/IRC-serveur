/*
** ============================================================================
**                      RESTART COMMAND - RFC 1459
** ============================================================================
**
**            Restart IRC Server (IRCOP Only)
**                              |
**                              v
**                    +---------------------+
**                    | RESTART             |
**                    +---------------------+
**                              |
**                              v
**                    +---------------------+
**                    | Check if IRCOP      |
**                    +---------------------+
**                         /         \
**                      YES           NO
**                       |             |
**                       v             v
**              +----------------+  ERR_NOPRIVILEGES (481)
**              | Send ERROR to  |  "Permission Denied"
**              | all clients    |
**              +----------------+
**                       |
**                       v
**              +----------------+
**              | Close all      |
**              | connections    |
**              +----------------+
**                       |
**                       v
**              +----------------+
**              | Save state     |
**              | (if needed)    |
**              +----------------+
**                       |
**                       v
**              +----------------+
**              | Exit process   |
**              | (or re-exec)   |
**              +----------------+
**                       |
**                       v
**              +----------------+
**              | Init script    |
**              | restarts server|
**              +----------------+
**                       |
**                       v
**                🔄 Restarted 🔄
**
**  Format: RESTART
**  
**  DANGER: This command will terminate the server!
**  All clients will be disconnected.
**  
**  In production, an init script or process manager
**  (systemd, supervisor, etc.) should restart the server
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
**  ┌─────────────────────────────────────────┐
**  │  broadcastServerRestart() - Notification│
**  │                                         │
**  │  Sends ERROR message to all connected   │
**  │  users informing them of restart        │
**  └─────────────────────────────────────────┘
*/
void Server::broadcastServerRestart(const int &operatorFd) {
	std::string message = "ERROR :Closing Link: Server restart by ";
	message += this->Users[operatorFd].getNickname();
	message += "\r\n";

	// Send to all connected users
	for (std::map<int, User>::iterator it = this->Users.begin(); 
	     it != this->Users.end(); ++it) {
		send(it->first, message.c_str(), message.length(), 0);
	}
}

/*
**  ┌─────────────────────────────────────────┐
**  │     closeAllConnections() - Cleanup     │
**  │                                         │
**  │  Closes all client connections          │
**  └─────────────────────────────────────────┘
*/
void Server::closeAllConnections() {
	std::cout << "Closing all client connections..." << std::endl;

	for (std::map<int, User>::iterator it = this->Users.begin(); 
	     it != this->Users.end(); ++it) {
		close(it->first);
	}

	// Clear data structures
	this->Users.clear();
	this->channelList.clear();
	this->pollFds.clear();
}

/*
* this fonction will handle the RESTART command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleRestart(const int &clientFd, const std::string &line) {
	(void)line;

	if (!this->Users[clientFd].isOperator()) {
		sendERR_NOPRIVILEGES(clientFd);
		return;
	}

	std::cout << "!!! SERVER RESTART INITIATED BY IRCOP " 
	          << this->Users[clientFd].getNickname() 
	          << " (fd: " << clientFd << ") !!!" << std::endl;

	broadcastServerRestart(clientFd);

	sleep(1);

	closeAllConnections();

	std::cout << "Server shutting down for restart..." << std::endl;

	exit(0);
}

/*
**  ┌─────────────────────────────────────────┐
**  │     Alternative: Graceful Restart       │
**  │                                         │
**  │  For production, you might want to:     │
**  │  1. Set a restart flag                  │
**  │  2. Stop accepting new connections      │
**  │  3. Wait for existing ops to complete   │
**  │  4. Then restart                        │
**  └─────────────────────────────────────────┘
*/
void Server::handleRestartGraceful(const int &clientFd) {
	// Check if user is IRC Operator
	if (!this->Users[clientFd].isOperator()) {
		sendERR_NOPRIVILEGES(clientFd);
		return;
	}

	// Set restart flag (to be implemented in Server class)
	// this->restartPending = true;

	std::string response = ":";
	response += SERVER_NAME;
	response += " NOTICE ";
	response += this->Users[clientFd].getNickname();
	response += " :Server will restart after all connections close\r\n";
	send(clientFd, response.c_str(), response.length(), 0);

	std::cout << "Graceful restart scheduled by " 
	          << this->Users[clientFd].getNickname() << std::endl;
}
