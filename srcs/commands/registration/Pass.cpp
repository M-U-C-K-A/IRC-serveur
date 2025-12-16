/*
** ============================================================================
**                         PASS COMMAND - RFC 1459
** ============================================================================
**
**                       Connection Flow
**                              |
**                              v
**                    +-------------------+
**                    |   Client Connect  |
**                    +-------------------+
**                              |
**                              | PASS <password>
**                              v
**                    +-------------------+
**                    |  Validate Format  |
**                    +-------------------+
**                         /         \
**                   Valid           Invalid
**                     |                |
**                     v                v
**            +---------------+  ERR_NEEDMOREPARAMS (461)
**            | Store Password|  "Not enough parameters"
**            +---------------+
**                     |
**                     v
**            +---------------+
**            | Wait for NICK |
**            | and USER cmds |
**            +---------------+
**                     |
**                     v
**            +---------------+
**            | Registration  |
**            | Complete      |
**            +---------------+
**
**  IMPORTANT NOTES:
**  - PASS must be sent BEFORE NICK and USER
**  - If sent after registration, ERR_ALREADYREGISTRED (462) is returned
**  - Password is stored but validated during full registration
**  - Empty password generates ERR_NEEDMOREPARAMS
**
**  Format: PASS <password>
**  Example: PASS secretpassword123
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* this fonction will handle the PASS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handlePass(const int &clientFd, const std::string &line)
{
	if (this->Users[clientFd].isRegistered())
	{
		sendERR_ALREADYREGISTRED(clientFd);
		return;
	}

	std::string password = getParam(PASS_CMD_LENGTH, line);

	if (password.empty())
	{
		sendERR_NEEDMOREPARAMS(clientFd, CMD_PASS);
		return;
	}

	this->Users[clientFd].setPassword(password);
	std::cout << "Password set for user " << clientFd << std::endl;
}
