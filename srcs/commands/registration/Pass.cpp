/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:08:42 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:28:29 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* this fonction will handle the PASS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handlePass(const int &clientFd, const std::string &line) {
	// Check if already registered
	if (this->Users[clientFd].getIsRegister()) {
		sendERR_ALREADYREGISTRED(clientFd);
		return;
	}

	// Extract password from command
	std::string password = line.substr(5); // Skip "PASS "
	if (!password.empty() && password[0] == ':')
		password.erase(0, 1);

	// Validate password
	if (password.empty() || password != this->password) {
		sendERR_PASSWDMISMATCH(clientFd);
		return;
	}

	// Password is correct - mark as having valid password
	this->Users[clientFd].setHasPass();
	std::cout << "Password set for user " << clientFd << std::endl;
}

/*
** ============================================================================
**                           PASS COMMAND
** ============================================================================
**
**  Format: PASS <password>
**
**  Flow: Connect → PASS → [Validate] → Store/Error
**  Rules: Must send BEFORE registration (NICK/USER).
**
** ============================================================================
*/
