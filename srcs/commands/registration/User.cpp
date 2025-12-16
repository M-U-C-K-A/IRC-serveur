/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:00:00 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 04:57:48 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* this fonction will handle the USER command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleUsername(const int &clientFd, const std::string &line) {
	User &user = this->Users[clientFd];

	// Check if already registered
	if (user.getIsRegister()) {
		sendERR_ALREADYREGISTRED(clientFd);
		return;
	}

	// Parse USER command: USER <username> <hostname> <servername> :<realname>
	std::string params = line.substr(5); // Skip "USER "
	if (params.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, "USER");
		return;
	}

	// Extract username (first parameter)
	size_t spacePos = params.find(' ');
	std::string username = (spacePos != std::string::npos) ? params.substr(0, spacePos) : params;
	
	if (username.empty()) {
		sendERR_NEEDMOREPARAMS(clientFd, "USER");
		return;
	}

	// Set username
	user.setUsername(username);
	user.setHasUsername();

	std::cout << "[IRC] User " << clientFd << " set username: " << username << std::endl;

	// Try to complete registration (will check if PASS and NICK are also set)
	user.tryRegisterUser();
	
	// If now registered, send welcome
	if (user.getIsRegister() && !user.getWelcomeMessage()) {
		checkUserRegistration(clientFd);
	}
}
