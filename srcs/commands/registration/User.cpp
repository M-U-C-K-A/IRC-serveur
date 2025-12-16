/*
** ============================================================================
**                         USER COMMAND - RFC 1459
** ============================================================================
**
**                       User Registration Flow
**                              |
**                              v
**                    +------------------------+
**                    | USER <user> 0 * :name  |
**                    +------------------------+
**                              |
**                              v
**                    +------------------------+
**                    | Check if registered    |
**                    +------------------------+
**                         /         \
**                   Not Yet       Already
**                     |              |
**                     v              v
**            +----------------+  ERR_ALREADYREGISTRED (462)
**            | Parse params:  |  "You may not reregister"
**            | - username     |
**            | - hostname     |
**            | - servername   |
**            | - realname     |
**            +----------------+
**                     |
**                     v
**            +----------------+
**            | Validate format|
**            +----------------+
**                   /       \
**              Valid      Invalid
**                |           |
**                v           v
**       +---------------+  ERR_NEEDMOREPARAMS (461)
**       | Store user    |  "Not enough parameters"
**       | information   |
**       +---------------+
**                |
**                v
**       +---------------+
**       | Check full    |
**       | registration: |
**       | PASS + NICK   |
**       | + USER        |
**       +---------------+
**                |
**                v
**       +---------------+
**       | Send Welcome  |
**       | messages (001-|
**       | 004 RPL)      |
**       +---------------+
**
**  USER COMMAND FORMAT:
**  USER <username> <hostname> <servername> :<realname>
**  
**  Example: USER john 0 * :John Doe
**           USER alice localhost server.com :Alice Wonderland
**
**  Parameters:
**  - username: User identifier (no spaces)
**  - hostname: Usually set to 0 (ignored by server)
**  - servername: Usually set to * (ignored by server)
**  - realname: Real name (can contain spaces, prefixed with :)
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
**  ┌─────────────────────────────────────────┐
**  │     parseUserCommand() - Parser         │
**  │                                         │
**  │  Extracts:                              │
**  │  - username (first parameter)           │
**  │  - realname (after :)                   │
**  └─────────────────────────────────────────┘
*/
struct UserParams {
	std::string username;
	std::string realname;
};

UserParams Server::parseUserCommand(const std::string &line) {
	UserParams params;
	
	// Find start of parameters (after "USER ")
	size_t pos = USER_CMD_LENGTH;
	if (pos >= line.length())
		return params;

	// Skip leading spaces
	while (pos < line.length() && line[pos] == ' ')
		pos++;

	// Extract username (up to next space)
	size_t usernameEnd = line.find(' ', pos);
	if (usernameEnd == std::string::npos)
		return params;
	
	params.username = line.substr(pos, usernameEnd - pos);

	// Find realname (after ':')
	size_t realnameStart = line.find(':', pos);
	if (realnameStart != std::string::npos) {
		params.realname = line.substr(realnameStart + 1);
		// Remove trailing CR/LF
		if (!params.realname.empty() && params.realname[params.realname.length() - 1] == '\r')
			params.realname.erase(params.realname.length() - 1);
		if (!params.realname.empty() && params.realname[params.realname.length() - 1] == '\n')
			params.realname.erase(params.realname.length() - 1);
	}

	return params;
}

/*
* this fonction will handle the USER command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleUser(const int &clientFd, const std::string &line) {
	if (this->Users[clientFd].isRegistered()) {
		sendERR_ALREADYREGISTRED(clientFd);
		return;
	}

	UserParams params = parseUserCommand(line);

	if (params.username.empty() || params.realname.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, CMD_USER);
		return;
	}

	this->Users[clientFd].setUsername(params.username);
	this->Users[clientFd].setRealname(params.realname);

	std::cout << "User info set for fd " << clientFd 
	          << ": username=" << params.username 
	          << ", realname=" << params.realname << std::endl;

	checkUserRegistration(clientFd);
}

/*
**  ┌─────────────────────────────────────────┐
**  │  checkUserRegistration() - Finalizer    │
**  │                                         │
**  │  Checks if user has provided:           │
**  │  - PASS (password)                      │
**  │  - NICK (nickname)                      │
**  │  - USER (username & realname)           │
**  │                                         │
**  │  If complete, sends welcome messages    │
**  └─────────────────────────────────────────┘
*/
void Server::checkUserRegistration(const int &clientFd) {
	User &user = this->Users[clientFd];

	// Check if all required information is present
	if (user.getPassword().empty() || 
	    user.getNickname().empty() || 
	    user.getUsername().empty() || 
	    user.getRealname().empty()) {
		// Not yet fully registered
		return;
	}

	// Mark user as registered
	user.setRegistered(true);

	// Send welcome messages
	sendWelcomeMessages(clientFd);

	std::cout << "User " << user.getNickname() 
	          << " (fd: " << clientFd << ") fully registered!" << std::endl;
}

/*
**  ┌─────────────────────────────────────────┐
**  │  sendWelcomeMessages() - Welcome Seq    │
**  │                                         │
**  │  Sends IRC welcome sequence:            │
**  │  - RPL_WELCOME (001)                    │
**  │  - RPL_YOURHOST (002)                   │
**  │  - RPL_CREATED (003)                    │
**  │  - RPL_MYINFO (004)                     │
**  └─────────────────────────────────────────┘
*/
void Server::sendWelcomeMessages(const int &clientFd) {
	User &user = this->Users[clientFd];
	std::string nick = user.getNickname();

	// RPL_WELCOME (001)
	std::string msg = ":";
	msg += SERVER_NAME;
	msg += " 001 " + nick + " :Welcome to the Internet Relay Network ";
	msg += nick + "!" + user.getUsername() + "@localhost\r\n";
	send(clientFd, msg.c_str(), msg.length(), 0);

	// RPL_YOURHOST (002)
	msg = ":";
	msg += SERVER_NAME;
	msg += " 002 " + nick + " :Your host is " + SERVER_NAME;
	msg += ", running version 1.0\r\n";
	send(clientFd, msg.c_str(), msg.length(), 0);

	// RPL_CREATED (003)
	msg = ":";
	msg += SERVER_NAME;
	msg += " 003 " + nick + " :This server was created today\r\n";
	send(clientFd, msg.c_str(), msg.length(), 0);

	// RPL_MYINFO (004)
	msg = ":";
	msg += SERVER_NAME;
	msg += " 004 " + nick + " " + SERVER_NAME;
	msg += " 1.0 io itklno\r\n";
	send(clientFd, msg.c_str(), msg.length(), 0);
}
