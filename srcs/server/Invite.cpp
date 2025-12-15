/*
** ============================================================================
**                            INVITE COMMAND
** ============================================================================
**
**     Operator                Channel                  Target User
**        |                       |                           |
**        |   INVITE user #chan   |                           |
**        |---------------------> |                           |
**        |                       |                           |
**        |   Check permissions   |                           |
**        |<--------------------> |                           |
**        |                       |                           |
**        |   Add to invite list  |                           |
**        |---------------------> |                           |
**        |                       |                           |
**        |                       |   :nick INVITE user #chan |
**        |                       |-------------------------> |
**        |                       |                           |
**        | RPL_INVITING          |                           |
**        |<----------------------|                           |
**        |                       |                           |
**        v                       v                           v
**
**  Flow: Operator invites user -> User added to invite list -> User notified
**
** ============================================================================
*/

#include "../../includes/Server.hpp"
#include "../../includes/Utils.hpp"

/*
* Find a channel by its name
* @param channelName - The name of the channel to find
* @return Reference to the Channel object if found, otherwise returns the first channel (scam
*/
Channel &Server::findChannelByName(const std::string &channelName) {
	for (std::vector<Channel>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
		if (it->getName() == channelName) {
			return (*it);
		}
	}
	return (this->channelList.front()); // <------- scam
}

/*
* Handle the INVITE command from a client
* @param clientFd - The file descriptor of the client sending the INVITE command
* @param line - The full command line received from the client
* @return void
*/
void Server::handleInvite(const int &clientFd, const std::string &line) {
	const std::string toInvite = getParam(INVITE_CMD_LENGTH, line);
	bool userExist = false;

	if (toInvite.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, CMD_INVITE);
		return ;
	}

	for (std::map<int, User>::iterator it = this->Users.begin(); it != this->Users.end(); ++it) {
		if (it->second.getNickname() == toInvite) {
			userExist = true;
			break ;
		}
	}

	if (!userExist) {
		sendRPL(clientFd, ERR_NOSUCHNICK, this->Users[clientFd].getNickname(), MSG_ERR_NOSUCHNICK);
		return ;
	}

	const std::string channelName = getChannelName(line);

	if (!channelExists(channelName)) {
		sendERR_NOSUCHCHANNEL(clientFd, channelName);
		return ;
	}


	processToInvite(clientFd, toInvite, findChannelByName(channelName));
}

/*
* Process the invitation logic for a user to a channel
* @param clientFd - The file descriptor of the client sending the INVITE command
* @param toInvite - The nickname of the user to be invited
* @param channel - Reference to the Channel object where the user is to be invited
* @return void
*/
void Server::processToInvite(const int &clientFd, const std::string &toInvite, Channel &channel) {
	if (channel.getInviteOnly()) {
		if (!channel.isOperator(clientFd)) {
			sendERR_CHANOPRIVSNEEDED(clientFd, channel.getName());
		} else if (channel.isMember(this->findIdByName(toInvite))) {
			sendRPL(clientFd, ERR_USERONCHANNEL, toInvite, toInvite + " " + channel.getName() + " :" + MSG_ERR_USERONCHANNEL);
		} else {
			notifyInvite(clientFd, toInvite, channel);
			sendRPL_INVITED(clientFd, toInvite, channel);
		}
		return ;
	} else {
		if (channel.isMember(this->findIdByName(toInvite))) {
			sendRPL(clientFd, ERR_USERONCHANNEL, toInvite, toInvite + " " + channel.getName() + " :" + MSG_ERR_USERONCHANNEL);
			return ;
		}
		notifyInvite(clientFd, toInvite, channel);
		sendRPL_INVITED(clientFd, toInvite, channel);
	}
}

/*
* Notify a user that they have been invited to a channel
* @param clientFd - The file descriptor of the client sending the INVITE command
* @param toInvite - The nickname of the user to be invited
* @param channel - Reference to the Channel object where the user is to be invited
* @return void
*/
void Server::notifyInvite(const int &clientFd, const std::string &toInvite, Channel &channel) {
	std::string buffer(":");

	buffer += this->Users[clientFd].getNickname() + "!";
	buffer += this->Users[clientFd].getUsername() +"@";
	buffer +=  "localhost INVITE ";
	buffer += toInvite + " :";
	buffer += channel.getName() + "\r\n";

	channel.invite(this->findIdByName(toInvite));
	send(this->findIdByName(toInvite), buffer.c_str(), buffer.size(), 0);
}