/*
** ============================================================================
**                              JOIN COMMAND
** ============================================================================
**
**         User                  Server                   Channel
**          |                       |                          |
**          |   JOIN #channel [key] |                          |
**          |---------------------> |                          |
**          |                       |                          |
**          |                       | Check channel exists?    |
**          |                       |                          |
**          |                       |  YES                 NO  |
**          |                       |  |                   |   |
**          |                       |  v                   v   |
**          |                       |                          |
**          |                       | Check permissions:       |
**          |                       | - Invite only? (+i)      |
**          |                       | - Password match? (+k)   |
**          |                       | - User limit? (+l)       |
**          |                       | - Banned? (+b)           |
**          |                       |                          |
**          |   :nick JOIN #chan    |                          |
**          |<--------------------- |------------------------> |
**          |                       |                          |
**          |   RPL_TOPIC (332)     |                          |
**          |<--------------------- |                          |
**          |                       |                          |
**          |   RPL_NAMREPLY (353)  |                          |
**          |<--------------------- |                          |
**          |                       |                          |
**          |   RPL_ENDOFNAMES(366) |                          |
**          |<--------------------- |                          |
**          v                       v                          v
**
**  Flow: User joins -> Channel created/joined -> Names list sent
**
**  Possible Errors:
**  - ERR_NEEDMOREPARAMS (461): Not enough parameters
**  - ERR_BANNEDFROMCHAN (474): Cannot join (banned)
**  - ERR_INVITEONLYCHAN (473): Cannot join (+i)
**  - ERR_BADCHANNELKEY (475): Cannot join (wrong password)
**  - ERR_CHANNELISFULL (471): Cannot join (+l limit reached)
**  - ERR_BADCHANMASK (476): Bad channel name format
**
** ============================================================================
*/

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the JOIN command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
* @author Hugo
*/
void Server::handleJoin(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}
