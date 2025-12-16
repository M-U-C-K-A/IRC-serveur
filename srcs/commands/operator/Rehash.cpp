/*
** ============================================================================
**                      REHASH COMMAND - RFC 1459
** ============================================================================
**
**          Reload Server Configuration (IRCOP Only)
**                              |
**                              v
**                    +---------------------+
**                    | REHASH              |
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
**              +---------------+  ERR_NOPRIVILEGES (481)
**              | Read config   |  "Permission Denied"
**              | file          |
**              +---------------+
**                       |
**                  /         \
**              Success      Error
**                 |            |
**                 v            v
**       +---------------+  ERR_FILEERROR (?)
**       | Update server |  "Cannot read config"
**       | settings      |
**       +---------------+
**                 |
**                 v
**       +---------------+
**       | RPL_REHASHING |
**       | confirmation  |
**       +---------------+
**                 |
**                 v
**           🔄 Reloaded 🔄
**
**  Format: REHASH
**  
**  Reloads:
**  - Operator passwords
**  - Port bindings (if possible)
**  - MOTD
**  - Other configuration settings
**
**  NOTE: Some settings may require server restart
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include <fstream>

/*
**  ┌─────────────────────────────────────────┐
**  │     reloadConfiguration() - Config Read │
**  │                                         │
**  │  Reads and applies configuration file   │
**  │  Returns true on success                │
**  └─────────────────────────────────────────┘
*/
bool Server::reloadConfiguration() {
	// TODO: Implement actual config file reading
	// For now, this is a placeholder
	
	std::cout << "Reloading configuration..." << std::endl;
	
	// Example: Read from "server.conf"
	std::ifstream configFile("server.conf");
	if (!configFile.is_open()) {
		std::cout << "Warning: server.conf not found" << std::endl;
		return false;
	}

	// Parse configuration
	std::string line;
	while (std::getline(configFile, line)) {
		// Skip comments and empty lines
		if (line.empty() || line[0] == '#')
			continue;
			
		// TODO: Parse configuration directives
		// Example: oper_user = admin
		//          oper_pass = secretpass
	}

	configFile.close();
	std::cout << "Configuration reloaded successfully" << std::endl;
	return true;
}

/*
* this fonction will handle the REHASH command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleRehash(const int &clientFd, const std::string &line) {
	(void)line;

	if (!this->Users[clientFd].isOperator()) {
		sendERR_NOPRIVILEGES(clientFd);
		return;
	}

	std::cout << "IRCOP " << this->Users[clientFd].getNickname() 
	          << " (fd: " << clientFd << ") initiated REHASH" << std::endl;

	bool success = reloadConfiguration();

	if (success) {
		sendRPL_REHASHING(clientFd);
	} else {
		std::string response = ":";
		response += SERVER_NAME;
		response += " NOTICE ";
		response += this->Users[clientFd].getNickname();
		response += " :Error reloading configuration file\r\n";
		send(clientFd, response.c_str(), response.length(), 0);
	}
}

/*
**  ┌─────────────────────────────────────────┐
**  │     Response Functions                  │
**  └─────────────────────────────────────────┘
*/

// RPL_REHASHING (382): Rehashing configuration file
void Server::sendRPL_REHASHING(const int &clientFd) {
	std::string nick = this->Users[clientFd].getNickname();
	
	std::string response = ":";
	response += SERVER_NAME;
	response += " 382 ";
	response += nick;
	response += " server.conf :Rehashing\r\n";
	
	send(clientFd, response.c_str(), response.length(), 0);
}
